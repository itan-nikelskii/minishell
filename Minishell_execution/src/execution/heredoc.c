/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 09:22:41 by acossari          #+#    #+#             */
/*   Updated: 2025/10/21 20:00:36 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Cleans up heredoc temp file and fd
 * @param fd: File descriptor to close (if not NULL)
 * @param filepath: Path to the temp file to unlink (if not NULL)
 * @msg: Optional error message to print (if not NULL)
 */
static int	create_temp_file(char *filepath)
{
	int	fd;

	build_heredoc_filepath(filepath);
	fd = open(filepath, O_CREAT | O_EXCL | O_WRONLY, 0600);
	if (fd == -1)
		print_perror("heredoc", "create temp file");
	return (fd);
}

/**
 * Reads lines from stdin until delimiter or signal
 * Expands $VAR and $? if expand flag is true
 * @param fd_write: File descriptor to write heredoc content
 * @param delimiter: String that ends the heredoc input
 * @param shell: Shell state for expansions
 * @param expand: If true, expand variables; if false, keep literal
 * @return 0 on success, -1 on SIGINT
 */
static int	read_heredoc_lines(int fd_write, char *delimiter,
								t_shell *shell, bool expand)
{
	char	*line;
	char	*expanded;

	while (1)
	{
		line = readline("> ");
		if (g_signal_received == SIGINT)
			return (free(line), -1);
		if (!line)
			return (0);
		if (ft_strcmp(line, delimiter) == 0)
			return (free(line), 0);
		expanded = hd_expand_line(line, shell, expand);
		free(line);
		if (!expanded)
			return (-1);
		write(fd_write, expanded, ft_strlen(expanded));
		write(fd_write, "\n", 1);
		free(expanded);
	}
}

/**
 * Handles heredoc input and returns read fd
 * @param delimiter: String that ends the heredoc input
 * @param shell: Shell state for expansions
 * @param expand: If true, expand $VAR and $?; if false, keep literal
 * @return File descriptor to read heredoc content, or -1 on error
 */
int	process_heredoc(char *delimiter, t_shell *shell, bool expand)
{
	char	filepath[HD_PATH_BUFSZ];
	int		fd_write;
	int		fd_read;

	setup_heredoc_signals();
	g_signal_received = 0;
	fd_write = create_temp_file(filepath);
	if (fd_write == -1)
		return (setup_prompt_signals(), -1);
	if (read_heredoc_lines(fd_write, delimiter, shell, expand) == -1)
		return (cleanup_heredoc(fd_write, filepath, NULL),
			setup_prompt_signals(), -1);
	close(fd_write);
	fd_read = open(filepath, O_RDONLY);
	if (fd_read == -1)
	{
		print_perror("heredoc", "open read");
		unlink(filepath);
		return (setup_prompt_signals(), -1);
	}
	unlink(filepath);
	setup_prompt_signals();
	return (fd_read);
}
