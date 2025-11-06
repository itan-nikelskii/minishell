/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 18:41:09 by acossari          #+#    #+#             */
/*   Updated: 2025/11/06 20:14:05 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Resolve the target path for cd command
 * @param cmd Command structure with argv
 * @param shell Shell state with envp
 * @param is_dash Set to true if path should be printed after cd (cd -)
 * @return Path string, or NULL on error (already printed)
 */
static char	*resolve_cd_path(t_command *cmd, t_shell *shell, bool *is_dash)
{
	char	*path;
	int		arg_count;

	arg_count = count_array(cmd->argv);
	if (arg_count > 2)
		return (print_error("cd", "too many arguments"), NULL);
	*is_dash = false;
	if (arg_count == 1)
	{
		path = get_env_value("HOME", shell->envp);
		if (!path || !*path)
			return (print_error("cd", "HOME not set"), NULL);
	}
	else
	{
		path = cmd->argv[1];
		if (ft_strncmp(path, "-", 2) == 0)
		{
			path = get_env_value("OLDPWD", shell->envp);
			if (!path || !*path)
				return (print_error("cd", "OLDPWD not set"), NULL);
			*is_dash = true;
		}
	}
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
