/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_heredoc.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 16:52:07 by acossari          #+#    #+#             */
/*   Updated: 2025/10/21 18:57:46 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Signal handler for SIGINT during heredoc input
 * @param sig: Signal number (unused)
 */
static void	heredoc_sigint_handler(int sig)
{
	(void)sig;
	g_signal_received = SIGINT;
	write(STDOUT_FILENO, "\n", 1);
}

/*
** Line-by-line:
** - struct sigaction sa;
**   Create a sigaction struct used to register the SIGINT handler.
** - sa.sa_handler = heredoc_sigint_handler;
**   Set the callback for Ctrl+C. The handler only sets a global flag and
**   writes a newline (async-signal-safe).
** - sigemptyset(&sa.sa_mask);
**   Do not block any extra signals while the handler runs (clean mask).
**   Without this call, sa.sa_mask would contains garbage data, eventually
**   blocking random signals during the handler execution.
** - sa.sa_flags = 0;
**   No SA_RESTART: readline/syscalls are interrupted by SIGINT instead of
**   being auto-restarted, so heredoc can exit immediately.
** - sigaction(SIGINT, &sa, NULL);
**   Install the handler for SIGINT. The third parameter is NULL since we
**   don't need to save the old action.
** - signal(SIGQUIT, SIG_IGN);
**   Ignore Ctrl+\ during heredoc (no "Quit"/core dump), matching bash.
**
** After heredoc, call setup_prompt_signals() to restore the prompt policy.
*/

/**
 * Setup signals for heredoc input
 * SIGINT: Custom handler to interrupt heredoc
 * SIGQUIT: Ignored
 */
void	setup_heredoc_signals(void)
{
	struct sigaction	sa;

	ft_bzero(&sa, sizeof(sa));
	sa.sa_handler = heredoc_sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}
