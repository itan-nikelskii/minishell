/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 19:52:21 by antoniocoss       #+#    #+#             */
/*   Updated: 2025/10/29 20:00:11 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Close redirection file descriptors if they are not stdin/stdout
 * @param in_fd Input file descriptor
 * @param out_fd Output file descriptor
 */
void	close_redir_fds(int in_fd, int out_fd)
{
	if (in_fd != STDIN_FILENO)
		close(in_fd);
	if (out_fd != STDOUT_FILENO)
		close(out_fd);
}

/**
 * Update fd safely - close previous if not stdin/stdout, then assign new
 * @param fd_ptr Pointer to fd to update (in_fd or out_fd)
 * @param new_fd New file descriptor to assign
 * @param std_fd Standard fd to compare against (STDIN_FILENO or STDOUT_FILENO)
 */
void	update_fd(int *fd_ptr, int new_fd, int std_fd)
{
	if (*fd_ptr != std_fd)
		close(*fd_ptr);
	*fd_ptr = new_fd;
}
