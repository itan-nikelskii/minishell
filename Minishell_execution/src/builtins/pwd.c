/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 10:21:47 by acossari          #+#    #+#             */
/*   Updated: 2025/10/23 13:50:19 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Builtin: pwd
 * Prints the current working directory
 * @param cmd Command structure (unused)
 * @param shell Shell state (unused)
 * @return 0 on success, 1 on error
 */
int	builtin_pwd(t_command *cmd, t_shell *shell)
{
	char	*path;

	(void)cmd;
	(void)shell;
	path = getcwd(NULL, 0);
	if (!path)
	{
		print_perror("pwd", NULL);
		return (1);
	}
	ft_putendl_fd(path, STDOUT_FILENO);
	free(path);
	return (0);
}
