/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   search_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 20:22:46 by acossari          #+#    #+#             */
/*   Updated: 2025/10/21 13:02:06 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Generic function to find a key in a string array
 * @param arr Array to search in
 * @param key Key to find
 * @param require_equal If true, searches for exact match (xenv case)
 *                      If false, searches for "key=" pattern (envp case)
 * @return Index if found, -1 otherwise
 */
int	find_in_array(char **arr, const char *key, bool require_equal)
{
	int		i;
	size_t	len;

	if (!arr || !key)
		return (-1);
	len = ft_strlen(key);
	i = 0;
	while (arr[i])
	{
		if (ft_strncmp(arr[i], key, len) == 0)
		{
			if (require_equal && ft_strlen(arr[i]) == len)
				return (i);
			if (!require_equal && arr[i][len] == '=')
				return (i);
		}
		i++;
	}
	return (-1);
}

/**
 * Find index of environment variable in envp
 * @param envp Environment array
 * @param key Variable name
 * @return Index if found, -1 otherwise
 */
int	find_env(char **envp, char *key)
{
	return (find_in_array(envp, key, false));
}

/**
 * Find index of key in xenv array
 * @param xenv The exported names without values array
 * @param key The key to search for
 * @return Index if found, -1 otherwise
 */
int	find_xenv(char **xenv, const char *key)
{
	return (find_in_array(xenv, key, true));
}
