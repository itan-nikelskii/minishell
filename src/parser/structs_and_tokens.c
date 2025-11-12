/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structs_and_tokens.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 14:49:21 by inikelsk          #+#    #+#             */
/*   Updated: 2025/11/12 10:31:13 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser.h"

/* Create and return a new token node. */
t_token	*new_token(t_token_type type, char *text)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	token->text = text;
	token->was_quoted = false;
	token->next = NULL;
	return (token);
}

/* Append token to the linked list. */
int	append_token(t_token_list *list, t_token *node)
{
	if (!node)
		return (-1);
	node->next = NULL;
	if (list->head == NULL)
	{
		list->head = node;
		list->tail = node;
	}
	else
	{
		list->tail->next = node;
		list->tail = node;
	}
	return (0);
}

/* Create and return a new redirection node. */
t_redir	*new_redir(t_token_type type, char *target, bool was_quoted)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->target = target;
	redir->was_quoted = was_quoted;
	redir->hd_path = NULL;
	redir->next = NULL;
	return (redir);
}

/* Handle the '|' char: create a new pipe token, append it to the token list,
   and return 0 on success or -1 on failure. */
int	create_token_pipe(size_t *i, t_token_list *list)
{
	t_token	*token;

	token = new_token(TOKEN_PIPE, ft_strdup("|"));
	if (!token || append_token(list, token) != 0)
		return (-1);
	*i = *i + 1;
	return (0);
}

/* Initialize tokenizer context and index (to keep tokenie() under 25 lines):
 * ctx	: pointer to tokenizer context to initialize
 * shell: shell pointer to store in ctx
 * error: pointer-to-error (stored in ctx)
 * i	: pointer to size_t index (set to 0). */
void	init_tok_context(t_tok_context *ctx, t_shell *shell, char **error,
			size_t *i)
{
	ctx->list.head = NULL;
	ctx->list.tail = NULL;
	ctx->shell = shell;
	ctx->error = error;
	*i = 0;
}
