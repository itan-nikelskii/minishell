/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 10:52:47 by acossari          #+#    #+#             */
/*   Updated: 2025/10/19 13:05:24 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Close all pipes in the pipeline context
 * @param ctx Pipeline context
 * @param count Number of pipes
 */
void	close_all_pipes(t_pipe_ctx *ctx, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		close(ctx->pipes[i][0]);
		close(ctx->pipes[i][1]);
		i++;
	}
}

/**
 * Create all pipes needed for the pipeline
 * @param ctx Pipeline context
 * @return 0 on success, -1 on error
 */
int	create_all_pipes(t_pipe_ctx *ctx)
{
	int	i;

	i = 0;
	while (i < ctx->cmd_count - 1)
	{
		if (pipe(ctx->pipes[i]) == -1)
		{
			print_error("pipe", "failed to create pipe");
			close_all_pipes(ctx, i);
			return (-1);
		}
		i++;
	}
	return (0);
}

/**
 * Wait for all child processes in the pipeline
 * @param ctx Pipeline context
 * @return Exit status of the last command
 */
int	wait_all_children(t_pipe_ctx *ctx)
{
	int	i;
	int	status;
	int	last_status;

	i = 0;
	last_status = 0;
	while (i < ctx->cmd_count)
	{
		waitpid(ctx->pids[i], &status, 0);
		if (i == ctx->cmd_count - 1)
			last_status = get_child_exit_status(status);
		i++;
	}
	return (last_status);
}
