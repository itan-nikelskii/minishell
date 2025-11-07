/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 18:41:09 by acossari          #+#    #+#             */
/*   Updated: 2025/11/07 18:14:16 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Get the home directory path from the environment.
 * @param shell Shell state with envp
 * @return Home directory path, or NULL on error
 */
static char	*get_home_path(t_shell *shell)
{
	char	*path;

	path = get_env_value("HOME", shell->envp);
	if (!path || !*path)
	{
		print_error("cd", "HOME not set");
		return (NULL);
	}
	return (path);
}

/**
 * Handle the '--' argument for cd: change to home directory
 * @param cmd Command structure with argv
 * @param shell Shell state with envp
 * @param arg_count Number of arguments in cmd->argv
 * @return Target path for cd, or NULL on error
 */
static char	*handle_double_dash(t_command *cmd, t_shell *shell, int arg_count)
{
	if (arg_count == 2)
		return (get_home_path(shell));
	if (arg_count == 3)
		return (cmd->argv[2]);
	print_error("cd", "too many arguments");
	return (NULL);
}

/**
 * Handle the '-' argument for cd: change to previous directory
 * @param shell Shell state with envp
 * @param is_dash Pointer to bool that will be set to true if path is '-'
 * @return Target path for cd, or NULL on error
 */
static char	*handle_dash(t_shell *shell, bool *is_dash)
{
	char	*path;

	path = get_env_value("OLDPWD", shell->envp);
	if (!path || !*path)
	{
		print_error("cd", "OLDPWD not set");
		return (NULL);
	}
	*is_dash = true;
	return (path);
}

/**
 * Determine the target path for the cd command based on arguments.
 * Flow: arg_count==1 → HOME
 *       argv[1]=="--" → handle_double_dash():
 *           - arg_count==2: "cd --" → HOME
 *           - arg_count==3: "cd -- /tmp" → /tmp
 *           - arg_count>3: "cd -- a b c" → error "too many arguments"
 *       arg_count>2 → error (rejects "cd - /tmp", "cd /foo /bar")
 *       argv[1]=="-" → handle_dash() (OLDPWD, prints path)
 *       default → argv[1]
 * @param cmd Command structure with argv
 * @param shell Shell state with envp
 * @param is_dash Pointer to bool that will be set to true if path is '-'
 * @return Target path for cd, or NULL on error
 */
static char	*resolve_cd_path(t_command *cmd, t_shell *shell, bool *is_dash)
{
	char	*path;
	int		arg_count;

	arg_count = count_array(cmd->argv);
	*is_dash = false;
	if (arg_count == 1)
		return (get_home_path(shell));
	path = cmd->argv[1];
	if (ft_strncmp(path, "--", 3) == 0)
		return (handle_double_dash(cmd, shell, arg_count));
	if (arg_count > 2)
		return (print_error("cd", "too many arguments"), NULL);
	if (ft_strncmp(path, "-", 2) == 0)
		return (handle_dash(shell, is_dash));
	return (path);
}

/**
 * Builtin: cd
 * Changes the current working directory
 * @param cmd Command structure with argv
 * @param shell Shell state with envp
 * @return 0 on success, 1 on failure
 */
int	builtin_cd(t_command *cmd, t_shell *shell)
{
	char	*path;
	char	*oldpwd;
	char	*newpwd;
	bool	is_dash;

	path = resolve_cd_path(cmd, shell, &is_dash);
	if (!path)
		return (1);
	oldpwd = getcwd(NULL, 0);
	if (chdir(path) == -1)
		return (print_perror("cd", path), free(oldpwd), 1);
	if (is_dash)
		ft_putendl_fd(path, STDOUT_FILENO);
	newpwd = getcwd(NULL, 0);
	if (oldpwd)
		env_update(shell, "OLDPWD", oldpwd);
	if (newpwd)
		env_update(shell, "PWD", newpwd);
	return (free(oldpwd), free(newpwd), 0);
}
