/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 14:02:43 by inikelsk          #+#    #+#             */
/*   Updated: 2025/11/08 21:37:40 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser.h"

/* Build bash-style error message with specific token (FIX 48, FIX 52)
 * Reads 1 or 2 chars from line[i]: if doubled (<<, >>), show both.
 * Otherwise show single char. (FIX 52: simplified - direct index) */
char	*build_redir_error(const char *line, size_t i)
{
	char	token_str[50];
	size_t	len;
	int		j;

	ft_strlcpy(token_str, "syntax error near unexpected token `", 50);
	len = ft_strlen(token_str);
	j = 0;
	if (!line[i] || line[i] == '\n')
	{
		ft_strlcat(token_str, "newline'", 50);
		return (ft_strdup(token_str));
	}
	token_str[len + j++] = line[i];
	if (line[i + 1] && (line[i + 1] == line[i]))
		token_str[len + j++] = line[i + 1];
	token_str[len + j] = '\'';
	token_str[len + j + 1] = '\0';
	return (ft_strdup(token_str));
}

/* Return the shell’s last exit status as a string. If shell is NULL, return 0.
   Caller must free() the result. */
char	*get_exit_status_str(t_shell *shell)
{
	int		n;
	char	*str;

	if (shell == NULL)
		n = 0;
	else
		n = shell->last_exit_status;
	str = ft_itoa(n);
	return (str);
}
