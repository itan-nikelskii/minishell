/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generic_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 15:18:29 by acossari          #+#    #+#             */
/*   Updated: 2025/11/10 23:17:58 by antoniocoss      ###   ########.fr       */
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

/**
 * Read a line with optional prompt (readline if interactive, GNL otherwise)
 * @param shell: Shell state
 * @param prompt: Prompt to use with readline (ignored in non-interactive)
 * @return Line read, or NULL on EOF/error
 */
char	*read_line_with_prompt(t_shell *shell, const char *prompt)
{
	char	*line;
	size_t	len;

	if (shell->interactive)
		return (readline(prompt));
	line = ft_get_next_line(STDIN_FILENO);
	if (!line)
		return (NULL);
	len = ft_strlen(line);
	if (len > 0 && line[len - 1] == '\n')
		line[len - 1] = '\0';
	return (line);
}
