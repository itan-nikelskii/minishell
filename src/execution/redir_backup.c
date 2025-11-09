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
