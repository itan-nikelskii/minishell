/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_dispatcher.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 09:38:26 by acossari          #+#    #+#             */
/*   Updated: 2025/11/11 13:12:26 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Checks if command is a builtin
 * @param cmd Command name to check
 * @return 1 if builtin, 0 otherwise
 */
int	is_builtin(char *cmd)
{
	if (strcmp(cmd, "pwd") == 0)
		return (1);
	if (strcmp(cmd, "echo") == 0)
		return (1);
	if (strcmp(cmd, "env") == 0)
		return (1);
	if (strcmp(cmd, "cd") == 0)
		return (1);
	if (strcmp(cmd, "export") == 0)
		return (1);
	if (strcmp(cmd, "unset") == 0)
		return (1);
	if (strcmp(cmd, "exit") == 0)
		return (1);
	return (0);
}

/**
 * Core builtin dispatcher (no FD manipulation)
 * @param cmd Command structure
 * @param shell Shell state
 * @return Exit status
 */
int	exec_builtin(t_command *cmd, t_shell *shell)
{
	if (strcmp(cmd->argv[0], "pwd") == 0)
		return (builtin_pwd(cmd, shell));
	if (strcmp(cmd->argv[0], "echo") == 0)
		return (builtin_echo(cmd, shell));
	if (strcmp(cmd->argv[0], "env") == 0)
		return (builtin_env(cmd, shell));
	if (strcmp(cmd->argv[0], "cd") == 0)
		return (builtin_cd(cmd, shell));
	if (strcmp(cmd->argv[0], "export") == 0)
		return (builtin_export(cmd, shell));
	if (strcmp(cmd->argv[0], "unset") == 0)
		return (builtin_unset(cmd, shell));
	if (strcmp(cmd->argv[0], "exit") == 0)
		return (builtin_exit(cmd, shell));
	return (1);
}

/**
 * Execute builtin in parent process without redirections
 * Direct wrapper around exec_builtin()
 * @param cmd Command to execute
 * @param shell Shell state
 * @return Exit status
 */
int	exec_builtin_in_parent_direct(t_command *cmd, t_shell *shell)
{
	return (exec_builtin(cmd, shell));
}
