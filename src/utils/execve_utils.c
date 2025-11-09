/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execve_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 16:27:35 by acossari          #+#    #+#             */
/*   Updated: 2025/11/06 14:43:32 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Print appropriate error message when command path not found
 * @param cmd_name Command name
 * @param envp Environment variables
 */
static void	print_path_not_found_error(char *cmd_name, char **envp)
{
	char	*pathenv;

	pathenv = get_env_value("PATH", envp);
	if (pathenv == NULL || pathenv[0] == '\0')
		print_error(cmd_name, "No such file or directory");
	else
		print_error(cmd_name, "command not found");
}

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
	int		saved_errno;

	if (!cmd->argv[0] || !*cmd->argv[0])
	{
		print_error(NULL, "command not found");
		cleanup_and_exit(shell, CMD_NOT_FOUND);
	}
	path = resolve_path(cmd->argv[0], shell->envp);
	if (!path)
	{
		print_path_not_found_error(cmd->argv[0], shell->envp);
		cleanup_and_exit(shell, CMD_NOT_FOUND);
	}
	execve(path, cmd->argv, shell->envp);
	saved_errno = errno;
	print_perror(cmd->argv[0], NULL);
	if (path != cmd->argv[0])
		free(path);
	errno = saved_errno;
	cleanup_and_exit(shell, map_execve_errno());
}

/**
 * Unified function to execute command in child process
 * Handles redirections, builtins (if allowed), and external commands
 * Also handles redirections-only commands (no argv)
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
	int	status;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	shell->in_child = true;
	in_fd = STDIN_FILENO;
	out_fd = STDOUT_FILENO;
	if (cmd->redirs && setup_redirections(cmd->redirs, &in_fd, &out_fd) == -1)
	{
		if (shell->last_exit_status == 130)
			cleanup_and_exit(shell, 130);
		cleanup_and_exit(shell, EXIT_FAILURE);
	}
	if (cmd->redirs && apply_redirections(in_fd, out_fd) == -1)
		cleanup_and_exit(shell, EXIT_FAILURE);
	if (!cmd->argv || !cmd->argv[0])
		cleanup_and_exit(shell, 0);
	if (allow_builtin && is_builtin(cmd->argv[0]))
	{
		status = execute_builtin(cmd, shell);
		cleanup_and_exit(shell, status);
	}
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
