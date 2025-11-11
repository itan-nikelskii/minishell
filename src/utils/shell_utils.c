/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 11:23:55 by acossari          #+#    #+#             */
/*   Updated: 2025/11/11 19:08:18 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Initialize shell state
 * @param envp: Environment variables from main
 * @return Pointer to initialized t_shell, or NULL on failure
 */
t_shell	*shell_init(char **envp)
{
	t_shell	*shell;

	shell = malloc(sizeof(t_shell));
	if (!shell)
		return (NULL);
	shell->envp = env_dup(envp);
	if (!shell->envp)
		return (free(shell), NULL);
	shell->xenv = malloc(sizeof(char *));
	if (!shell->xenv)
		return (free_array(shell->envp), free(shell), NULL);
	shell->xenv[0] = NULL;
	shell->last_exit_status = 0;
	shell->stdin_backup = -1;
	shell->stdout_backup = -1;
	shell->interactive = isatty(STDIN_FILENO);
	shell->in_child = false;
	return (shell);
}

/**
 * Cleanup shell state and return exit status
 * @param shell: Pointer to t_shell to cleanup
 * @return Exit status from shell->last_exit_status
 */
int	shell_cleanup(t_shell *shell)
{
	int	exit_status;

	if (!shell)
		return (0);
	ft_gnl_clear(STDIN_FILENO);
	free_array(shell->envp);
	free_array(shell->xenv);
	exit_status = shell->last_exit_status;
	free(shell);
	return (exit_status);
}

/**
 * Cleanup shell state and exit process
 * Combines shell_cleanup() and exit() for cleaner code
 * @param shell: Pointer to t_shell to cleanup
 * @param status: Exit status code
 */
void	cleanup_and_exit(t_shell *shell, int status)
{
	shell_cleanup(shell);
	exit(status);
}

/**
 * Map execve errno to shell-compatible exit codes
 * EACCES: Permission denied → 126 (command found but not executable)
 * ENOEXEC: Exec format error → 126 (invalid executable format)
 * EISDIR: Is a directory → 126 (trying to execute a directory)
 * ENOTDIR: Not a directory → 126 (path component not a directory)
 * ENOENT: No such file or directory → 127 (command not found)
 * Other errors → 1 (general failure)
 * @return Exit code corresponding to errno
 */
int	map_execve_errno(void)
{
	if (errno == EACCES || errno == ENOEXEC || errno == EISDIR
		|| errno == ENOTDIR)
		return (CMD_NOT_EXECUTABLE);
	if (errno == ENOENT)
		return (CMD_NOT_FOUND);
	return (EXIT_FAILURE);
}

/**
 * Extract exit status from child process status
 * Handles both normal exit and signal termination
 * WIFEXITED: true if child exited normally → return WEXITSTATUS (0-255)
 * WEXITSTATUS: extracts the low 8 bits as exit code
 * WIFSIGNALED: true if child killed by signal → return 128 + WTERMSIG
 * WTERMSIG: extracts the signal number that terminated the child
 * @param status Status returned by waitpid()
 * @return Exit code (0-255 for normal exit, 128+signal for signals)
 */
int	get_child_exit_status(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (EXIT_FAILURE);
}
