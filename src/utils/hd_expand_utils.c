/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hd_expand_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 18:49:53 by acossari          #+#    #+#             */
/*   Updated: 2025/11/11 00:04:36 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Append string to dynamic buffer
 * @param buf: Buffer to append to
 * @param str: String to append
 * @return: 0 on success, -1 on malloc failure
 */
int	hd_append_str(char **buf, const char *str)
{
	char	*tmp;

	tmp = ft_strjoin(*buf, str);
	if (!tmp)
		return (-1);
	free(*buf);
	*buf = tmp;
	return (0);
}

/**
 * Append single character to dynamic buffer
 * @param buf: Buffer to append to
 * @param c: Character to append
 * @return: 0 on success, -1 on malloc failure
 */
int	hd_append_char(char **buf, char c)
{
	char	tmp[2];

	tmp[0] = c;
	tmp[1] = '\0';
	return (hd_append_str(buf, tmp));
}

/**
 * Extract variable name from position i in string str
 * Variable names: [a-zA-Z_][a-zA-Z0-9_]*
 * @param str: Input string
 * @param i: Starting position (after $)
 * @return: Allocated variable name or NULL
 */
char	*hd_extract_varname(const char *str, size_t i)
{
	size_t	start;
	size_t	len;

	start = i;
	if (!ft_isalpha(str[i]) && str[i] != '_')
		return (NULL);
	while (ft_isalnum(str[i]) || str[i] == '_')
		i++;
	len = i - start;
	return (ft_substr(str, start, len));
}
