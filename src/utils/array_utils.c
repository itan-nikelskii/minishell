/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   array_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 19:37:21 by acossari          #+#    #+#             */
/*   Updated: 2025/10/21 13:00:24 by antoniocoss      ###   ########.fr       */
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
 * Copy array elements except one at specified index
 * @param dest Destination array (must be pre-allocated)
 * @param src Source array
 * @param skip_idx Index to skip
 * @param total Total elements in source
 */
void	array_copy_except(char **dest, char **src, int skip_idx, int total)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (i < total)
	{
		if (i != skip_idx)
			dest[j++] = src[i];
		i++;
	}
	dest[j] = NULL;
}
