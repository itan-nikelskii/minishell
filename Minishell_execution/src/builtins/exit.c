/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 12:43:11 by antoniocoss       #+#    #+#             */
/*   Updated: 2025/10/23 13:50:19 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Handle exit with no arguments
 * @param shell Shell state
 */
static void	exit_no_args(t_shell *shell)
{
	ft_putendl_fd("exit", STDERR_FILENO);
	exit(shell->last_exit_status);
}

/**
 * Handle exit with invalid numeric argument
 * @param arg The invalid argument
 */
static void	exit_invalid_arg(char *arg)
{
	ft_putstr_fd("exit\nminishell: exit: ", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putendl_fd(": numeric argument required", STDERR_FILENO);
	exit(2);
}

/**
 * Handle exit with too many arguments
 * @return Always returns 1
 */
static int	exit_too_many_args(void)
{
	ft_putendl_fd("exit", STDERR_FILENO);
	ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
	return (1);
}

/*
** EXIT — Behavior and design notes (minishell)
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
		exit_no_args(shell);
	if (!is_valid_exit_arg(cmd->argv[1], &val))
		exit_invalid_arg(cmd->argv[1]);
	if (argc > 2)
		return (exit_too_many_args());
	ft_putendl_fd("exit", STDERR_FILENO);
	exit((unsigned char)val);
}
