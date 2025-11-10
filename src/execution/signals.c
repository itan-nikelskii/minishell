/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 14:27:36 by acossari          #+#    #+#             */
/*   Updated: 2025/11/10 19:36:22 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/*
 * Global signal flag (THE ONLY GLOBAL in the project)
 * Defined here, declared as 'extern' in minishell.h for other files
 * Used across all signal contexts (prompt, heredoc, exec)
 * Value: 0 = no signal, SIGINT (2), SIGQUIT (3), etc.
 * Must be reset to 0 after handling signal in each context
 *
 * volatile: Prevents compiler optimizations that assume
 *   the variable doesn't change unexpectedly
 * sig_atomic_t: Guarantees atomic (uninterruptible) read/write operations,
 *   safe for signal handlers
 * extern declaration: Avoids multiple definitions by defining here
 *   and declaring in header
 */
volatile sig_atomic_t	g_signal_received = 0;

/**
 * Signal handler for SIGINT during interactive prompt
 * @param sig: Signal number received from kernel
 *
 * Subject-compliant solution using only allowed readline functions.
 * Avoids rl_done (not permitted by subject) which would terminate
 *   readline loop.
 *
 * Functions used:
 * - rl_on_new_line(): Moves cursor to new line
 *      (needed even with rl_replace_line to ensure proper positioning
 *      before redisplay)
 * - rl_replace_line("", 0): Clears current input line
 * - rl_redisplay(): Redraws the prompt and cleared line
 */
static void	prompt_sigint_handler(int sig)
{
	g_signal_received = sig;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

/**
 * Signal handler for SIGINT in child process with PS2 prompt
 * Used by: heredoc (<<), trailing pipe continuation (|)
 * @param sig: Signal number received from kernel
 */
static void	child_sigint_handler(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	exit(130);
}

/**
 * Setup signals for parent at PS1 prompt (primary prompt)
 * SIGINT: Custom handler using only allowed readline functions
 * SIGQUIT: Ignored
 */
void	setup_parent_ps1_signals(void)
{
	struct sigaction	sa;

	ft_bzero(&sa, sizeof(sa));
	sa.sa_handler = prompt_sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}

/**
 * Setup signals for parent waiting on child (heredoc/exec)
 * SIGINT: SIG_IGN (no handler, no rl_redisplay!)
 * SIGQUIT: SIG_IGN
 * CRITICAL: Prevents double prompt during Ctrl+C in heredoc.
 */
void	setup_parent_wait_signals(void)
{
	struct sigaction	sa;

	ft_bzero(&sa, sizeof(sa));
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}

/**
 * Setup signals for child process with PS2 prompt (heredoc, continuation)
 * SIGINT: Custom handler that writes newline and exits with status 130
 * SIGQUIT: Ignored
 */
void	setup_child_ps2_signals(void)
{
	struct sigaction	sa;

	ft_bzero(&sa, sizeof(sa));
	sa.sa_handler = child_sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}
