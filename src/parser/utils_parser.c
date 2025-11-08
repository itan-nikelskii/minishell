/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 14:02:43 by inikelsk          #+#    #+#             */
/*   Updated: 2025/11/08 19:40:10 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser.h"

/* Build bash-style error message with specific token (FIX 48) */
char	*build_redir_error(char bad_char)
{
	char	*msg;
	char	token_str[50];

	ft_strlcpy(token_str, "syntax error near unexpected token `", 50);
	token_str[ft_strlen(token_str)] = bad_char;
	token_str[ft_strlen(token_str) + 1] = '\0';
	ft_strlcat(token_str, "'", 50);
	msg = ft_strdup(token_str);
	return (msg);
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
