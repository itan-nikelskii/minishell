/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   xenv_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 23:23:48 by antoniocoss       #+#    #+#             */
/*   Updated: 2025/11/09 20:26:28 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Add a key to the xenv array if it doesn't already exist
 * @param shell The shell structure containing xenv
 * @param key The key to add
 * @return 0 on success, -1 on memory allocation failure
 */
int	xenv_add(t_shell *shell, const char *key)
{
	char	*key_dup;
	char	**new_xenv;

	if (!key || find_xenv(shell->xenv, key) >= 0)
		return (0);
	key_dup = ft_strdup(key);
	if (!key_dup)
		return (-1);
	new_xenv = array_add_entry(shell->xenv, key_dup);
	if (!new_xenv)
	{
		free(key_dup);
		return (-1);
	}
	free(shell->xenv);
	shell->xenv = new_xenv;
	return (0);
}

/**
 * Remove a key from the xenv array if it exists
 * @param shell The shell structure containing xenv
 * @param key The key to remove
 * @return 0 on success, -1 on memory allocation failure
 */
int	xenv_remove(t_shell *shell, const char *key)
{
	int		key_idx;
	char	**new_xenv;

	key_idx = find_xenv(shell->xenv, key);
	if (key_idx < 0)
		return (0);
	new_xenv = array_remove_at(shell->xenv, key_idx);
	if (!new_xenv)
		return (-1);
	free(shell->xenv);
	shell->xenv = new_xenv;
	return (0);
}
