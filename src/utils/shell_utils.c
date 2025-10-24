/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 11:23:55 by acossari          #+#    #+#             */
/*   Updated: 2025/10/21 13:02:15 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/*
 * init_shell - Initialize shell state
 *
 * @envp: Environment variables from main
 *
 * Returns: Pointer to initialized shell, or NULL on failure
 *
 * Initializes:
 * - envp: Copy of environment
 * - xenv: Empty exported names list
 * - last_exit_status: 0
 * - stdin_backup/stdout_backup: -1 (invalid fd)
 */
t_shell	*init_shell(char **envp)
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
	return (shell);
}

/*
 * free_shell - Free all shell resources
 *
 * @shell: Shell state to free
 *
 * Frees:
 * - envp array
 * - xenv array
 * - shell struct itself
 */
void	free_shell(t_shell *shell)
{
	if (!shell)
		return ;
	if (shell->envp)
		free_array(shell->envp);
	if (shell->xenv)
		free_array(shell->xenv);
	free(shell);
}
