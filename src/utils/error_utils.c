/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 14:28:51 by acossari          #+#    #+#             */
/*   Updated: 2025/11/10 18:33:56 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Print common error prefix: "minishell: <prefix>: "
 * @param prefix Optional prefix (e.g., command name)
 */
static void	print_prefix(char *prefix)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	if (prefix)
	{
		ft_putstr_fd(prefix, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
	}
}

/**
 * Print error message to stderr in format: "minishell: <prefix>: <msg>"
 * @param prefix Prefix for error message (e.g., command name)
 * @param msg Error message
 */
void	print_error(char *prefix, char *msg)
{
	print_prefix(prefix);
	ft_putstr_fd(msg, STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
}

/**
 * Print error with argument in format: "minishell: <prefix>: `<arg>': <msg>"
 * @param prefix Command name (e.g., "export", "unset")
 * @param arg Argument that caused the error
 * @param msg Error message
 */
void	print_error_arg(char *prefix, char *arg, char *msg)
{
	print_prefix(prefix);
	ft_putstr_fd("`", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putstr_fd("': ", STDERR_FILENO);
	ft_putstr_fd(msg, STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
}

/**
 * Print error using perror format:
 * "minishell: <prefix>: <arg>: <system error>"
 * @param prefix First prefix (e.g., "cd")
 * @param arg Second argument (e.g., path) - can be NULL
 */
void	print_perror(char *prefix, char *arg)
{
	print_prefix(prefix);
	if (arg)
	{
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
	}
	perror(NULL);
}

/**
 * Print message when child terminated by signal
 * Handles SIGINT (newline) and SIGQUIT (Quit message)
 * Only prints in interactive mode
 * @param status Status from waitpid()
 */
void	print_signal_message(int status)
{
	int	sig;

	if (!WIFSIGNALED(status) || !isatty(STDIN_FILENO))
		return ;
	sig = WTERMSIG(status);
	if (sig == SIGINT)
		write(STDOUT_FILENO, "\n", 1);
	else if (sig == SIGQUIT)
		write(STDERR_FILENO, "Quit (core dumped)\n", 19);
}
