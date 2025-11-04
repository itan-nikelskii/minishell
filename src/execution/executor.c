/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 10:23:14 by acossari          #+#    #+#             */
/*   Updated: 2025/11/04 11:54:39 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Prepare all heredocs for command list
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
 * Main execution function
 * Decides if command is single or pipeline
 * @param cmd Command list to execute
 * @param shell Shell state
 * @return Exit status of last command
 */
int	execute_command(t_command *cmd, t_shell *shell)
{
	int	cmd_count;
	int	exit_status;
	int	result;

	if (!cmd || !shell)
		return (1);
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
 * Execute builtin in parent process
 * Handles redirections by saving/restoring stdin/stdout
 * Also handles redirections-only commands (no argv)
 * @param cmd Command to execute
 * @param shell Shell state
 * @return Exit status
 */
static int	exec_builtin_in_parent(t_command *cmd, t_shell *shell)
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
	{
		if (in_fd != STDIN_FILENO)
			close(in_fd);
		if (out_fd != STDOUT_FILENO)
			close(out_fd);
		return (1);
	}
	if (apply_redirections(in_fd, out_fd) == -1)
		return (restore_std_fds(shell), 1);
	if (cmd->argv && cmd->argv[0])
		exit_status = execute_builtin(cmd, shell);
	else
		exit_status = 0;
	restore_std_fds(shell);
	return (exit_status);
}

/**
 * Executes a single command (no pipeline)
 * Handles:
 *   - Builtins with/without redirections
 *   - External commands
 *   - Redirections-only (no command, e.g., >file)
 *
 * @param cmd Command to execute (argv may be NULL for redirections-only)
 * @param shell Shell state
 * @return Exit status
 */
int	execute_single_command(t_command *cmd, t_shell *shell)
{
	int	exit_status;

	if (!cmd)
		return (0);
	if (!cmd->argv || !cmd->argv[0])
	{
		if (cmd->redirs)
			return (exec_builtin_in_parent(cmd, shell));
		return (0);
	}
	if (is_builtin(cmd->argv[0]))
	{
		if (cmd->redirs)
			exit_status = exec_builtin_in_parent(cmd, shell);
		else
			exit_status = execute_builtin(cmd, shell);
	}
	else
		exit_status = exec_external_in_child(cmd, shell);
	return (exit_status);
}

/**
 * Execute external command in child process (fork + execve)
 *
 * Process flow:
 * 1. PARENT: fork() creates child process
 * 2. CHILD: exec_child_single() handles redirections and execve
 * 3. PARENT: waitpid() waits for child → returns exit status
 *
 * @param cmd Command to execute
 * @param shell Shell state
 * @return Exit status of the command
 */
int	exec_external_in_child(t_command *cmd, t_shell *shell)
{
	pid_t	pid;
	int		status;

	setup_parent_wait_signals();
	pid = fork();
	if (pid == -1)
	{
		setup_parent_ps1_signals();
		print_perror("fork", NULL);
		return (EXIT_FAILURE);
	}
	if (pid == 0)
		exec_child_single(cmd, shell);
	waitpid(pid, &status, 0);
	if (isatty(STDIN_FILENO) && WIFSIGNALED(status)
		&& WTERMSIG(status) == SIGINT)
		write(STDOUT_FILENO, "\n", 1);
	setup_parent_ps1_signals();
	return (get_child_exit_status(status));
}
