/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generic_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 15:18:29 by acossari          #+#    #+#             */
/*   Updated: 2025/11/10 21:00:07 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Counts number of commands in linked list
 * @param cmd_list First command in list
 * @return Number of commands
 */
int	count_commands(t_command *cmd_list)
{
	int			count;
	t_command	*current;

	count = 0;
	current = cmd_list;
	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}

/**
 * Frees a NULL-terminated array of strings
 * @param array Array to free
 */
void	free_array(char **array)
{
	int	i;

	if (!array)
		return ;
	i = 0;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}

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
