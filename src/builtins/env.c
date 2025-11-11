/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:36:19 by acossari          #+#    #+#             */
/*   Updated: 2025/11/11 21:52:54 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Builtin: env
 * Prints all environment variables with values
 * @param cmd Command structure (unused)
 * @param shell Shell state with envp
 * @return 0 on success
 */
int	builtin_env(t_command *cmd, t_shell *shell)
{
	int	i;

	(void)cmd;
	if (!shell->envp)
		return (0);
	i = 0;
	while (shell->envp[i])
	{
		if (ft_strchr(shell->envp[i], '='))
			ft_putendl_fd(shell->envp[i], STDOUT_FILENO);
		i++;
	}
	return (0);
}
