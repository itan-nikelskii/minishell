/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 14:02:43 by inikelsk          #+#    #+#             */
/*   Updated: 2025/11/11 09:35:16 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser.h"

/* Build bash-style error message with specific token. Read 1 or 2 chars from 
 * line[i]: if doubled (<<, >>), show both; otherwise show single char. */
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
