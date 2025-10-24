/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 09:28:14 by acossari          #+#    #+#             */
/*   Updated: 2025/10/21 12:59:27 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Update fd safely - close previous if not stdin/stdout, then assign new
 * @param fd_ptr Pointer to fd to update (in_fd or out_fd)
 * @param new_fd New file descriptor to assign
 * @param std_fd Standard fd to compare against (STDIN_FILENO or STDOUT_FILENO)
 */
static void	update_fd(int *fd_ptr, int new_fd, int std_fd)
{
	if (*fd_ptr != std_fd)
		close(*fd_ptr);
	*fd_ptr = new_fd;
}

/**
 * Process single redirection and update in_fd/out_fd
 * @param redir Redirection to process
 * @param in_fd Pointer to input fd
 * @param out_fd Pointer to output fd
 * @return 0 on success, -1 on error
 */
static int	process_redir(t_redir *redir, int *in_fd, int *out_fd, t_shell *sh)
{
	int	fd;

	fd = -1;
	if (redir->type == TOKEN_HEREDOC)
		fd = process_heredoc(redir->target, sh, !redir->was_quoted);
	else if (redir->type == TOKEN_REDIR_IN)
		fd = open(redir->target, O_RDONLY);
	else if (redir->type == TOKEN_REDIR_OUT)
		fd = open(redir->target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (redir->type == TOKEN_REDIR_APPEND)
		fd = open(redir->target, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
		return (print_perror("minishell", redir->target), -1);
	if (redir->type == TOKEN_HEREDOC || redir->type == TOKEN_REDIR_IN)
		update_fd(in_fd, fd, STDIN_FILENO);
	else
		update_fd(out_fd, fd, STDOUT_FILENO);
	return (0);
}

/**
 * Setup redirections: open all files and update in_fd/out_fd
 * @param redirs List of redirections
 * @param in_fd Pointer to input fd (initialized to STDIN_FILENO)
 * @param out_fd Pointer to output fd (initialized to STDOUT_FILENO)
 * @param shell Shell state for heredoc expansions
 * @return 0 on success, -1 on error
 */
int	setup_redirections(t_redir *redirs, int *in_fd, int *out_fd, t_shell *sh)
{
	t_redir	*current;

	*in_fd = STDIN_FILENO;
	*out_fd = STDOUT_FILENO;
	current = redirs;
	while (current)
	{
		if (process_redir(current, in_fd, out_fd, sh) == -1)
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
