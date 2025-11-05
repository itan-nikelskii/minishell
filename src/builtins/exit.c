/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 12:43:11 by antoniocoss       #+#    #+#             */
/*   Updated: 2025/11/05 11:57:12 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Handle exit with no arguments (FIX ISSUE 8.2)
 * @param shell Shell state
 */
static void	exit_no_args(t_shell *shell)
{
	int exit_status;

	if (shell->interactive && !shell->in_child)
		ft_putendl_fd("exit", STDERR_FILENO);
	// MODIFIED: save the exit status before freeing shell to avoid use-after-free:
	exit_status = shell->last_exit_status;
	// MODIFIED: if it's the main shell (not a child), do cleanup so we don't
	// leak allocations tracked by valgrind (env, xenv, history, etc):
	if (!shell->in_child)
	{
		clear_history();
		shell_cleanup(shell);
	}
	exit(exit_status);
}

/**
 * Handle exit with invalid numeric argument (FIX ISSUE 8.2)
 * @param arg The invalid argument
 * @param shell Shell state
 */
static void	exit_invalid_arg(char *arg, t_shell *shell)
{
	if (shell->interactive && !shell->in_child)
		ft_putendl_fd("exit", STDERR_FILENO);
	ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
	// MODIFIED: these three if-else-if blocks ensure that exit 100000000000000
	// does not leak:
	if (arg)
	{
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putendl_fd(": numeric argument required", STDERR_FILENO);
	}
	else
		ft_putendl_fd(": numeric argument required", STDERR_FILENO);
	if (arg)
		free(arg);
	if (!shell->in_child)
	{
		clear_history();
		shell_cleanup(shell);
	}
	exit(2);
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
	{
		// MODIFIED: free the built command struc before exiting:
		free_commands(cmd);
		exit_no_args(shell);
	}
	if (!is_valid_exit_arg(cmd->argv[1], &val))
	{
		// MODIFIED: duplicate the invalid arg before freeing cmd so the error
		// printer can safely read it (otherwise invalid read size issues)
		char *bad_arg = ft_strdup(cmd->argv[1]);
		free_commands(cmd);
		if (!bad_arg)
			exit_invalid_arg(NULL, shell);
		exit_invalid_arg(bad_arg, shell);
	}
	if (argc > 2)
		return (exit_too_many_args(shell));
	if (shell->interactive && !shell->in_child)
		ft_putendl_fd("exit", STDERR_FILENO);
	// MODIFIED: free command resources for the command being executed, then
	// cleanup shell/global resources when exiting the main shell:
	if (!shell->in_child)
	{
		free_commands(cmd);
		clear_history();
		shell_cleanup(shell);
	}
	exit((unsigned char)val);
}
