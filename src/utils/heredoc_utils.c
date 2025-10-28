/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 10:37:18 by acossari          #+#    #+#             */
/*   Updated: 2025/10/27 20:01:56 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Get a unique counter for heredoc temp files
 * @return Incrementing integer each call
 */
int	get_heredoc_counter(void)
{
	static int	counter = 0;

	return (counter++);
}

/**
 * Build the filepath for the heredoc temp file
 * @param filepath: Buffer to store the generated filepath
 */
void	build_heredoc_filepath(char *filepath)
{
	char	*prefix;
	char	*pid_str;
	char	*counter_str;

	prefix = "/tmp/minishell_hd_";
	pid_str = ft_itoa(getpid());
	counter_str = ft_itoa(get_heredoc_counter());
	ft_strlcpy(filepath, prefix, HD_PATH_BUFSZ);
	ft_strlcat(filepath, pid_str, HD_PATH_BUFSZ);
	ft_strlcat(filepath, "_", HD_PATH_BUFSZ);
	ft_strlcat(filepath, counter_str, HD_PATH_BUFSZ);
	free(pid_str);
	free(counter_str);
}

/**
 * Cleanup heredoc resources
 * @param fd_write: File descriptor to close (if not -1)
 * @param filepath: Path to the temp file to unlink (if not NULL)
 * @param line: Line buffer to free (if not NULL)
 */
void	cleanup_heredoc(int fd_write, char *filepath, char *line)
{
	if (line)
		free(line);
	if (fd_write != -1)
		close(fd_write);
	if (filepath && filepath[0])
		unlink(filepath);
}

/**
 * Read a line for heredoc (readline if interactive, GNL otherwise)
 * @param shell: Shell state
 * @return Line read, or NULL on EOF/error
 */
char	*read_heredoc_line(t_shell *shell)
{
	char	*line;
	size_t	len;

	if (shell->interactive)
		return (readline("> "));
	line = ft_get_next_line(STDIN_FILENO);
	if (!line)
		return (NULL);
	len = ft_strlen(line);
	if (len > 0 && line[len - 1] == '\n')
		line[len - 1] = '\0';
	return (line);
}
