/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_prompt.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 18:17:52 by acossari          #+#    #+#             */
/*   Updated: 2025/10/24 22:16:19 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Signal handler for SIGINT during interactive prompt
 * @param sig: Signal number (unused)
 *
 * Forces readline to return immediately by setting rl_done
 * This allows Ctrl+C to abort continuation prompts immediately
 */
static void	prompt_sigint_handler(int sig)
{
	(void)sig;
	g_signal_received = SIGINT;
}

/**
 * Event hook called by readline during input
 * Checks if SIGINT was received and forces readline to abort
 * @return 0 to continue, non-zero would trigger special behavior
 */
static int	check_signal_hook(void)
{
	if (g_signal_received == SIGINT)
		rl_done = 1;
	return (0);
}

/**
 * Setup signals for interactive prompt
 * SIGINT: Custom handler to interrupt input
 * SIGQUIT: Ignored
 */
void	setup_prompt_signals(void)
{
	struct sigaction	sa;

	rl_catch_signals = 0;
	ft_bzero(&sa, sizeof(sa));
	sa.sa_handler = prompt_sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
	rl_event_hook = check_signal_hook;
}
