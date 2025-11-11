/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 10:23:14 by acossari          #+#    #+#             */
/*   Updated: 2025/11/11 15:04:59 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Prepare all heredocs for a list of commands
 * @param cmd_list Command list
 * @param shell Shell state
 * @return 0 on success, 130 on SIGINT, -1 on error
 */
static int	prepare_all_heredocs(t_command *cmd_list, t_shell *shell)
{
	t_command	*cmd;
	int			result;

	cmd = cmd_list;
	while (cmd)
	{
		result = prepare_heredocs(cmd, shell);
		if (result == -2)
			return (-2);
		if (result != 0)
			return (result);
		cmd = cmd->next;
	}
	return (0);
}

/**
 * Execute command(s), handling pipelines and heredocs
 * @param cmd Command list to execute
 * @param shell Shell state
 * @return Exit status of last command
 */
int	execute_command(t_command *cmd, t_shell *shell)
{
	int	cmd_count;
	int	exit_status;
	int	result;

	result = prepare_all_heredocs(cmd, shell);
	if (result == -2)
	{
		shell->last_exit_status = 2;
		exit(2);
	}
	if (result == 130)
		return (shell->last_exit_status = 130);
	if (result != 0)
		return (shell->last_exit_status = 1);
	cmd_count = count_commands(cmd);
	if (cmd_count == 1)
		exit_status = execute_single_command(cmd, shell);
	else
		exit_status = execute_pipeline(cmd, shell);
	shell->last_exit_status = exit_status;
	return (exit_status);
}

/**
 * Execute builtin in parent process with redirections
 * Handles redirections by saving/restoring stdin/stdout
 * @param cmd Command to execute (must have argv)
 * @param shell Shell state
 * @return Exit status
 */
static int	exec_builtin_in_parent_with_redir(t_command *cmd, t_shell *shell)
{
	int	in_fd;
	int	out_fd;
	int	exit_status;

	if (setup_redirections(cmd->redirs, &in_fd, &out_fd) == -1)
	{
		if (g_signal_received == SIGINT)
			return (130);
		return (1);
	}
	if (save_std_fds(shell) == -1)
		return (cleanup_redir_fds(in_fd, out_fd), 1);
	if (apply_redirections(in_fd, out_fd) == -1)
		return (restore_std_fds(shell), 1);
	exit_status = exec_builtin(cmd, shell);
	restore_std_fds(shell);
	return (exit_status);
}

/**
 * Execute external command in child process
 * @param cmd Command to execute
 * @param shell Shell state
 * @return Exit status of the command
 */
int	exec_external_in_child(t_command *cmd, t_shell *shell)
{
	pid_t	pid;
	int		status;
	int		exit_status;

	setup_parent_wait_signals();
	pid = fork();
	if (pid == -1)
	{
		setup_parent_ps1_signals();
		print_perror("fork", NULL);
		return (EXIT_FAILURE);
	}
	if (pid == 0)
		exec_command_in_child(cmd, shell);
	waitpid(pid, &status, 0);
	print_signal_message(status);
	setup_parent_ps1_signals();
	exit_status = get_child_exit_status(status);
	return (exit_status);
}

/**
 * Executes a single command (no pipeline)
 * Handles:
 *   - Builtins with/without redirections
 *   - External commands
 *   - Redirections-only (no command, e.g., >file)
 *   - Empty commands (e.g., '')
 * @param cmd Command to execute (argv may be NULL for redirections-only)
 * @param shell Shell state
 * @return Exit status
 */
int	execute_single_command(t_command *cmd, t_shell *shell)
{
	if (!cmd->argv || !cmd->argv[0])
	{
		if (cmd->redirs)
			return (apply_redirections_in_parent(cmd->redirs, shell));
		return (0);
	}
	if (!cmd->argv[0][0])
	{
		print_error(cmd->argv[0], "command not found");
		return (CMD_NOT_FOUND);
	}
	if (is_builtin(cmd->argv[0]))
	{
		if (cmd->redirs)
			return (exec_builtin_in_parent_with_redir(cmd, shell));
		return (exec_builtin_in_parent_direct(cmd, shell));
	}
	return (exec_external_in_child(cmd, shell));
}
