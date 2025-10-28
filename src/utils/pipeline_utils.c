/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 10:52:47 by acossari          #+#    #+#             */
/*   Updated: 2025/10/28 20:00:28 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Cleanup pipes in parent process
 * @param pipefd Array [read_end, write_end]
 */
void	cleanup_pipes(int pipefd[2])
{
	close(pipefd[0]);
	close(pipefd[1]);
}

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
