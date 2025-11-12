/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_quotes.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 14:12:02 by inikelsk          #+#    #+#             */
/*   Updated: 2025/11/12 10:31:54 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser.h"

/* Parse single quoted literal (no expansion) and store it in *out;
   return 0 on sucess, -1 if something went wrong. */
int	parse_single_quote(const char *s, size_t *i, char **out)
{
	size_t	start;
	size_t	j;
	char	*str;

	start = *i + 1;
	j = start;
	while (s[j] != '\0' && s[j] != '\'')
		j++;
	if (s[j] != '\'')
		return (-1);
	str = strdup_range(s, start, j);
	if (!str)
		return (-1);
	*i = j + 1;
	*out = str;
	return (0);
}

/* Parse double-quoted string, allowing for $ expansion, and store it in *out.
   Don't expand $ in heredoc delimiters (shell == NULL).
   Return 0 on sucess, -1 if something went wrong. */
int	parse_double_quote(const char *s, size_t *i, char **out, t_shell *shell)
{
	size_t		j;
	t_dyn_buf	*dynamic_buf;

	dynamic_buf = dynbuf_create_and_init();
	if (!dynamic_buf)
		return (-1);
	j = *i + 1;
	while (s[j] != '\0' && s[j] != '"')
	{
		if (s[j] == '$' && shell != NULL)
		{
			if (expand_dollar(s, &j, dynamic_buf, shell) != 0)
				return (dynamic_buf_free(dynamic_buf), -1);
			continue ;
		}
		if (append_char(dynamic_buf, s[j]) != 0)
			return (dynamic_buf_free(dynamic_buf), -1);
		j++;
	}
	if (s[j] != '"')
		return (dynamic_buf_free(dynamic_buf), -1);
	*out = dynamic_buf->buf;
	free(dynamic_buf);
	*i = j + 1;
	return (0);
}

/* Handle special characters: \ and $. Return 1 if handling was successful,
-1 on error, and 0 if it was not a special character (the caller should do
a normal append). */
static int	handle_special_char(const char *s, size_t *j, t_dyn_buf *buf,
			t_shell *shell)
{
	if (s[*j] == '\\' && s[*j + 1] != '\0')
	{
		if (append_char(buf, s[*j + 1]) != 0)
			return (-1);
		*j += 2;
		return (1);
	}
	if (s[*j] == '$' && shell != NULL)
	{
		if (expand_dollar(s, j, buf, shell) != 0)
			return (-1);
		return (1);
	}
	return (0);
}

/* Parse an unquoted word, allowing for $ expansion, and store it in *out.
   Don't expand $ in heredoc delimiters (shell == NULL).
   Return 0 on sucess, -1 if something went wrong. */
int	parse_unquoted_word(const char *s, size_t *i, char **out, t_shell *shell)
{
	size_t		j;
	t_dyn_buf	*dynamic_buf;
	int			char_handle_result;

	dynamic_buf = dynbuf_create_and_init();
	if (!dynamic_buf)
		return (-1);
	j = *i;
	while (s[j] != '\0' && is_word_char(s[j]))
	{
		char_handle_result = handle_special_char(s, &j, dynamic_buf, shell);
		if (char_handle_result == -1)
			return (dynamic_buf_free(dynamic_buf), -1);
		if (char_handle_result == 1)
			continue ;
		if (append_char(dynamic_buf, s[j]) != 0)
			return (dynamic_buf_free(dynamic_buf), -1);
		j++;
	}
	*out = dynamic_buf->buf;
	free(dynamic_buf);
	*i = j;
	return (0);
}
