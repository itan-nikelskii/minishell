/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:10:00 by antoniocoss      #+#    #+#             */
/*   Updated: 2025/11/04 13:10:00 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Cleanup redirection file descriptors if they are not stdin/stdout
 * Only closes if fd is not STDIN_FILENO (0) or STDOUT_FILENO (1)
 * @param in_fd Input file descriptor to cleanup
 * @param out_fd Output file descriptor to cleanup
 */
void	cleanup_redir_fds(int in_fd, int out_fd)
{
	if (in_fd != STDIN_FILENO)
		close(in_fd);
	if (out_fd != STDOUT_FILENO)
		close(out_fd);
}
