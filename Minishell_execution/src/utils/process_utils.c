/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 14:37:52 by acossari          #+#    #+#             */
/*   Updated: 2025/10/21 13:02:01 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Map execve errno to shell-compatible exit codes
 * @return Exit code corresponding to errno
 */
int	map_execve_errno(void)
{
	if (errno == EACCES || errno == ENOEXEC || errno == EISDIR)
		return (CMD_NOT_EXECUTABLE);
	if (errno == ENOENT)
		return (CMD_NOT_FOUND);
	return (EXIT_FAILURE);
}

/**
 * Extract exit status from child process status
 * Handles both normal exit and signal termination
 * @param status Status returned by waitpid()
 * @return Exit code (0-255 for normal exit, 128+signal for signals)
 */
int	get_child_exit_status(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (EXIT_FAILURE);
}
