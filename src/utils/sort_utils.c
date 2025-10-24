/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 23:52:34 by antoniocoss       #+#    #+#             */
/*   Updated: 2025/10/15 13:26:20 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/*
** keycmp – compares only the KEY part of "KEY=VALUE" strings.
** The '=' character is treated as a terminator (like '\0'): values are ignored.
**
** Four cases:
** 1) Keys are identical (up to '=' or '\0')  -> return 0
**    Examples: "FOO=1" vs "FOO=2"  => 0
**              "FOO"   vs "FOO=bar" => 0
**
** 2) `a` is a strict prefix of `b`'s key     -> return < 0
**    Examples: "FOO"   vs "FOOBAR=1" => negative
**
** 3) `b` is a strict prefix of `a`'s key     -> return > 0
**    Examples: "FOOBAR=1" vs "FOO"   => positive
**
** 4) First mismatching character decides     -> (unsigned)a[i] - (unsigned)b[i]
**    Examples: "BAR=1"  vs "FOO=2"   => 'B' - 'F' < 0 (negative)
**              "FOO2=2" vs "FOO1=1"  => '2' - '1' > 0 (positive)
*/

/**
 * Compare two environment variable keys (up to '=' or end)
 * @param a First environment variable string
 * @param b Second environment variable string
 * @return Negative if a < b, positive if a > b, 0 if equal
 */
int	keycmp(const char *a, const char *b)
{
	size_t	i;

	i = 0;
	while (a[i] && a[i] != '=' && b[i] && b[i] != '=')
	{
		if (a[i] != b[i])
			return ((unsigned char)a[i] - (unsigned char)b[i]);
		i++;
	}
	if ((a[i] == '\0' || a[i] == '=') && (b[i] == '\0' || b[i] == '='))
		return (0);
	if (a[i] == '\0' || a[i] == '=')
		return (-1);
	return (1);
}

/**
 * Sort an array of environment variable strings in place
 * using bubble sort based on keys
 * @param env The array of environment variable strings to sort
 */
void	sort_env_array(char **env)
{
	int		i;
	int		j;
	char	*tmp;

	i = 0;
	while (env[i])
	{
		j = i + 1;
		while (env[j])
		{
			if (keycmp(env[i], env[j]) > 0)
			{
				tmp = env[i];
				env[i] = env[j];
				env[j] = tmp;
			}
			j++;
		}
		i++;
	}
}
