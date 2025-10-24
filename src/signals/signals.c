/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 14:27:36 by acossari          #+#    #+#             */
/*   Updated: 2025/10/21 11:30:17 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/*
 * Global signal flag (THE ONLY GLOBAL in the project)
 *
 * Defined here, declared as 'extern' in minishell.h for other files
 *
 * Used across all signal contexts (prompt, heredoc, exec)
 * Value: 0 = no signal, SIGINT (2), SIGQUIT (3), etc.
 *
 * Must be reset to 0 after handling signal in each context
 */
volatile sig_atomic_t	g_signal_received = 0;

/**
 * Setup signals for execution in the parent process
 * SIGINT: Ignored (child will handle it)
 * SIGQUIT: Ignored (child will handle it)
 */
void	setup_exec_signals(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

/**
 * Resets signals to default behavior for child processes
 * SIGINT: Default behavior (terminate process)
 * SIGQUIT: Default behavior (terminate process and create core dump)
 */
void	reset_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}
