/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 22:17:02 by acossari          #+#    #+#             */
/*   Updated: 2025/10/27 14:27:17 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/**
 * Initialize parse result and handle empty lines
 * @param line: Input line
 * @param result: Parse result to initialize
 * @param shell: Shell state
 * @return 1 if should continue, 0 if should return
 */
static int	init_parse(char *line, t_parse_result *result, t_shell *shell)
{
	if (line[0] == '\0')
		return (free(line), 0);
	if (shell->interactive && line[0] != '\0')
		add_history(line);
	result->commands = NULL;
	result->error = NULL;
	result->incomplete_pipe = false;
	return (1);
}

/**
 * Process a single input line: parse and execute
 * @param line: Input line from readline
 * @param shell: Shell state
 */
static void	process_line(char *line, t_shell *shell)
{
	t_parse_result	result;

	if (!init_parse(line, &result, shell))
		return ;
	if (parse(line, &result, shell) != 0)
		return (print_error(NULL, "internal parse error"), free(line));
	if (result.error)
		return (handle_parse_error(&result, line, shell));
	if (result.incomplete_pipe)
	{
		line = process_continuation(line, shell);
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
