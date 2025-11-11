/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections_backup.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 10:52:39 by acossari          #+#    #+#             */
/*   Updated: 2025/11/04 10:17:03 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Save standard file descriptors
 * @param shell Shell state
 * @return 0 on success, -1 on error
 */
int	save_std_fds(t_shell *shell)
{
	shell->stdin_backup = dup(STDIN_FILENO);
	if (shell->stdin_backup == -1)
	{
		perror("minishell: dup");
		return (-1);
	}
	shell->stdout_backup = dup(STDOUT_FILENO);
	if (shell->stdout_backup == -1)
	{
		perror("minishell: dup");
		close(shell->stdin_backup);
		return (-1);
	}
	return (0);
}

/**
 * Restore standard file descriptors
 * @param shell Shell state
 * @return 0 on success, -1 on error
 */
int	restore_std_fds(t_shell *shell)
{
	int	status;

	status = 0;
	if (shell->stdin_backup != -1)
	{
		if (dup2(shell->stdin_backup, STDIN_FILENO) == -1)
			status = -1;
		close(shell->stdin_backup);
		shell->stdin_backup = -1;
	}
	if (shell->stdout_backup != -1)
	{
		if (dup2(shell->stdout_backup, STDOUT_FILENO) == -1)
			status = -1;
		close(shell->stdout_backup);
		shell->stdout_backup = -1;
	}
	return (status);
}

/**
 * Apply redirections in parent process (redirections-only commands)
 * Saves/restores stdin/stdout around redirection application
 * Used for commands like: > file, < in > out, etc.
 * @param redirs Redirection list to apply
 * @param shell Shell state
 * @return Exit status (0 on success, 1 on error, 130 on SIGINT)
 */
int	apply_redirections_in_parent(t_redir *redirs, t_shell *shell)
{
	int	in_fd;
	int	out_fd;

	if (setup_redirections(redirs, &in_fd, &out_fd) == -1)
	{
		if (g_signal_received == SIGINT)
			return (130);
		return (1);
	}
	if (save_std_fds(shell) == -1)
		return (cleanup_redir_fds(in_fd, out_fd), 1);
	if (apply_redirections(in_fd, out_fd) == -1)
		return (restore_std_fds(shell), 1);
	restore_std_fds(shell);
	return (0);
}
