/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 11:23:55 by acossari          #+#    #+#             */
/*   Updated: 2025/10/27 14:08:15 by antoniocoss      ###   ########.fr       */
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
