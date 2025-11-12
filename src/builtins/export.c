/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 16:18:34 by acossari          #+#    #+#             */
/*   Updated: 2025/11/12 10:31:35 by inikelsk         ###   ########.fr       */
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
	char	*key_dup;

	if (find_env(shell->envp, key) >= 0)
		return (0);
	key_dup = ft_strdup(key);
	if (!key_dup)
		return (1);
	if (xenv_add(shell, key_dup) == -1)
		return (free(key_dup), 1);
	return (0);
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

/**
 * Builtin: export
 * Behavior:
 *   export          → print sorted "declare -x name[=value]" (envp + xenv)
 *   export name     → add to xenv if not in envp (not passed to children)
 *   export name=val → add/update envp, remove from xenv (passed to children)
 * Examples:
 *   export TEST        → xenv only (not in env output)
 *   export TEST=hello  → envp (shows in env output)
 * @param cmd Command structure with arguments
 * @param shell Shell state
 * @return 0 on success, 1 if any argument failed
 */
int	builtin_export(t_command *cmd, t_shell *shell)
{
	int	i;
	int	exit_status;

	if (!cmd->argv[1])
		return (print_export_merged(shell), 0);
	i = 1;
	exit_status = 0;
	while (cmd->argv[i])
	{
		if (process_export_arg(cmd->argv[i], shell))
			exit_status = 1;
		i++;
	}
	return (exit_status);
}
