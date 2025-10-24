/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   xenv_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 23:23:48 by antoniocoss       #+#    #+#             */
/*   Updated: 2025/10/15 19:55:15 by antoniocoss      ###   ########.fr       */
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
	int		current_count;
	int		i;
	char	**new_xenv;

	if (!key || find_xenv(shell->xenv, key) >= 0)
		return (0);
	current_count = count_array(shell->xenv);
	new_xenv = malloc(sizeof(char *) * (current_count + 2));
	if (!new_xenv)
		return (-1);
	i = 0;
	while (i < current_count)
	{
		new_xenv[i] = shell->xenv[i];
		i++;
	}
	new_xenv[i] = ft_strdup(key);
	if (!new_xenv[i])
		return (free(new_xenv), -1);
	new_xenv[i + 1] = NULL;
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
	int		current_count;
	int		key_idx;
	char	**new_xenv;

	key_idx = find_xenv(shell->xenv, key);
	if (key_idx < 0)
		return (0);
	current_count = count_array(shell->xenv);
	new_xenv = malloc(sizeof(char *) * current_count);
	if (!new_xenv)
		return (-1);
	free(shell->xenv[key_idx]);
	array_copy_except(new_xenv, shell->xenv, key_idx, current_count);
	free(shell->xenv);
	shell->xenv = new_xenv;
	return (0);
}
