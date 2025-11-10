/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   array_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 19:37:21 by acossari          #+#    #+#             */
/*   Updated: 2025/11/09 20:30:32 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Count elements in NULL-terminated string array
 * @param array Array to count (e.g., argv, envp)
 * @return Number of elements (not including NULL)
 */
int	count_array(char **array)
{
	int	count;

	count = 0;
	while (array && array[count])
		count++;
	return (count);
}

/**
 * Compare two environment variable keys (before '=')
 * @param a First string (KEY=VALUE or KEY)
 * @param b Second string (KEY=VALUE or KEY)
 * @return < 0 if a < b, 0 if equal, > 0 if a > b
 */
int	keycmp(const char *a, const char *b)
{
	int	i;

	i = 0;
	while (a[i] && b[i] && a[i] != '=' && b[i] != '=')
	{
		if (a[i] != b[i])
			return ((unsigned char)a[i] - (unsigned char)b[i]);
		i++;
	}
	if ((a[i] == '=' || a[i] == '\0') && (b[i] == '=' || b[i] == '\0'))
		return (0);
	if (a[i] == '=' || a[i] == '\0')
		return (-1);
	return (1);
}

/**
 * Sort environment array alphabetically by key (bubble sort)
 * @param env Array to sort (modified in place)
 */
void	sort_env_array(char **env)
{
	int		i;
	int		j;
	int		count;
	char	*tmp;

	if (!env)
		return ;
	count = count_array(env);
	i = 0;
	while (i < count - 1)
	{
		j = 0;
		while (j < count - i - 1)
		{
			if (keycmp(env[j], env[j + 1]) > 0)
			{
				tmp = env[j];
				env[j] = env[j + 1];
				env[j + 1] = tmp;
			}
			j++;
		}
		i++;
	}
}

/**
 * Remove element at specified index from NULL-terminated array
 * Generic function used by both env_remove and xenv_remove
 * @param array Original array to modify
 * @param index Index of element to remove
 * @return New array with element removed, or NULL on error
 * @note Caller must free the old array after successful removal
 */
char	**array_remove_at(char **array, int index)
{
	char	**new_array;
	int		count;
	int		i;
	int		j;

	if (!array || index < 0)
		return (NULL);
	count = count_array(array);
	if (index >= count)
		return (NULL);
	new_array = malloc(sizeof(char *) * count);
	if (!new_array)
		return (NULL);
	free(array[index]);
	i = 0;
	j = 0;
	while (i < count)
	{
		if (i != index)
			new_array[j++] = array[i];
		i++;
	}
	new_array[j] = NULL;
	return (new_array);
}

/**
 * Add entry to NULL-terminated array
 * Generic function used by both env_add and xenv_add
 * @param array Original array to expand
 * @param new_entry New entry to add (must be already allocated)
 * @return New array with entry added, or NULL on error
 * @note Caller must free the old array after successful addition
 */
char	**array_add_entry(char **array, char *new_entry)
{
	char	**new_array;
	int		count;
	int		i;

	if (!new_entry)
		return (NULL);
	count = count_array(array);
	new_array = malloc(sizeof(char *) * (count + 2));
	if (!new_array)
		return (NULL);
	i = 0;
	while (i < count)
	{
		new_array[i] = array[i];
		i++;
	}
	new_array[i] = new_entry;
	new_array[i + 1] = NULL;
	return (new_array);
}
