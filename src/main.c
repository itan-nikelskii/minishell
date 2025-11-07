/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 22:17:02 by acossari          #+#    #+#             */
/*   Updated: 2025/11/08 00:19:35 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/**
 * Handle parse error: print, cleanup, and set exit status
 * @param result: Parse result with error info
 * @param line: Input line to free
 * @param shell: Shell state to set exit status
 */
static void	handle_parse_error(t_parse_result *result, char *line,
		t_shell *shell)
{
	print_error(NULL, result->error);
	free(result->error);
	free_commands(result->commands);
	free(line);
	if (!shell->interactive)
		exit(2);
	shell->last_exit_status = 2;
}

/**
 * Process a single input line: parse and execute
 * @param line: Input line from readline
 * @param shell: Shell state
 */
static void	process_line(char *line, t_shell *shell)
{
	t_parse_result	result;

	result = (t_parse_result){NULL, NULL, false};
	if (parse(line, &result, shell) != 0)
		return (print_error(NULL, "internal parse error"),
			free_commands(result.commands), free(line));
	if (result.error)
		return (handle_parse_error(&result, line, shell));
	if (result.incomplete_pipe)
	{
		free_commands(result.commands);
		line = process_continuation(line, shell);
		if (line)
			process_line(line, shell);
		return ;
	}
	if (result.commands)
	{
		execute_command(result.commands, shell);
		free_commands(result.commands);
	}
	free(line);
}

/**
 * Main read-eval-print loop
 * @param shell: Shell state
 */
static void	main_loop(t_shell *shell)
{
	char	*line;

	while (1)
	{
		rl_on_new_line();
		line = read_input_line(shell);
		if (g_signal_received == SIGINT)
		{
			shell->last_exit_status = 130;
			g_signal_received = 0;
		}
		if (!line)
			break ;
		if (line[0] == '\0')
		{
			free(line);
			continue ;
		}
		if (shell->interactive)
			add_history(line);
		process_line(line, shell);
		g_signal_received = 0;
	}
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

	(void)argc;
	(void)argv;
	shell = shell_init(envp);
	if (!shell)
		return (ft_putstr_fd("minishell: init failed\n", STDERR_FILENO), 1);
	setup_parent_ps1_signals();
	main_loop(shell);
	if (shell->interactive)
		ft_putstr_fd("exit\n", STDOUT_FILENO);
	return (shell_cleanup(shell));
}
