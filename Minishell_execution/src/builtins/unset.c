/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 19:31:59 by antoniocoss       #+#    #+#             */
/*   Updated: 2025/10/23 13:50:19 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Unset environment variables
 * @param cmd Command structure with arguments
 * @param shell Shell state
 * @return 0 on success, 1 on error with invalid identifiers
 *
 * Behavior:
 * - No arguments: return 0
 * - Valid names: remove from both envp and xenv
 * - Invalid names: print error and set status=1, continue with others
 */
int	builtin_unset(t_command *cmd, t_shell *shell)
{
	int	i;
	int	status;

	if (!cmd->argv[1])
		return (0);
	status = 0;
	i = 1;
	while (cmd->argv[i])
	{
		if (!is_valid_export_identifier(cmd->argv[i])
			|| ft_strchr(cmd->argv[i], '=') != NULL)
		{
			print_error_arg("unset", cmd->argv[i], "not a valid identifier");
			status = 1;
		}
		else
		{
			if (env_remove(shell, cmd->argv[i]) == -1)
				return (1);
			if (xenv_remove(shell, cmd->argv[i]) == -1)
				return (1);
		}
		i++;
	}
	return (status);
}
