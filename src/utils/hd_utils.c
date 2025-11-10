/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hd_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 10:37:18 by acossari          #+#    #+#             */
/*   Updated: 2025/11/10 23:18:00 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Build the filepath for the heredoc temp file
 * Uses static counter for unique names (bash-like behavior)
 * @param filepath: Buffer to store the generated filepath
 */
void	build_heredoc_filepath(char *filepath)
{
	static int	counter = 0;
	char		*counter_str;

	counter_str = ft_itoa(counter);
	counter++;
	ft_strlcpy(filepath, "/tmp/.minishell_heredoc_", HD_PATH_BUFSZ);
	ft_strlcat(filepath, counter_str, HD_PATH_BUFSZ);
	free(counter_str);
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
