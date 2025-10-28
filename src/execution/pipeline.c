/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 09:23:46 by acossari          #+#    #+#             */
/*   Updated: 2025/10/28 20:00:28 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Fork and execute first command in pipeline
 * @param cmd First command
 * @param pipefd Pipe file descriptors
 * @param shell Shell state
 * @return PID of child, or -1 on error
 */
static pid_t	fork_exec_first(t_command *cmd, int pipefd[2], t_shell *shell)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0)
	{
		close(pipefd[0]);
		if (dup2(pipefd[1], STDOUT_FILENO) == -1)
		{
			print_perror("dup2", NULL);
			exit(EXIT_FAILURE);
		}
		close(pipefd[1]);
		exec_child_piped(cmd, shell);
	}
	return (pid);
}

/**
 * Fork and execute second command in pipeline
 * @param cmd Second command
 * @param pipefd Pipe file descriptors
 * @param shell Shell state
 * @return PID of child, or -1 on error
 */
static pid_t	fork_exec_second(t_command *cmd, int pipefd[2], t_shell *shell)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0)
	{
		close(pipefd[1]);
		if (dup2(pipefd[0], STDIN_FILENO) == -1)
		{
			print_perror("dup2", NULL);
			exit(EXIT_FAILURE);
		}
		close(pipefd[0]);
		exec_child_piped(cmd, shell);
	}
	return (pid);
}

/*
** Pipeline semantics:
**   In each child: FIRST hook the pipe with dup2; THEN apply the
**   command's redirections; finally run builtin (exit(status)) or execve.
**   Builtins are executed in the child processes so that they do not affect
**   the parent process's state.
**
** Why this order:
**   Redirections belong to the command and may overwrite the pipe FDs.
**   Example: `echo hi > out | cat`
**     C1: dup2(pipe[1], 1);          // stdout -> pipe write end
**         > out => dup2(outfd, 1);   // overwrites pipe with file
**   Result: file "out" gets "hi"; C2 (cat) reads EOF and prints nothing.
*/

/**
 * Execute pipeline of 2 commands
 * @param cmd_list Linked list of commands (must have 2 commands)
 * @param shell Shell state
 * @return Exit status of last command
 */
int	execute_pipeline(t_command *cmd_list, t_shell *shell)
{
	int		pipefd[2];
	pid_t	pids[2];
	int		status;

	if (pipe(pipefd) == -1)
		return (print_perror("pipe", NULL), EXIT_FAILURE);
	setup_parent_wait_signals();
	pids[0] = fork_exec_first(cmd_list, pipefd, shell);
	if (pids[0] == -1)
		return (setup_parent_ps1_signals(), cleanup_pipes(pipefd),
			print_perror("fork", NULL), EXIT_FAILURE);
	pids[1] = fork_exec_second(cmd_list->next, pipefd, shell);
	if (pids[1] == -1)
		return (cleanup_pipes(pipefd), waitpid(pids[0], NULL, 0),
			setup_parent_ps1_signals(), print_perror("fork", NULL),
			EXIT_FAILURE);
	cleanup_pipes(pipefd);
	status = wait_all_children(pids, 2);
	setup_parent_ps1_signals();
	return (get_child_exit_status(status));
}
