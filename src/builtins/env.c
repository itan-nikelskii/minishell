/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:36:19 by acossari          #+#    #+#             */
/*   Updated: 2025/11/06 14:37:33 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Builtin: env
 * Prints all environment variables
 * @param cmd Command structure (unused)
 * @param shell Shell state with envp
 * @return Always 0 (env never fails)
 */
int	builtin_env(t_command *cmd, t_shell *shell)
{
	int	i;

	if (!shell->envp)
		return (0);
    // MODIFIED: if an argument is provided, report "No such file or directory"
	// and return 127 (fix for #31). I really hope this doesn't break anything
    if (cmd && cmd->argv && cmd->argv[1])
    {
        errno = ENOENT;
        print_perror("env", cmd->argv[1]);
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
