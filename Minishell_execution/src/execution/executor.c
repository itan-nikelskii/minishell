/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 10:23:14 by acossari          #+#    #+#             */
/*   Updated: 2025/10/23 13:50:19 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Main execution function
 * Decides if command is single, 2-command pipeline, or multi-command pipeline
 * @param cmd Command list to execute
 * @param shell Shell state
 * @return Exit status of last command
 */
int	execute_command(t_command *cmd, t_shell *shell)
{
	int	cmd_count;

	if (!cmd || !shell)
		return (1);
	cmd_count = count_commands(cmd);
	if (cmd_count == 1)
		return (execute_single_command(cmd, shell));
	else if (cmd_count == 2)
		return (execute_pipeline(cmd, shell));
	else
		return (execute_pipeline_multi(cmd, shell));
}

/**
 * Execute builtin with redirections
 * @param cmd Command to execute
 * @param shell Shell state
 * @return Exit status
 */
static int	exec_builtin_with_redir(t_command *cmd, t_shell *shell)
{
	int	in_fd;
	int	out_fd;
	int	exit_status;

	if (setup_redirections(cmd->redirs, &in_fd, &out_fd, shell) == -1)
	{
		if (g_signal_received == SIGINT)
			return (130);
		return (1);
	}
	if (save_std_fds(shell) == -1)
	{
		if (in_fd != STDIN_FILENO)
			close(in_fd);
		if (out_fd != STDOUT_FILENO)
			close(out_fd);
		return (1);
	}
	if (apply_redirections(in_fd, out_fd) == -1)
		return (restore_std_fds(shell), 1);
	exit_status = execute_builtin(cmd, shell);
	restore_std_fds(shell);
	return (exit_status);
}

/**
 * Executes a single command (no pipeline)
 * Distinguishes between builtins and external commands
 *
 * @param cmd Command to execute
 * @param shell Shell state
 * @return Exit status
 */
int	execute_single_command(t_command *cmd, t_shell *shell)
{
	int	exit_status;

	if (!cmd || !cmd->argv || !cmd->argv[0])
		return (0);
	if (is_builtin(cmd->argv[0]))
	{
		if (cmd->redirs)
			exit_status = exec_builtin_with_redir(cmd, shell);
		else
			exit_status = execute_builtin(cmd, shell);
	}
	else
		exit_status = execute_external(cmd, shell);
	shell->last_exit_status = exit_status;
	return (exit_status);
}
