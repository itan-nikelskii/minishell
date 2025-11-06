/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 19:31:59 by antoniocoss       #+#    #+#             */
/*   Updated: 2025/11/06 19:33:33 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Unset environment variables
 * @param cmd Command structure with arguments
 * @param shell Shell state
 * @return 0 on success, 1 on malloc failure
 *
 * Behavior (bash compatible):
 * - No arguments: return 0
 * - All names (valid or invalid): silently try to remove, no error messages
 * - Only malloc failure returns 1
 */
int	builtin_unset(t_command *cmd, t_shell *shell)
{
	int	i;

	i = 1;
	while (cmd->argv[i])
	{
		if (env_remove(shell, cmd->argv[i]) == -1)
			return (1);
		if (xenv_remove(shell, cmd->argv[i]) == -1)
			return (1);
		i++;
	}
	return (0);
}
