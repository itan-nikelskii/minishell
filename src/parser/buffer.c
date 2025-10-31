/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buffer.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 14:25:18 by inikelsk          #+#    #+#             */
/*   Updated: 2025/10/31 11:18:16 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parser.h"

/* Create a dynamic buffer and initialize it to default values; on success,
return a pointer to it; on failure, return NULL. */
t_dyn_buf	*dynbuf_create_and_init(void)
{
	t_dyn_buf	*dynamic_buf;

	dynamic_buf = malloc(sizeof(t_dyn_buf));
	if (!dynamic_buf)
		return (NULL);
	dynamic_buf->buf = malloc(64);
	if (!dynamic_buf->buf)
		return (free(dynamic_buf), NULL);
	dynamic_buf->len = 0;
	dynamic_buf->cap = 64;
	dynamic_buf->buf[0] = '\0';
	return (dynamic_buf);
}

/* Ensure buffer capacity for 'need' amount of bytes and grow the buffer with
realloc if necessary. Uses:
- append_char() and append_str() rely on this to safely append chars/strings
  without buffer overflow;
- parse_double_quote() and parse_unquoted_word() build their resulting strings
  incrementally (may append literals and then expansions (env vars, $, etc) of 
  unknown size => need a dynamic buffer that can be grown piecewise. */
int	ensure_buffer_capacity(t_dyn_buf *dynamic_buf, size_t need)
{
	char	*tmp;

	if (dynamic_buf->cap >= need)
		return (0);
	while (dynamic_buf->cap < need)
	{
		dynamic_buf->cap *= 2;
		if (dynamic_buf->cap == 0)
			dynamic_buf->cap = 64;
	}
	tmp = ft_realloc(dynamic_buf->buf, dynamic_buf->cap);
	if (!tmp)
		return (-1);
	dynamic_buf->buf = tmp;
	return (0);
}
