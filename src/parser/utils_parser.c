/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 14:02:43 by inikelsk          #+#    #+#             */
/*   Updated: 2025/11/07 10:18:22 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser.h"

/* Duplicate and return substring s[from ... to]; return NULL on failure. */
char	*strdup_range(const char *s, size_t from, size_t to)
{
	char	*out;
	size_t	len;

	len = to - from;
	out = malloc(len + 1);
	if (!out)
		return (NULL);
	ft_memcpy(out, s + from, len);
	out[len] = '\0';
	return (out);
}

/* If c is a word char (non-meta or space), return 1; otherwise, return 0. */
int	is_word_char(char c)
{
	if (ft_isspace((unsigned char)c))
		return (0);
	if (c == '|' || c == '<' || c == '>' || c == '\'' || c == '"')
		return (0);
	return (1);
}

/* Append char c to the buffer buf; return 0 on sucess, -1 on failure. */
int	append_char(t_dyn_buf *dynamic_buf, char c)
{
	int	status;

	status = ensure_buffer_capacity(dynamic_buf, dynamic_buf->len + 2);
	if (status != 0)
		return (-1);
	dynamic_buf->buf[dynamic_buf->len] = c;
	dynamic_buf->len += 1;
	dynamic_buf->buf[dynamic_buf->len] = '\0';
	return (0);
}

/* Append string s to buffer buf; return 0 on sucess, -1 on failure. */
int	append_str(t_dyn_buf *dynamic_buf, const char *s)
{
	size_t	s_len;
	int		status;

	s_len = ft_strlen(s);
	status = ensure_buffer_capacity(dynamic_buf, dynamic_buf->len + s_len + 1);
	if (status != 0)
		return (-1);
	ft_memcpy(dynamic_buf->buf + dynamic_buf->len, s, s_len);
	dynamic_buf->len += s_len;
	dynamic_buf->buf[dynamic_buf->len] = '\0';
	return (0);
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
