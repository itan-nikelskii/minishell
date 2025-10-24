/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 18:41:09 by acossari          #+#    #+#             */
/*   Updated: 2025/10/23 13:50:19 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Resolve the target path for cd command
 * @param cmd Command structure with argv
 * @param shell Shell state with envp
 * @param print_path Set to true if path should be printed after cd (cd -)
 * @return Path string, or NULL on error (already printed)
 */
static char	*resolve_cd_path(t_command *cmd, t_shell *shell, bool *print_path)
{
	char	*path;

	if (cmd->argv[2])
		return (print_error("cd", "too many arguments"), NULL);
	path = cmd->argv[1];
	*print_path = false;
	if (!path)
	{
		path = get_env_value("HOME", shell->envp);
		if (!path || !*path)
			return (print_error("cd", "HOME not set"), NULL);
	}
	else if (ft_strncmp(path, "-", 2) == 0)
	{
		path = get_env_value("OLDPWD", shell->envp);
		if (!path || !*path)
			return (print_error("cd", "OLDPWD not set"), NULL);
		*print_path = true;
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
	bool	print_path;

	path = resolve_cd_path(cmd, shell, &print_path);
	if (!path)
		return (1);
	oldpwd = getcwd(NULL, 0);
	if (chdir(path) == -1)
		return (print_perror("cd", path), free(oldpwd), 1);
	if (print_path)
		ft_putendl_fd(path, 1);
	newpwd = getcwd(NULL, 0);
	if (oldpwd)
		env_update(shell, "OLDPWD", oldpwd);
	if (newpwd)
		env_update(shell, "PWD", newpwd);
	return (free(oldpwd), free(newpwd), 0);
}
