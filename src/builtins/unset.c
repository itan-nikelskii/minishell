/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 19:31:59 by antoniocoss       #+#    #+#             */
/*   Updated: 2025/11/07 20:04:29 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Handle invalid options for unset (FIX 47)
 * @param arg The argument to check
 * @param shell Shell state to set exit status
 * @return 1 if invalid option detected, 0 otherwise
 *
 * Behavior:
 * - "-" (single dash alone): return 0 (valid name)
 * - "--" (double dash alone): return 0 (valid name)
 * - "-X", "--X", etc.: print error, set exit 2, return 1
 */
static int	handle_unset_option(const char *arg, t_shell *shell)
{
	if (arg[0] == '-' && arg[1] != '\0')
	{
		if (arg[1] == '-' && arg[2] == '\0')
			return (0);
		print_error_arg("unset", (char *)arg, "invalid option");
		ft_putendl_fd("unset: usage: unset [name ...]", STDERR_FILENO);
		shell->last_exit_status = 2;
		return (1);
	}
	return (0);
}

/**
 * Unset environment variables
 * @param cmd Command structure with arguments
 * @param shell Shell state
 * @return 0 on success, 2 on invalid option, 1 on malloc failure
 *
 * Behavior (bash compatible):
 * - No arguments: return 0
 * - Invalid options (-, --): print error and return 2
 * - All names (valid or invalid): silently try to remove, no error messages
 * - Only malloc failure returns 1
 */
int	builtin_unset(t_command *cmd, t_shell *shell)
{
	int	i;

	i = 1;
	while (cmd->argv[i])
	{
		if (handle_unset_option(cmd->argv[i], shell))
			return (2);
		if (env_remove(shell, cmd->argv[i]) == -1)
			return (1);
		if (xenv_remove(shell, cmd->argv[i]) == -1)
			return (1);
		i++;
	}
	return (0);
}
