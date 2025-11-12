/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hd_prepare.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 19:20:01 by antoniocoss       #+#    #+#             */
/*   Updated: 2025/11/12 10:31:35 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Heredoc read loop (generic version returns exit code)
 * @param fd: File descriptor to write to
 * @param delimiter: Heredoc delimiter
 * @param shell: Shell state
 * @param expand: Whether to expand variables
 * @return Exit code: 0 on success, 130 on SIGINT, 1 on error
 */
static int	heredoc_read_loop(int fd, char *delimiter, t_shell *shell,
		bool expand)
{
	char	*line;
	char	*expanded;

	while (1)
	{
		line = read_line_with_prompt(shell, "> ");
		if (g_signal_received == SIGINT)
			return (free(line), close(fd), 130);
		if (!line)
			return (print_heredoc_eof_warning(delimiter), close(fd), 0);
		if (ft_strcmp(line, delimiter) == 0)
			return (free(line), close(fd), 0);
		if (expand)
		{
			expanded = hd_expand_line(line, shell);
			free(line);
			if (!expanded)
				return (close(fd), 1);
			line = expanded;
		}
		ft_putendl_fd(line, fd);
		free(line);
	}
}

/**
 * Process heredoc in child (interactive mode only)
 * @param delimiter: Heredoc delimiter
 * @param expand: Whether to expand variables
 * @param tmp_path: Path to temp file
 * @param shell: Shell state
 * @return 0 on success, 130 on SIGINT, -1 on error
 */
static int	process_heredoc_interactive(char *delimiter, bool expand,
		char *tmp_path, t_shell *shell)
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
		exit(heredoc_read_loop(fd, delimiter, shell, expand));
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
 * Process heredoc in parent (non-interactive mode only)
 * @param delimiter: Heredoc delimiter
 * @param expand: Whether to expand variables
 * @param tmp_path: Path to temp file
 * @param shell: Shell state
 * @return 0 on success, -1 on error
 */
static int	process_heredoc_noninteractive(char *delimiter, bool expand,
		char *tmp_path, t_shell *shell)
{
	int	fd;
	int	result;

	fd = open(tmp_path, O_CREAT | O_EXCL | O_WRONLY, 0600);
	if (fd == -1)
		return (-1);
	result = heredoc_read_loop(fd, delimiter, shell, expand);
	if (result == 1)
		return (-1);
	return (0);
}

/**
 * Process a single heredoc redirection
 * @param redir: Redirection node
 * @param shell: Shell state
 * @param cmd: Command (for cleanup on error)
 * @return 0 on success, 130 on SIGINT, -1 on error
 */
static int	process_single_heredoc(t_redir *redir, t_shell *shell,
		t_command *cmd)
{
	char	tmp_path[256];
	int		result;

	build_heredoc_filepath(tmp_path);
	if (shell->interactive)
		result = process_heredoc_interactive(redir->target,
				!redir->was_quoted, tmp_path, shell);
	else
		result = process_heredoc_noninteractive(redir->target,
				!redir->was_quoted, tmp_path, shell);
	if (result == 130)
		return (cleanup_prepared_heredocs(cmd), 130);
	if (result != 0)
		return (cleanup_prepared_heredocs(cmd), -1);
	redir->hd_path = ft_strdup(tmp_path);
	if (!redir->hd_path)
		return (cleanup_prepared_heredocs(cmd), -1);
	return (0);
}

/**
 * Prepare all heredocs for a command
 * @param cmd: Command with redirections
 * @param shell: Shell state
 * @return 0 on success, 130 on SIGINT, -1 on error, -2 on too many heredocs
 */
int	prepare_heredocs(t_command *cmd, t_shell *shell)
{
	t_redir	*redir;
	int		result;

	if (too_many_heredocs(cmd))
		return (-2);
	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC)
		{
			result = process_single_heredoc(redir, shell, cmd);
			if (result != 0)
				return (result);
		}
		redir = redir->next;
	}
	return (0);
}
