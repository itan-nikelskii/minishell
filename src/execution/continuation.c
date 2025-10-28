/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   continuation.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 13:32:03 by acossari          #+#    #+#             */
/*   Updated: 2025/10/27 14:02:23 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Child process for continuation prompt (PS2)
 * Reads one line with readline("> ") and writes to pipe
 * Uses PS2 signal profile (exit 130 on SIGINT)
 * @param pipe_fd: Pipe write end for sending line to parent
 */
static void	continuation_child(int pipe_fd)
{
	char	*continuation;

	setup_child_ps2_signals();
	rl_outstream = stderr;
	continuation = readline("> ");
	if (!continuation || g_signal_received == SIGINT)
	{
		close(pipe_fd);
		exit(130);
	}
	write(pipe_fd, continuation, ft_strlen(continuation));
	free(continuation);
	close(pipe_fd);
	exit(0);
}

/**
 * Read continuation line from child via pipe
 * @param pipe_fd: Pipe read end
 * @return Line read, or NULL on error
 */
static char	*read_continuation_from_pipe(int pipe_fd)
{
	char	buffer[4096];
	ssize_t	bytes;

	bytes = read(pipe_fd, buffer, sizeof(buffer) - 1);
	if (bytes <= 0)
		return (NULL);
	buffer[bytes] = '\0';
	return (ft_strdup(buffer));
}

/**
 * Join line and continuation with newline
 * Frees both input strings
 * @return Joined string or NULL on error
 */
static char	*join_continuation(char *line, char *continuation)
{
	char	*tmp;
	char	*complete;

	tmp = ft_strjoin(line, "\n");
	free(line);
	if (!tmp)
		return (free(continuation), NULL);
	complete = ft_strjoin(tmp, continuation);
	return (free(tmp), free(continuation), complete);
}

/**
 * Fork child and handle result
 * @return 0 on success, 130 on SIGINT, -1 on error
 */
static int	fork_and_wait_child(int pipefd[2], t_shell *shell)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0)
		continuation_child(pipefd[1]);
	close(pipefd[1]);
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 130)
		return (shell->last_exit_status = 130, 130);
	return (0);
}

/**
 * Handle continuation prompt for incomplete pipe (PUBLIC API)
 * Uses child process strategy for proper SIGINT handling
 * Switches signal profiles: PARENT-WAIT during fork, PS1 after
 * 
 * @param line: Initial line with trailing pipe
 * @param shell: Shell state
 * @return Complete line (line + "\n" + continuation), or NULL if Ctrl+D/Ctrl+C
 */
char	*process_continuation(char *line, t_shell *shell)
{
	int		pipefd[2];
	int		child_status;
	char	*continuation;

	if (pipe(pipefd) == -1)
		return (free(line), NULL);
	setup_parent_wait_signals();
	child_status = fork_and_wait_child(pipefd, shell);
	setup_parent_ps1_signals();
	if (child_status == -1)
		return (close(pipefd[0]), close(pipefd[1]), free(line), NULL);
	if (child_status == 130)
		return (close(pipefd[0]), free(line), NULL);
	continuation = read_continuation_from_pipe(pipefd[0]);
	close(pipefd[0]);
	if (!continuation)
		return (shell->last_exit_status = 2,
			print_error(NULL, "syntax error: unexpected end of file"),
			free(line), NULL);
	return (join_continuation(line, continuation));
}
