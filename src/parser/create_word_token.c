/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_word_token.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 10:54:46 by inikelsk          #+#    #+#             */
/*   Updated: 2025/10/30 13:20:05 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parser.h"

/* Handle leading $ followed by a quoted segment ($'...' or $"...'). Parse the 
 quoted segment and set *segment. Return -1 on error, 1 on parsing success. */
static int	handle_dollar_prefixed_quote(const char *line, size_t *i,
                char **segment, t_shell *shell)
{
    *i = *i + 1;					// skip leading $
    if (line[*i] == '\'')
    {
        if (parse_single_quote(line, i, segment) != 0)
            return (-1);
        return (1);
    }
    if (line[*i] == '"')
    {
        if (parse_double_quote(line, i, segment, shell) != 0)
            return (-1);
        return (1);
    }
    return (-1);
}

/* Parse one segment starting at line[*i]. Handle dollar-prefixed quoted, 
 single-quoted, double-quoted, and unquoted segments. Returns:
 * -1 : error;
 *  1 : parsed segment was quoted (segment set);
 *  0 : parsed unquoted segment (segment set). */
static int	parse_segment(const char *line, size_t *i, char **segment,
				t_shell *shell)
{
	*segment = NULL;
	if (line[*i] == '$' && (line[*i + 1] == '\'' || line[*i + 1] == '"'))
	{
		if (handle_dollar_prefixed_quote(line, i, segment, shell) == -1)
			return (-1);
		return (1);
	}
	if (line[*i] == '\'')
	{
		if (parse_single_quote(line, i, segment) != 0)
			return (-1);
		return (1);
	}
	if (line[*i] == '"')
	{
		if (parse_double_quote(line, i, segment, shell) != 0)
			return (-1);
		return (1);
	}
	if (parse_unquoted_word(line, i, segment, shell) != 0)
		return (-1);
	return (0);
}

/* Append a parsed segment into dynamic buffer and free the segment on success.
   Return 0 on success, -1 on failure. */
static int	append_segment_to_buf(t_dyn_buf *buf, char *segment)
{
	if (append_str(buf, segment) != 0)
		return (-1);
	free(segment);
	return (0);
}

/* Finalize dynamic buffer into a TOKEN_WORD and append to list. Take ownership
   of buf->buf on success (buf struct freed, buf->buf moved into token->text). 
   Return 0 on success, -1 on failure. */
static int	finalize_token_from_buf(t_dyn_buf *buf, bool had_quote,
				t_token_list *list)
{
	t_token	*token;

	token = new_token(TOKEN_WORD, buf->buf);
	if (!token)
	{
        dynamic_buf_free(buf);
        return (-1);
	}
	token->was_quoted = had_quote;
	free(buf);
	if (append_token(list, token) != 0)
	{
        free(token->text);
        free(token);
        return (-1);
	}
	return (0);
}

/* Create a token out of an unquoted word or a quoted string. Coalesce adjacent 
   segments into one token (handle expansion as needed). Return 0 on success, 
   -1 on failure. */
int	create_token_quote_or_word(const char *line, size_t *i, t_token_list *list,
		t_shell *shell)
{
	t_dyn_buf	*buf;
	bool			had_quote;
	char			*segment;
	int				seg_ret;

	buf = dynbuf_create_and_init();
	if (!buf)
		return (-1);
	had_quote = false;
	while (line[*i] != '\0' && !isspace((unsigned char)line[*i]) 
		&& line[*i] != '|' && line[*i] != '<' && line[*i] != '>')
	{
		segment = NULL;
		seg_ret = parse_segment(line, i, &segment, shell);
		if (seg_ret == -1)
			return (dynamic_buf_free(buf), -1);
		if (append_segment_to_buf(buf, segment) != 0)
			return (dynamic_buf_free(buf), -1);
		if (seg_ret == 1)
			had_quote = true;
	}
	if (finalize_token_from_buf(buf, had_quote, list) != 0)
		return (-1);
	return (0);
}
