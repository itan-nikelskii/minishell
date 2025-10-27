/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_prompt.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 18:17:52 by acossari          #+#    #+#             */
/*   Updated: 2025/10/27 11:39:40 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Signal handler for SIGINT during interactive prompt
 * @param sig: Signal number received from kernel
 *
 * Subject-compliant solution using only allowed readline functions:
 * - rl_on_new_line() - move to new line
 * - rl_replace_line() - clear input buffer
 * - rl_redisplay() - redisplay prompt
 *
 * This gives bash-like behavior (immediate return) WITHOUT using
 * forbidden functions (rl_done, rl_event_hook, rl_catch_signals)
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
 * Setup signals for parent at PS1 prompt (primary prompt)
 * SIGINT: Custom handler using only allowed readline functions
 * SIGQUIT: Ignored
 *
 * Used when parent is in readline() at main prompt.
 * NO rl_event_hook, NO rl_catch_signals, NO rl_done
 * Readline's default signal handling (rl_catch_signals=1) combined
 * with our handler gives immediate Ctrl+C response
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
