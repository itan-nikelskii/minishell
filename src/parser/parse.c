/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 11:42:23 by inikelsk          #+#    #+#             */
/*   Updated: 2025/10/30 14:58:18 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parser.h"

/* Append cmd to the command list. Caller provides pointers to head and tail. */
static void	append_command(t_command **head, t_command **tail, t_command *cmd)
{
	if (*head == NULL)
	{
		*head = cmd;
		*tail = cmd;
	}
	else
	{
		(*tail)->next = cmd;
		*tail = cmd;
	}
}

/* Parse token list into pipeline (linked commands). Set incomplete_pipe = true
   if the token list ends with a single trailing pipe (execution layer checks
   the flag to get more input). */
static t_command	*parse_tokens_to_commands(t_token *t, char **error)
{
	t_command	*head;
	t_command	*tail;
	t_command	*cmd;

	head = NULL;
	tail = NULL;
	while (t != NULL)
	{
		if (t->type == TOKEN_PIPE)
		{
			t = t->next;
			continue ;
		}
		cmd = build_command_from_tokens(&t, error);
		if (!cmd)
			return (NULL);
		append_command(&head, &tail, cmd);
		if (t != NULL && t->type == TOKEN_PIPE)
			t = t->next;
	}
	return (head);
}

/* Handle an error for parse(): set result->error using tok_err if present,
   otherwise set fallback if result->error is not already set. Free tokens
   if provided and return 0 (parse() uses this return value). */
static int	handle_parse_error(t_parse_result *result, char *tok_err,
                const char *fallback, t_token *tokens)
{
	if (tok_err)
		result->error = tok_err;
	else if (!result->error && fallback != NULL)
		result->error = ft_strdup(fallback);
	if (tokens)
		free_tokens(tokens);
	return (0);
}

/* Top-level parse entrypoint (called from main). Create tokens, resolve pipes,
   making tokens into commands; throw appropriate errors if something fails. */
int	parse(const char *line, t_parse_result *result, t_shell *shell)
{
	t_token		*tokens;
	char		*tok_err;

	if (!result)
		return (-1);
	tokens = NULL;
	tok_err = NULL;
	tokens = tokenize(line, &tok_err, shell);
	if (!tokens)
		return (handle_parse_error(result, tok_err, NULL, NULL));
	if (deal_with_pipes(&tokens, &tok_err, &result->incomplete_pipe) != 0)
		return (handle_parse_error(result, tok_err, "Syntax error in pipe usage", tokens));
	result->commands = parse_tokens_to_commands(tokens, &tok_err);
	if (!result->commands && !result->incomplete_pipe)
		return (handle_parse_error(result, tok_err, "Syntax error building commands", tokens));
	free_tokens(tokens);
	return (0);
}
