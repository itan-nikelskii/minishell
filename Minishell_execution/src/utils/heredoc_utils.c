/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 10:37:18 by acossari          #+#    #+#             */
/*   Updated: 2025/10/21 17:10:45 by acossari         ###   ########.fr       */
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
