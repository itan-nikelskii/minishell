/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 18:41:09 by acossari          #+#    #+#             */
/*   Updated: 2025/11/06 12:01:10 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/* Count and return the number of arguments, or 0 on error. */	// MODIFIED: new helper to help with line count in resolve_cd_path
static size_t	count_args(char **argv)
{
	size_t	i;

	i = 0;
	if (!argv)
		return (0);
	while (argv[i])
		i++;
	if (i > 0)
		return (i - 1);
	return (0);
}

/**
 * Resolve the target path for cd command
 * @param cmd Command structure with argv
 * @param shell Shell state with envp
 * @param print_path Set to true if path should be printed after cd (cd -)
 * @return Path string, or NULL on error (already printed)
 */
// MODIFIED: added explicit argument counting instead of directly checking for cmd->argv[2] (this fixes issue #30);
// also rewrote the third if block a bit (it was checking for !path twice for no reason, this should be safer now)
// Sorry for messing with line count! 
static char	*resolve_cd_path(t_command *cmd, t_shell *shell, bool *print_path)
{
	char	*path;
	size_t	arg_count;

	if (cmd && cmd->argv)
		arg_count = count_args(cmd->argv);
	else
		arg_count = 0;
	if (arg_count > 1)
		return (print_error("cd", "too many arguments"), NULL);
	*print_path = false;
	if (!cmd || !cmd->argv || !cmd->argv[1])
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
			*print_path = true;
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
