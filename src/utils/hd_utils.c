/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 10:37:18 by acossari          #+#    #+#             */
/*   Updated: 2025/10/30 14:35:09 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Build the filepath for the heredoc temp file
 * @param filepath: Buffer to store the generated filepath
 */
void	build_heredoc_filepath(char *filepath)
{
	static int	counter = 0;
	char		*prefix;
	char		*pid_str;
	char		*counter_str;

	prefix = "/tmp/minishell_hd_";
	pid_str = ft_itoa(getpid());
	counter_str = ft_itoa(counter++);
	ft_strlcpy(filepath, prefix, HD_PATH_BUFSZ);
	ft_strlcat(filepath, pid_str, HD_PATH_BUFSZ);
	ft_strlcat(filepath, "_", HD_PATH_BUFSZ);
	ft_strlcat(filepath, counter_str, HD_PATH_BUFSZ);
	free(pid_str);
	free(counter_str);
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

/**
 * Print heredoc EOF warning (bash-like)
 * @param delimiter: Expected delimiter
 */
void	print_heredoc_eof_warning(char *delimiter)
{
	ft_putstr_fd("minishell: warning: here-document at line X ", STDERR_FILENO);
	ft_putstr_fd("delimited by end-of-file (wanted `", STDERR_FILENO);
	ft_putstr_fd(delimiter, STDERR_FILENO);
	ft_putstr_fd("')\n", STDERR_FILENO);
}

/**
 * Cleanup prepared heredoc files
 * @param cmd: Command with heredocs
 */
void	cleanup_prepared_heredocs(t_command *cmd)
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
 * Check if command has too many heredocs (>16)
 * @param cmd: Command to check
 * @return true if too many heredocs, false otherwise
 */
bool	too_many_heredocs(t_command *cmd)
{
	t_redir	*redir;
	int		count;

	count = 0;
	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC)
			count++;
		redir = redir->next;
		if (count > 16)
		{
			print_error(NULL, "maximum here-document count exceeded");
			return (true);
		}
	}
	return (false);
}
