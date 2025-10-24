/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 22:17:02 by acossari          #+#    #+#             */
/*   Updated: 2025/10/23 19:31:20 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

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
 * Handle parse errors (FIX ISSUE 7: set exit status to 2)
 * @param result: Parse result with error message
 * @param line: Input line to free
 * @param shell: Shell state (for setting exit status)
 */
static void	handle_parse_error(t_parse_result *result, char *line,
		t_shell *shell)
{
	if (result->error)
	{
		print_error(NULL, result->error);
		free(result->error);
	}
	shell->last_exit_status = 2;
	free(line);
}

/**
 * Handle continuation prompt for incomplete pipe (FIX ISSUE 6)
 * @param line: Initial line with trailing pipe
 * @param shell: Shell state
 * @return Complete line or NULL if Ctrl+D or Ctrl+C
 */
static char	*handle_continuation(char *line, t_shell *shell)
{
	char	*continuation;
	char	*complete;
	char	*tmp;

	while (1)
	{
		continuation = readline("> ");
		if (g_signal_received == SIGINT)
		{
			shell->last_exit_status = 130;
			free(line);
			if (continuation)
				free(continuation);
			return (NULL);
		}
		if (!continuation)
		{
			shell->last_exit_status = 2;
			print_error(NULL, "syntax error: unexpected end of file");
			free(line);
			return (NULL);
		}
		tmp = ft_strjoin(line, "\n");
		free(line);
		if (!tmp)
			return (free(continuation), NULL);
		complete = ft_strjoin(tmp, continuation);
		free(tmp);
		free(continuation);
		if (!complete)
			return (NULL);
		return (complete);
	}
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
	if (parse(line, &result, shell) != 0)
		return (print_error(NULL, "internal parse error"), free(line));
	if (result.error)
		return (handle_parse_error(&result, line, shell));
	if (result.incomplete_pipe)
	{
		line = handle_continuation(line, shell);
		if (!line)
			return ;
		return (process_line(line, shell));
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
 * @param argc: Argument count
 * @param argv: Argument vector
 * @param envp: Environment variables
 * @return Exit status
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
