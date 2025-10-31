/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 09:28:14 by acossari          #+#    #+#             */
/*   Updated: 2025/10/31 13:33:16 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Open heredoc file and unlink
 * @param redir Redirection containing heredoc info
 * @return File descriptor of opened heredoc, or -1 on error
 */
static int	open_heredoc(t_redir *redir)
{
	int	fd;

	if (!redir->hd_path)
		return (print_error("heredoc", "not prepared"), -1);
	fd = open(redir->hd_path, O_RDONLY);
	if (fd == -1)
		return (print_perror("open", redir->hd_path), -1);
	unlink(redir->hd_path);
	return (fd);
}

/**
 * Process single redirection and update in_fd/out_fd
 * @param redir Redirection to process
 * @param in_fd Pointer to input file descriptor
 * @param out_fd Pointer to output file descriptor
 * @return 0 on success, -1 on error
 */
static int	process_redir(t_redir *redir, int *in_fd, int *out_fd)
{
	int	fd;

	fd = -1;
	if (redir->type == TOKEN_HEREDOC)
		fd = open_heredoc(redir);
	else if (redir->type == TOKEN_REDIR_IN)
		fd = open(redir->target, O_RDONLY);
	else if (redir->type == TOKEN_REDIR_OUT)
		fd = open(redir->target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (redir->type == TOKEN_REDIR_APPEND)
		fd = open(redir->target, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1 && redir->type != TOKEN_HEREDOC)
		print_perror("minishell", redir->target);
	if (fd == -1)
		return (-1);
	if (redir->type == TOKEN_HEREDOC || redir->type == TOKEN_REDIR_IN)
		update_fd(in_fd, fd, STDIN_FILENO);
	else
		update_fd(out_fd, fd, STDOUT_FILENO);
	return (0);
}

/**
 * Setup redirections and update in_fd/out_fd
 * @param redirs Linked list of redirections
 * @param in_fd Pointer to input file descriptor
 * @param out_fd Pointer to output file descriptor
 * @return 0 on success, -1 on error
 */
int	setup_redirections(t_redir *redirs, int *in_fd, int *out_fd)
{
	t_redir	*current;

	*in_fd = STDIN_FILENO;
	*out_fd = STDOUT_FILENO;
	current = redirs;
	while (current)
	{
		if (process_redir(current, in_fd, out_fd) == -1)
		{
			update_fd(in_fd, STDIN_FILENO, STDIN_FILENO);
			update_fd(out_fd, STDOUT_FILENO, STDOUT_FILENO);
			return (-1);
		}
		current = current->next;
	}
	return (0);
}

/**
 * Apply redirections using dup2
 * @param in_fd Input file descriptor
 * @param out_fd Output file descriptor
 * @return 0 on success, -1 on error
 */
int	apply_redirections(int in_fd, int out_fd)
{
	if (in_fd != STDIN_FILENO)
	{
		if (dup2(in_fd, STDIN_FILENO) == -1)
			return (print_perror("dup2", NULL), close(in_fd), -1);
		close(in_fd);
	}
	if (out_fd != STDOUT_FILENO)
	{
		if (dup2(out_fd, STDOUT_FILENO) == -1)
			return (print_perror("dup2", NULL), close(out_fd), -1);
		close(out_fd);
	}
	return (0);
}
