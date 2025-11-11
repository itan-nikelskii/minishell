/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execve_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 16:27:35 by acossari          #+#    #+#             */
/*   Updated: 2025/11/11 19:06:38 by antoniocoss      ###   ########.fr       */
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
	if (!pathenv || !pathenv[0])
		print_error(cmd_name, "No such file or directory");
	else
		print_error(cmd_name, "command not found");
}

/**
 * Execute command directly with execve
 * This function never returns on success
 * Used in child processes after fork
 * @param cmd Command to execute
 * @param shell Shell state with envp
 */
void	execve_or_die(t_command *cmd, t_shell *shell)
{
	char	*path;
	int		saved_errno;

	if (!cmd->argv[0][0])
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
 * Execute command in child process (never returns, always exits)
 * Handles: redirections, builtins, external commands
 * Note: Redirections-only (no argv) handled here for pipelines only
 *       (single commands handle this in parent - see execute_single_command)
 * @param cmd Command to execute (argv may be NULL in pipeline context)
 * @param shell Shell state
 */
void	exec_command_in_child(t_command *cmd, t_shell *shell)
{
	int	in_fd;
	int	out_fd;
	int	status;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	shell->in_child = true;
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
	if (is_builtin(cmd->argv[0]))
	{
		status = exec_builtin(cmd, shell);
		cleanup_and_exit(shell, status);
	}
	execve_or_die(cmd, shell);
}
