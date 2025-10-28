/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   external_executor.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 11:42:33 by acossari          #+#    #+#             */
/*   Updated: 2025/10/28 18:30:06 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Executes an external command using fork + execve
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
int	execute_external(t_command *cmd, t_shell *shell)
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
