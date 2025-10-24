/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 11:09:53 by acossari          #+#    #+#             */
/*   Updated: 2025/10/23 13:50:19 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Check if a string is a valid -n flag for echo
 * Valid flags are: -n, -nn, -nnn, ...
 * @param s String to check
 * @return 1 if valid -n flag, 0 otherwise
 */
static int	is_nflag(const char *s)
{
	int	i;

	if (!s || s[0] != '-' || s[1] == '\0')
		return (0);
	i = 1;
	while (s[i] == 'n')
		i++;
	return (s[i] == '\0');
}

/**
 * Builtin: echo
 * Prints arguments separated by space, with optional newline
 * @param cmd Command structure with argv
 * @param shell Shell state (unused)
 * @return Always 0 (echo never fails)
 */
int	builtin_echo(t_command *cmd, t_shell *shell)
{
	int	i;
	int	newline;

	(void)shell;
	i = 1;
	newline = 1;
	while (cmd->argv[i] && is_nflag(cmd->argv[i]))
	{
		newline = 0;
		i++;
	}
	while (cmd->argv[i])
	{
		ft_putstr_fd(cmd->argv[i], STDOUT_FILENO);
		if (cmd->argv[i + 1])
			ft_putstr_fd(" ", STDOUT_FILENO);
		i++;
	}
	if (newline)
		ft_putstr_fd("\n", STDOUT_FILENO);
	return (0);
}
