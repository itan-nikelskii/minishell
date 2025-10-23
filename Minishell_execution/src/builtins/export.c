/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 16:18:34 by acossari          #+#    #+#             */
/*   Updated: 2025/10/23 13:50:19 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Export a variable without a value (only key).
 * @param key The variable name to export.
 * @param shell The shell structure containing environment variables.
 * @return 0 on success, 1 on error.
 */
static int	export_without_value(char *key, t_shell *shell)
{
	if (find_env(shell->envp, key) >= 0)
		return (0);
	return (xenv_add(shell, key) == -1);
}

/**
 * Export a variable with a value (handles KEY= and KEY=VALUE).
 * @param identifier The identifier to check.
 * @return 1 if valid, 0 otherwise.
 */
static int	export_with_value(char *arg, char *equal_pos, t_shell *shell)
{
	char	*key;
	char	*val;

	key = ft_substr(arg, 0, equal_pos - arg);
	if (!key)
		return (1);
	val = equal_pos + 1;
	if (env_update(shell, key, val) == -1)
		return (free(key), 1);
	xenv_remove(shell, key);
	return (free(key), 0);
}

/**
 * Process a single export argument.
 * @param arg The argument to process.
 * @param shell The shell structure containing environment variables.
 * @return Exit status (0 on success, 1 on error).
 */
static int	process_export_arg(char *arg, t_shell *shell)
{
	char	*equal_sign;

	if (!arg || !*arg)
		return (1);
	if (!is_valid_export_identifier(arg))
	{
		print_error_arg("export", arg, "not a valid identifier");
		return (1);
	}
	equal_sign = ft_strchr(arg, '=');
	if (equal_sign)
		return (export_with_value(arg, equal_sign, shell));
	else
		return (export_without_value(arg, shell));
}

/*
** EXPORT behavior (bash-like) – quick reference
**
** Printing:
** - `export` with no args prints the sorted union of:
**   - envp entries (KEY=VALUE)
**   - xenv entries (bare KEY without '=')
**   Format: `declare -x KEY="VALUE"` or `declare -x KEY`
**   Note: `export` output is sorted by KEY; `env` is not sorted.
**
** Cases:
** 1) export KEY           (no '=')
**    - If KEY exists in envp: do nothing.
**    - Else: add KEY to xenv (tracked as "bare"). Not passed to execve.
**    - `export` shows:  declare -x KEY
**    - `env` shows:     (nothing for KEY)
**
** 2) export KEY=          (empty value)
**    - Set/overwrite envp with KEY="" (i.e., "KEY=").
**    - Remove KEY from xenv if present.
**    - `export` shows:  declare -x KEY=""
**    - `env` shows:     KEY=
**
** 3) export KEY=VALUE
**    - Set/overwrite envp with KEY=VALUE.
**    - Remove KEY from xenv if present.
**    - `export` shows:  declare -x KEY="VALUE"
**    - `env` shows:     KEY=VALUE
**
** Additional notes:
** - Invalid identifiers (first char not [A-Za-z_], or chars outside
**   [A-Za-z0-9_]) trigger:
**     minishell: export: `ARG': not a valid identifier
**   Exit status becomes 1, but remaining args are processed.
** - Duplicate args are processed left→right; last assignment wins.
** - Unsupported flags (e.g., -p, -n) and `KEY+=VAL` are treated as invalid.
** - Only envp is inherited by children (execve). xenv is internal tracking.
**
** Examples:
**   $ export KEY1
**   $ export KEY2=
**   $ export KEY3=test
**
**   $ env
**   ...              (order not guaranteed)
**   KEY2=
**   KEY3=test
**
**   $ export
**   declare -x KEY1
**   declare -x KEY2=""
**   declare -x KEY3="test"   (sorted by key)
*/

/**
 * Builtin command: export
 * Handles exporting environment variables.
 * @param cmd The command structure containing arguments.
 * @param shell The shell structure containing environment variables.
 * @return Exit status (0 on success, 1 on error).
*/
int	builtin_export(t_command *cmd, t_shell *shell)
{
	int	i;
	int	exit_status;

	if (!cmd->argv[1])
		return (print_export_merged(shell), 0);
	exit_status = 0;
	i = 1;
	while (cmd->argv[i])
	{
		if (process_export_arg(cmd->argv[i], shell))
			exit_status = 1;
		i++;
	}
	return (exit_status);
}
