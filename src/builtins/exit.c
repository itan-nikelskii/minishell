/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 12:43:11 by antoniocoss       #+#    #+#             */
/*   Updated: 2025/11/12 10:31:35 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Handle exit with no arguments
 * @param cmd Command structure to free
 * @param shell Shell state
 */
static void	exit_no_args(t_command *cmd, t_shell *shell)
{
	int	exit_status;

	if (shell->interactive && !shell->in_child)
		ft_putendl_fd("exit", STDERR_FILENO);
	exit_status = shell->last_exit_status;
	free_commands(cmd);
	if (!shell->in_child)
		rl_clear_history();
	cleanup_and_exit(shell, exit_status);
}

/**
 * Handle exit with invalid numeric argument
 * @param arg_orig Original argument string (not freed by this function)
 * @param cmd Command structure to free
 * @param shell Shell state
 */
static void	exit_invalid_arg(const char *arg_orig, t_command *cmd,
		t_shell *shell)
{
	char	*arg_copy;

	if (shell->interactive && !shell->in_child)
		ft_putendl_fd("exit", STDERR_FILENO);
	ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
	arg_copy = ft_strdup(arg_orig);
	free_commands(cmd);
	if (arg_copy)
	{
		ft_putstr_fd(arg_copy, STDERR_FILENO);
		free(arg_copy);
	}
	ft_putendl_fd(": numeric argument required", STDERR_FILENO);
	if (!shell->in_child)
		rl_clear_history();
	cleanup_and_exit(shell, 2);
}

/**
 * Handle exit with too many arguments
 * @param shell Shell state
 * @return Always returns 1
 */
static int	exit_too_many_args(t_shell *shell)
{
	if (shell->interactive && !shell->in_child)
		ft_putendl_fd("exit", STDERR_FILENO);
	ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
	return (1);
}

/**
 * Behavior and design notes
 *
 * Cases:
 *  - exit → prints "exit", exits with last_exit_status
 *  - exit <num> → prints "exit", exits with (unsigned char)<num>
 *  - exit <non-num> → prints "exit" + "numeric argument required", exits 2
 *  - exit <num> <extra> → prints "exit" + "too many arguments", returns 1
 *
 * Parsing: optional [+-] followed by digits only. Overflow → invalid.
 * LLONG_MIN rejected as overflow (strict implementation).
 * "exit" printed on exit or user error (not in child processes).
 *
 * Builtin: exit
 * Exit the shell with optional exit code
 * @param cmd Command structure with arguments
 * @param shell Shell state
 * @return 1 if too many arguments (doesn't exit), otherwise exits process
 */
int	builtin_exit(t_command *cmd, t_shell *shell)
{
	int			argc;
	long long	val;

	argc = count_array(cmd->argv);
	if (argc == 1)
		exit_no_args(cmd, shell);
	if (!is_valid_exit_arg(cmd->argv[1], &val))
		exit_invalid_arg(cmd->argv[1], cmd, shell);
	if (argc > 2)
		return (exit_too_many_args(shell));
	if (shell->interactive && !shell->in_child)
		ft_putendl_fd("exit", STDERR_FILENO);
	free_commands(cmd);
	if (!shell->in_child)
		rl_clear_history();
	cleanup_and_exit(shell, (unsigned char)val);
	return (0);
}
