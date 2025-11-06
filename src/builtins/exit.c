/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 12:43:11 by antoniocoss       #+#    #+#             */
/*   Updated: 2025/11/06 14:43:35 by antoniocoss      ###   ########.fr       */
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
 * Handle exit with too many arguments (FIX ISSUE 8.2)
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

/*
** EXIT — Behavior and design notes
**
** Cases and exit codes:
**  - exit → prints "exit" and terminates with last_exit_status
**  - exit <num> (valid parse) → prints "exit" and terminates with
**    (unsigned char)<num>
**  - exit <non-num> → prints "exit" + "numeric argument required"
**    to stderr and TERMINATES with exit code 2
**  - exit <num> <extra> → prints "exit" + "too many arguments"
**    to stderr, does NOT terminate the shell, returns 1
**
** Strict parsing:
**  - Accepts only: optional [ + | - ] followed by >=1 digits
**    (no spaces, no extra chars).
**  - Builds the absolute value as positive, checking for overflow
**    BEFORE acc * 10 + digit.
**  - If the operation would exceed LLONG_MAX → INVALID (treated as
**    non-numeric).
**
** Design choice regarding LLONG_MIN:
**  - We intentionally do NOT handle the special case
**    "-9223372036854775808" (|LLONG_MIN| = LLONG_MAX + 1).
**  - That string is rejected by the parser as overflow →
**    "numeric argument required", exit 2.
**  - Note: some bash versions accept LLONG_MIN; we deliberately keep
**    a strict implementation.
**
** "exit" printing:
**  - Printed whenever the builtin exits or reports a user error on
**    exit usage.
**  - Should NOT be printed in child processes or pipelines when the
**    main shell stays alive; this policy is handled at the executor
**    level (here we assume the single-command case).
*/

/**
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
