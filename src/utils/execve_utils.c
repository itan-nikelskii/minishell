/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execve_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 16:27:35 by acossari          #+#    #+#             */
/*   Updated: 2025/10/23 13:58:18 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Execute command directly with execve (no fork)
 * This function never returns on success
 * Used in child processes after fork
 * @param cmd Command to execute
 * @param shell Shell state with envp
 */
void	execve_or_die(t_command *cmd, t_shell *shell)
{
	char	*path;

	if (!cmd->argv[0] || !*cmd->argv[0])
	{
		ft_putstr_fd("command not found\n", STDERR_FILENO);
		exit(CMD_NOT_FOUND);
	}
	path = resolve_path(cmd->argv[0], shell->envp);
	if (!path)
	{
		ft_putstr_fd(cmd->argv[0], STDERR_FILENO);
		ft_putstr_fd(": command not found\n", STDERR_FILENO);
		exit(CMD_NOT_FOUND);
	}
	execve(path, cmd->argv, shell->envp);
	print_perror(cmd->argv[0], NULL);
	if (path != cmd->argv[0])
		free(path);
	exit(map_execve_errno());
}

/**
 * Unified function to execute command in child process
 * Handles redirections, builtins (if allowed), and external commands
 * This function never returns - it always exits
 * Called from fork in already-forked child (pipe dup2 already done by caller)
 * @param cmd Command to execute
 * @param shell Shell state
 * @param allow_builtin If true, executes builtins; if false, treats as external
 */
static void	exec_command_in_child(t_command *cmd, t_shell *shell,
	int allow_builtin)
{
	int	in_fd;
	int	out_fd;

	reset_signals();
	shell->in_child = true;
	in_fd = STDIN_FILENO;
	out_fd = STDOUT_FILENO;
	if (cmd->redirs)
	{
		if (setup_redirections(cmd->redirs, &in_fd, &out_fd, shell) == -1)
		{
			if (shell->last_exit_status == 130)
				exit(130);
			exit(EXIT_FAILURE);
		}
		if (apply_redirections(in_fd, out_fd) == -1)
			exit(EXIT_FAILURE);
	}
	if (allow_builtin && is_builtin(cmd->argv[0]))
		exit(execute_builtin(cmd, shell));
	execve_or_die(cmd, shell);
}

/**
 * Execute command in child process for pipeline context
 * Allows builtins to be executed in child (won't affect parent state)
 * Pre: called in a forked child. Post: never returns
 * @param cmd Command to execute
 * @param shell Shell state
 */
void	exec_child_piped(t_command *cmd, t_shell *shell)
{
	exec_command_in_child(cmd, shell, 1);
}

/**
 * Execute command in child process for single external command
 * Builtins already handled in parent, this is for external only
 * Pre: called in a forked child. Post: never returns
 * @param cmd Command to execute
 * @param shell Shell state
 */
void	exec_child_single(t_command *cmd, t_shell *shell)
{
	exec_command_in_child(cmd, shell, 0);
}
