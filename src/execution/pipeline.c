/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 09:38:23 by acossari          #+#    #+#             */
/*   Updated: 2025/11/07 23:24:48 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Setup pipes for child process based on its position in pipeline
 * @param i Index of the command in the pipeline
 * @param ctx Pipeline context
 */
static void	setup_child_pipes(int i, t_pipe_ctx *ctx)
{
	if (i > 0)
	{
		dup2(ctx->prev_pipe[0], STDIN_FILENO);
		close_pipe_ends(ctx->prev_pipe);
	}
	if (i < ctx->cmd_count - 1)
	{
		close(ctx->next_pipe[0]);
		dup2(ctx->next_pipe[1], STDOUT_FILENO);
		close(ctx->next_pipe[1]);
	}
}

/**
 * Fork and execute a command in the pipeline
 * @param cmd Command to execute
 * @param i Index of the command in the pipeline
 * @param ctx Pipeline context
 * @return PID of the child process, or -1 on error
 */
static pid_t	fork_and_exec(t_command *cmd, int i, t_pipe_ctx *ctx)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (-1);
	}
	if (pid == 0)
	{
		setup_child_pipes(i, ctx);
		exec_child_piped(cmd, ctx->shell);
	}
	return (pid);
}

/**
 * Create next pipe for the pipeline if not the last command
 * @param i Index of the command in the pipeline
 * @param ctx Pipeline context
 * @return 0 on success, -1 on error
 */
static int	create_next_pipe(int i, t_pipe_ctx *ctx)
{
	if (i < ctx->cmd_count - 1)
	{
		if (pipe(ctx->next_pipe) == -1)
		{
			perror("pipe");
			return (-1);
		}
	}
	return (0);
}

/**
 * Cleanup after pipeline execution
 * @param ctx Pipeline context
 * @param i Index up to which commands were forked
 */
static void	cleanup_pipeline(t_pipe_ctx *ctx, int i)
{
	int	j;

	if (i > 0)
		close_pipe_ends(ctx->prev_pipe);
	j = 0;
	while (j < i)
	{
		waitpid(ctx->pids[j], NULL, 0);
		j++;
	}
	free(ctx->pids);
}

/**
 * Execute a pipeline with 2 or more commands
 * Progressive pipe creation: only prev_pipe and next_pipe open at a time
 * @param cmd_list List of commands in the pipeline
 * @param shell Shell state
 * @return Exit status of the last command
 */
int	execute_pipeline(t_command *cmd_list, t_shell *shell)
{
	t_pipe_ctx	ctx;
	t_command	*cur;
	int			i;
	int			result;

	if (init_pipeline_ctx(&ctx, cmd_list, shell) == -1)
		return (1);
	setup_parent_wait_signals();
	cur = cmd_list;
	i = 0;
	while (i < ctx.cmd_count)
	{
		if (create_next_pipe(i, &ctx) == -1)
			return (cleanup_pipeline(&ctx, i), setup_parent_ps1_signals(), 1);
		ctx.pids[i] = fork_and_exec(cur, i, &ctx);
		if (ctx.pids[i] == -1)
			return (cleanup_pipeline(&ctx, i), setup_parent_ps1_signals(), 1);
		advance_pipes(i, &ctx);
		cur = cur->next;
		i++;
	}
	result = wait_all_children(ctx.pids, ctx.cmd_count);
	setup_parent_ps1_signals();
	free(ctx.pids);
	return (get_child_exit_status(result));
}
