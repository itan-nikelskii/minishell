/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 15:27:42 by acossari          #+#    #+#             */
/*   Updated: 2025/11/10 19:05:29 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Duplicate environment array (deep copy)
 * @param envp Original environment array
 * @return Allocated copy of envp, or NULL on failure
 */
char	**env_dup(char **envp)
{
	char	**new_envp;
	int		count;
	int		i;

	if (!envp)
		return (NULL);
	count = count_array(envp);
	new_envp = malloc(sizeof(char *) * (count + 1));
	if (!new_envp)
		return (NULL);
	i = 0;
	while (i < count)
	{
		new_envp[i] = ft_strdup(envp[i]);
		if (!new_envp[i])
		{
			while (--i >= 0)
				free(new_envp[i]);
			return (free(new_envp), NULL);
		}
		i++;
	}
	new_envp[i] = NULL;
	return (new_envp);
}

/**
 * Create new environment entry "key=value"
 * @param key Variable name
 * @param value Variable value
 * @return Allocated string "key=value", or NULL on failure
 */
static char	*create_env_entry(char *key, char *value)
{
	char	*entry;
	char	*tmp;

	tmp = ft_strjoin(key, "=");
	if (!tmp)
		return (NULL);
	entry = ft_strjoin(tmp, value);
	free(tmp);
	return (entry);
}

/**
 * Add new environment variable to shell->envp
 * @param shell Shell state
 * @param new_entry New entry to add
 * @return 0 on success, -1 on failure
 */
static int	env_add(t_shell *shell, char *new_entry)
{
	char	**new_envp;

	new_envp = array_add_entry(shell->envp, new_entry);
	if (!new_envp)
		return (-1);
	free(shell->envp);
	shell->envp = new_envp;
	return (0);
}

/**
 * Update or add environment variable in shell->envp
 * @param shell Shell state
 * @param key Variable name
 * @param value New value
 * @return 0 on success, -1 on failure
 */
int	env_update(t_shell *shell, char *key, char *value)
{
	int		idx;
	char	*new_entry;

	if (!shell || !key || !value)
		return (-1);
	new_entry = create_env_entry(key, value);
	if (!new_entry)
		return (-1);
	idx = find_env(shell->envp, key);
	if (idx >= 0)
	{
		free(shell->envp[idx]);
		shell->envp[idx] = new_entry;
	}
	else
	{
		if (env_add(shell, new_entry) == -1)
		{
			free(new_entry);
			return (-1);
		}
	}
	return (0);
}

/**
 * Remove environment variable from shell->envp
 * @param shell Shell state
 * @param key Variable name to remove
 * @return 0 on success, -1 on failure
 */
int	env_remove(t_shell *shell, char *key)
{
	int		key_idx;
	char	**new_env;

	key_idx = find_env(shell->envp, key);
	if (key_idx < 0)
		return (0);
	new_env = array_remove_at(shell->envp, key_idx);
	if (!new_env)
		return (-1);
	free(shell->envp);
	shell->envp = new_env;
	return (0);
}
