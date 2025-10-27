/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 09:22:41 by acossari          #+#    #+#             */
/*   Updated: 2025/10/27 13:57:22 by antoniocoss      ###   ########.fr       */
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
 * Child process: reads heredoc lines and writes to FD
 * Exits with 0 on success, 130 on SIGINT, 1 on error
 * @param fd_write: File descriptor to write heredoc content
 * @param delimiter: String that ends the heredoc input
 * @param shell: Shell state for expansions
 * @param expand: If true, expand variables; if false, keep literal
 */
static void	heredoc_child_process(int fd_write, char *delimiter,
								t_shell *shell, bool expand)
{
	char	*line;
	char	*expanded;

	setup_child_ps2_signals();
	while (1)
	{
		line = readline("> ");
		if (!line)
			(close(fd_write), exit(130));
		if (ft_strcmp(line, delimiter) == 0)
			(free(line), close(fd_write), exit(0));
		expanded = hd_expand_line(line, shell, expand);
		if (!expanded)
			(free(line), close(fd_write), exit(1));
		write(fd_write, expanded, ft_strlen(expanded));
		write(fd_write, "\n", 1);
		(free(line), free(expanded));
	}
}

/**
 * Handle child process exit and return status
 * @param pid: PID of the child process
 * @param shell: Shell state for updating last_exit_status
 * @return 0 on success, -1 on error, 130 on SIGINT
 */
static int	handle_child_exit(pid_t pid, t_shell *shell)
{
	int	status;

	while (waitpid(pid, &status, 0) == -1 && errno == EINTR)
		continue ;
	if (WIFEXITED(status) && WEXITSTATUS(status) == 130)
		return (shell->last_exit_status = 130, 130);
	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
		return (-1);
	return (0);
}

/**
 * Cleanup and open file for reading
 * @param filepath: Path to the heredoc temp file
 * @param child_status: Exit status of the heredoc child process
 * @return File descriptor to read heredoc content, -1 on error, -130 on
 */
static int	finalize_heredoc(char *filepath, int child_status)
{
	int	fd_read;

	if (child_status == 130)
		return (unlink(filepath), -130);
	if (child_status != 0)
		return (unlink(filepath), -1);
	fd_read = open(filepath, O_RDONLY);
	if (fd_read == -1)
		return (print_perror("heredoc", "open read"), unlink(filepath), -1);
	return (unlink(filepath), fd_read);
}

/**
 * Handles heredoc input using child helper process
 * @param delimiter: String that ends the heredoc input
 * @param shell: Shell state for expansions
 * @param expand: If true, expand $VAR and $?; if false, keep literal
 * @return File descriptor to read heredoc content, or -1 on error
 */
int	process_heredoc(char *delimiter, t_shell *shell, bool expand)
{
	char	filepath[HD_PATH_BUFSZ];
	int		fd_write;
	pid_t	pid;
	int		child_status;

	fd_write = create_temp_file(filepath);
	if (fd_write == -1)
		return (-1);
	setup_parent_wait_signals();
	pid = fork();
	if (pid == -1)
		return (setup_parent_ps1_signals(),
			close(fd_write), unlink(filepath), -1);
	if (pid == 0)
		heredoc_child_process(fd_write, delimiter, shell, expand);
	close(fd_write);
	child_status = handle_child_exit(pid, shell);
	setup_parent_ps1_signals();
	return (finalize_heredoc(filepath, child_status));
}
