/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_multi.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 09:38:23 by acossari          #+#    #+#             */
/*   Updated: 2025/10/28 20:00:28 by antoniocoss      ###   ########.fr       */
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
	int	j;

	if (i == 0)
		dup2(ctx->pipes[0][1], STDOUT_FILENO);
	else if (i == ctx->cmd_count - 1)
		dup2(ctx->pipes[ctx->cmd_count - 2][0], STDIN_FILENO);
	else
	{
		dup2(ctx->pipes[i - 1][0], STDIN_FILENO);
		dup2(ctx->pipes[i][1], STDOUT_FILENO);
	}
	j = 0;
	while (j < ctx->cmd_count - 1)
	{
		close(ctx->pipes[j][0]);
		close(ctx->pipes[j][1]);
		j++;
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
		print_error("fork", "failed to create process");
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
 * Fork all children for the pipeline
 * @param ctx Pipeline context
 * @param cmd_list List of commands in the pipeline
 * @return 0 on success, -1 on error
 */
static int	fork_all_children(t_pipe_ctx *ctx, t_command *cmd_list)
{
	t_command	*cur;
	int			i;

	cur = cmd_list;
	i = 0;
	while (i < ctx->cmd_count)
	{
		ctx->pids[i] = fork_and_exec(cur, i, ctx);
		if (ctx->pids[i] == -1)
		{
			close_all_pipes(ctx, ctx->cmd_count - 1);
			while (--i >= 0)
				waitpid(ctx->pids[i], NULL, 0);
			return (-1);
		}
		cur = cur->next;
		i++;
	}
	return (0);
}

/**
 * Execute a pipeline with 2 or more commands
 * @param cmd_list List of commands in the pipeline
 * @param shell Shell state
 * @return Exit status of the last command
 */
int	execute_pipeline(t_command *cmd_list, t_shell *shell)
{
	t_pipe_ctx	ctx;
	int			result;

	ctx.cmd_count = count_commands(cmd_list);
	ctx.shell = shell;
	ctx.pipes = malloc(sizeof(int [2]) * (ctx.cmd_count - 1));
	ctx.pids = malloc(sizeof(pid_t) * ctx.cmd_count);
	if (!ctx.pipes || !ctx.pids)
		return (free(ctx.pipes), free(ctx.pids), 1);
	if (create_all_pipes(&ctx) == -1)
		return (free(ctx.pipes), free(ctx.pids), 1);
	setup_parent_wait_signals();
	if (fork_all_children(&ctx, cmd_list) == -1)
		return (setup_parent_ps1_signals(), free(ctx.pipes), free(ctx.pids), 1);
	close_all_pipes(&ctx, ctx.cmd_count - 1);
	result = wait_all_children(ctx.pids, ctx.cmd_count);
	setup_parent_ps1_signals();
	free(ctx.pipes);
	free(ctx.pids);
	return (get_child_exit_status(result));
}
