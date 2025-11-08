/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 09:24:37 by inikelsk          #+#    #+#             */
/*   Updated: 2025/11/08 22:03:20 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser.h"

/* Create a pipe token and return 0 on success or -1 on malloc failure. */
static int	handle_pipe(size_t *i, t_tok_context *ctx)
{
	int	status;

	status = create_token_pipe(i, &ctx->list);
	if (status != 0)
	{
		*(ctx->error) = ft_strdup("malloc failure");
		free_tokens(ctx->list.head);
		return (-1);
	}
	return (0);
}

/* Create a redirection token and return 0 on success or -1 on failure (FIX 52)
 * (FIX 52: simplified error handling - direct index from
 * create_token_redirection) */
static int	handle_redir(const char *line, size_t *i, t_tok_context *ctx)
{
	int		ret_value;

	ret_value = create_token_redirection(line, i, &ctx->list);
	if (ret_value != 0)
	{
		if (ret_value == -1)
			*(ctx->error) = ft_strdup("malloc failure");
		else
			*(ctx->error) = build_redir_error(line, ret_value - 1);
		free_tokens(ctx->list.head);
		return (-1);
	}
	return (0);
}

/* Create a word token, differentiate between that word being part of a command
   or a HEREDOC delimiter. Return 0 on success or -1 on failure. */
static int	handle_word(const char *line, size_t *i, t_tok_context *ctx)
{
	int	status;

	if (ctx->list.tail && ctx->list.tail->type == TOKEN_HEREDOC)
		status = create_token_quote_or_word(line, i, &ctx->list, NULL);
	else
		status = create_token_quote_or_word(line, i, &ctx->list, ctx->shell);
	if (status != 0)
	{
		*(ctx->error) = ft_strdup("parse error in word/quote");
		free_tokens(ctx->list.head);
		return (-1);
	}
	return (0);
}

/* Handle a valid token at line[*i]: pipe, redirection or word/quote. Returns:
 *  0: handled successfully (handler advanced *i)
 *  1: not a handled token (caller should advance i)
 * -1: error (handler set *error and freed tokens) */
static int	handle_valid_token(const char *line, size_t *i, t_tok_context *ctx)
{
	int	status;

	if (line[*i] == '|')
	{
		status = handle_pipe(i, ctx);
		if (status != 0)
			return (-1);
		return (0);
	}
	if (line[*i] == '<' || line[*i] == '>')
	{
		status = handle_redir(line, i, ctx);
		if (status != 0)
			return (-1);
		return (0);
	}
	if (line[*i] == '\'' || line[*i] == '"' || is_word_char(line[*i]))
	{
		status = handle_word(line, i, ctx);
		if (status != 0)
			return (-1);
		return (0);
	}
	return (1);
}

/* Tokenize input line into linked token list. */
t_token	*tokenize(const char *line, char **error, t_shell *shell)
{
	t_tok_context	ctx;
	size_t			i;
	int				status;

	init_tok_context(&ctx, shell, error, &i);
	while (line[i] != '\0')
	{
		if (ft_isspace((unsigned char)line[i]))
		{
			i++;
			continue ;
		}
		if (line[i] == ';')
		{
			*error = ft_strdup("Unsupported special character: ;");
			return (free_tokens(ctx.list.head), NULL);
		}
		status = handle_valid_token(line, &i, &ctx);
		if (status == -1)
			return (NULL);
		if (status == 0)
			continue ;
		i++;
	}
	return (ctx.list.head);
}
