/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_child.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 16:52:07 by acossari          #+#    #+#             */
/*   Updated: 2025/10/27 12:50:39 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Signal handler for SIGINT in child process with PS2 prompt
 * Used by: heredoc (<<), trailing pipe continuation (|)
 * @param sig: Signal number received from kernel
 *
 * Writes newline and exits immediately with status 130.
 * Parent detects exit status and handles cleanup.
 */
static void	child_sigint_handler(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	exit(130);
}

/*
** Line-by-line:
** - struct sigaction sa;
**   Create a sigaction struct used to register the SIGINT handler.
** - sa.sa_handler = child_sigint_handler;
**   Set the callback for Ctrl+C. The handler writes newline and exits.
** - sigemptyset(&sa.sa_mask);
**   Do not block any extra signals while the handler runs (clean mask).
**   Without this call, sa.sa_mask would contains garbage data, eventually
**   blocking random signals during the handler execution.
** - sa.sa_flags = 0;
**   No SA_RESTART: readline/syscalls are interrupted by SIGINT instead of
**   being auto-restarted, so the child can exit immediately.
** - sigaction(SIGINT, &sa, NULL);
**   Install the handler for SIGINT. The third parameter is NULL since we
**   don't need to save the old action.
** - signal(SIGQUIT, SIG_IGN);
**   Ignore Ctrl+\ in PS2 contexts (no "Quit"/core dump), matching bash.
*/

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
