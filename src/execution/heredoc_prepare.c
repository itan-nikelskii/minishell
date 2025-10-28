/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_prepare.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 19:20:00 by antoniocoss       #+#    #+#             */
/*   Updated: 2025/10/27 23:31:05 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Cleanup prepared heredoc files
 * @param cmd: Command with heredocs
 */
static void	cleanup_prepared_heredocs(t_command *cmd)
{
	t_redir	*redir;

	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC && redir->hd_path)
		{
			unlink(redir->hd_path);
			free(redir->hd_path);
			redir->hd_path = NULL;
		}
		redir = redir->next;
	}
}

/**
 * Heredoc read loop in child process
 * @param fd: File descriptor to write to
 * @param delimiter: Heredoc delimiter
 * @param shell: Shell state
 * @param expand: Whether to expand variables
 */
static void	heredoc_read_loop(int fd, char *delimiter, t_shell *shell,
		bool expand)
{
	char	*line;
	char	*expanded;

	while (1)
	{
		line = read_heredoc_line(shell);
		if (!line || g_signal_received == SIGINT)
			(free(line), close(fd), exit(130));
		if (ft_strcmp(line, delimiter) == 0)
			(free(line), close(fd), exit(0));
		if (expand)
		{
			expanded = hd_expand_line(line, shell, true);
			(free(line), line = expanded);
			if (!line)
				(close(fd), exit(1));
		}
		write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
		free(line);
	}
}

/**
 * Spawn child to read heredoc and write to temp file
 * @param delimiter: Heredoc delimiter
 * @param expand: Whether to expand variables
 * @param tmp_path: Path to temp file
 * @param shell: Shell state
 * @return 0 on success, 130 on SIGINT, -1 on error
 */
static int	spawn_heredoc_reader(char *delimiter, bool expand, char *tmp_path,
		t_shell *shell)
{
	pid_t	pid;
	int		fd;
	int		status;

	setup_parent_wait_signals();
	pid = fork();
	if (pid == -1)
		return (setup_parent_ps1_signals(), -1);
	if (pid == 0)
	{
		setup_child_ps2_signals();
		fd = open(tmp_path, O_CREAT | O_EXCL | O_WRONLY, 0600);
		if (fd == -1)
			exit(1);
		heredoc_read_loop(fd, delimiter, shell, expand);
	}
	waitpid(pid, &status, 0);
	setup_parent_ps1_signals();
	if (WIFEXITED(status) && WEXITSTATUS(status) == 130)
		return (130);
	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
		return (-1);
	return (0);
}

/**
 * Prepare all heredocs for a command
 * @param cmd: Command with redirections
 * @param shell: Shell state
 * @return 0 on success, 130 on SIGINT, -1 on error
 */
int	prepare_heredocs(t_command *cmd, t_shell *shell)
{
	t_redir	*redir;
	char	tmp_path[256];
	int		result;

	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC)
		{
			build_heredoc_filepath(tmp_path);
			result = spawn_heredoc_reader(redir->target, !redir->was_quoted,
					tmp_path, shell);
			if (result == 130)
				return (cleanup_prepared_heredocs(cmd), 130);
			if (result != 0)
				return (cleanup_prepared_heredocs(cmd), -1);
			redir->hd_path = ft_strdup(tmp_path);
			if (!redir->hd_path)
				return (cleanup_prepared_heredocs(cmd), -1);
		}
		redir = redir->next;
	}
	return (0);
}
