/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_parent_wait.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/27 11:30:00 by acossari          #+#    #+#             */
/*   Updated: 2025/10/27 11:41:33 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Setup signals for parent waiting on child (heredoc/exec)
 * SIGINT: SIG_IGN (no handler, no rl_redisplay!)
 * SIGQUIT: SIG_IGN
 *
 * CRITICAL: This prevents the prompt handler from calling rl_redisplay()
 * while the parent is NOT in a readline() call (during waitpid).
 * Without this, Ctrl+C in heredoc causes double prompt.
 *
 * Signal profile: PARENT-WAIT
 * - Used when parent is waiting for child (heredoc or exec)
 * - Ignores SIGINT completely - child handles it
 * - Must restore prompt signals after waitpid()
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
