/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:36:19 by acossari          #+#    #+#             */
/*   Updated: 2025/11/06 20:42:49 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Builtin: env
 * Prints all environment variables
 * @param cmd Command structure
 * @param shell Shell state with envp
 * @return 0 on success, 127 if arguments provided (fix #31)
 */
int	builtin_env(t_command *cmd, t_shell *shell)
{
	int	i;

	if (!shell->envp)
		return (0);
	if (cmd->argv[1])
	{
		print_error_arg("env", cmd->argv[1], "No such file or directory");
		return (127);
	}
	i = 0;
	while (shell->envp[i])
	{
		if (ft_strchr(shell->envp[i], '='))
			ft_putendl_fd(shell->envp[i], STDOUT_FILENO);
		i++;
	}
	return (0);
}
