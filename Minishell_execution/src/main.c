/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 22:17:02 by acossari          #+#    #+#             */
/*   Updated: 2025/10/23 15:13:13 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <readline/readline.h>
#include <readline/history.h>

/**
 * Initialize shell state
 * @param envp: Environment variables from main
 * @return Pointer to initialized t_shell, or NULL on failure
 */
static t_shell	*init_shell(char **envp)
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
		return (free(shell->envp), free(shell), NULL);
	shell->xenv[0] = NULL;
	shell->last_exit_status = 0;
	shell->stdin_backup = -1;
	shell->stdout_backup = -1;
	return (shell);
}

/** 
 * Cleanup shell state and free memory
 * @param shell: Pointer to t_shell to cleanup
 */
static void	cleanup_shell(t_shell *shell)
{
	int	i;

	if (!shell)
		return ;
	if (shell->envp)
	{
		i = 0;
		while (shell->envp[i])
			free(shell->envp[i++]);
		free(shell->envp);
	}
	if (shell->xenv)
	{
		i = 0;
		while (shell->xenv[i])
			free(shell->xenv[i++]);
		free(shell->xenv);
	}
	free(shell);
}

/** 
 * Process a single input line: parse and execute
 * @param line: Input line from readline
 * @param shell: Shell state
 */
static void	process_line(char *line, t_shell *shell)
{
	t_parse_result	result;

	if (line[0] == '\0')
		return (free(line));
	add_history(line);
	result.commands = NULL;
	result.error = NULL;
	result.incomplete_pipe = false;
	if (parse(line, &result) != 0)
	{
		ft_putstr_fd("minishell: internal parse error\n", STDERR_FILENO);
		return (free(line));
	}
	if (result.error)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(result.error, STDERR_FILENO);
		ft_putstr_fd("\n", STDERR_FILENO);
		free(result.error);
		return (free(line));
	}
	if (result.commands)
	{
		execute_command(result.commands, shell);
		free_commands(result.commands);
	}
	free(line);
}

/**
 * Main entry point for minishell
 */
int	main(int argc, char **argv, char **envp)
{
	t_shell	*shell;
	char	*line;

	(void)argc;
	(void)argv;
	shell = init_shell(envp);
	if (!shell)
		return (ft_putstr_fd("minishell: init failed\n", STDERR_FILENO), 1);
	setup_prompt_signals();
	while (1)
	{
		line = readline("minishell$ ");
		if (!line)
			break ;
		process_line(line, shell);
		g_signal_received = 0;
	}
	ft_putstr_fd("exit\n", STDOUT_FILENO);
	cleanup_shell(shell);
	return (0);
}
