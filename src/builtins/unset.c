/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 19:31:59 by antoniocoss       #+#    #+#             */
/*   Updated: 2025/11/11 21:49:07 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Unset environment variables
 * Silently removes all specified variable names from environment
 * @param cmd Command structure with arguments
 * @param shell Shell state
 * @return 0 on success, 1 on malloc failure
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
