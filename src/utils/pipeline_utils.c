/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 10:52:47 by acossari          #+#    #+#             */
/*   Updated: 2025/11/07 23:24:56 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Close both ends of a pipe
 * @param pipe_fd Array [read_end, write_end]
 */
void	close_pipe_ends(int pipe_fd[2])
{
	close(pipe_fd[0]);
	close(pipe_fd[1]);
}

/**
 * Initialize pipeline context
 * @param ctx Pipeline context to initialize
 * @param cmd_list List of commands
 * @param shell Shell state
 * @return 0 on success, -1 on malloc failure
 */
int	init_pipeline_ctx(t_pipe_ctx *ctx, t_command *cmd_list, t_shell *shell)
{
	ctx->cmd_count = count_commands(cmd_list);
	ctx->shell = shell;
	ctx->pids = malloc(sizeof(pid_t) * ctx->cmd_count);
	if (!ctx->pids)
		return (-1);
	return (0);
}

/**
 * Close previous pipe and move next pipe to previous
 * @param i Index of the command in the pipeline
 * @param ctx Pipeline context
 */
void	advance_pipes(int i, t_pipe_ctx *ctx)
{
	if (i > 0)
		close_pipe_ends(ctx->prev_pipe);
	if (i < ctx->cmd_count - 1)
	{
		ctx->prev_pipe[0] = ctx->next_pipe[0];
		ctx->prev_pipe[1] = ctx->next_pipe[1];
	}
}

/**
 * Wait for N children and print newline if any received SIGINT
 * Used for pipeline wait (2 or more commands)
 * @param pids Array of process IDs
 * @param n Number of processes to wait for
 * @return Raw status of last child (for get_child_exit_status)
 */
int	wait_all_children(pid_t *pids, int n)
{
	int	i;
	int	status;
	int	last_status;
	int	interrupted;

	i = 0;
	last_status = 0;
	interrupted = 0;
	while (i < n)
	{
		waitpid(pids[i], &status, 0);
		if (!interrupted && WIFSIGNALED(status)
			&& WTERMSIG(status) == SIGINT)
			interrupted = 1;
		if (i == n - 1)
			last_status = status;
		i++;
	}
	if (interrupted && isatty(STDIN_FILENO))
		write(STDOUT_FILENO, "\n", 1);
	return (last_status);
}
