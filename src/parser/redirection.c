/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 14:14:24 by inikelsk          #+#    #+#             */
/*   Updated: 2025/11/11 09:32:41 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser.h"

/* Check for invalid redirection sequences:
 * more than two redir signs in a row;
 * <>, ><, <|;
 * any valid redir sign followed by space(s) and another redir sign.
Return values: -1 = valid, >= 0 = index of problematic token. */
static int	is_invalid_redir_sequence(const char *line, size_t i)
{
	size_t	j;
	int		count;
	char	this;
	char	next;

	j = i;
	count = 0;
	this = line[i];
	next = line[i + 1];
	if (this == '<' && next == '|')
		return (i);
	while (line[j] == '<' || line[j] == '>')
	{
		count++;
		j++;
	}
	if (count > 2)
		return (i + 2);
	if ((this == '<' && next == '>') || (this == '>' && next == '<'))
		return (i + 1);
	while (ft_isspace(line[j]))
		j++;
	if (line[j] == '<' || line[j] == '>')
		return (j);
	return (-1);
}

/* Advance the index *i by 1 or 2 depending on the redirection type. */
static void	advance_redir_index(const char *line, size_t *i)
{
	char	next;

	next = line[*i + 1];
	if (next && (next == '<' || next == '>' || next == '|'))
		*i += 2;
	else
		*i += 1;
}

/* Handle the redirection (<, <<, >, >>, >|) chars: determine the redirection
type, create a new token, append it to the token list, and update *i depending
on the number of chars in the redirection type (1 or 2). Return 0 on success, 
-1 on malloc fail, or (error_index + 1) for syntax error. */
int	create_token_redirection(const char *line, size_t *i, t_token_list *list)
{
	t_token	*token;
	int		err_idx;

	err_idx = is_invalid_redir_sequence(line, *i);
	if (err_idx >= 0)
		return (err_idx + 1);
	if (line[*i] == '<')
	{
		if (line[*i + 1] && line[*i + 1] == '<')
			token = new_token(TOKEN_HEREDOC, ft_strdup("<<"));
		else
			token = new_token(TOKEN_REDIR_IN, ft_strdup("<"));
	}
	else
	{
		if (line[*i + 1] && line[*i + 1] == '>')
			token = new_token(TOKEN_REDIR_APPEND, ft_strdup(">>"));
		else if (line[*i + 1] && line[*i + 1] == '|')
			token = new_token(TOKEN_REDIR_OUT, ft_strdup(">|"));
		else
			token = new_token(TOKEN_REDIR_OUT, ft_strdup(">"));
	}
	if (!token || append_token(list, token) != 0)
		return (advance_redir_index(line, i), -1);
	return (advance_redir_index(line, i), 0);
}

/* Consume the redirection target token and attach to cmd.redirs. On success,
   return 0. If the target doesn't exist or is not a TOKEN_WORD type, or if
   anything goes wrong with mallocs, return -1. */
int	consume_redirection_target(t_token *token, t_command *cmd)
{
	t_token_type	redir_type;
	char			*target;
	t_redir			*redir;
	t_redir			*last;

	if (!token || !token->next)
		return (-1);
	redir_type = token->type;
	token = token->next;
	target = ft_strdup(token->text);
	if (!target || token->type != TOKEN_WORD)
		return (-1);
	redir = new_redir(redir_type, target, token->was_quoted);
	if (!redir)
		return (free(target), -1);
	if (!cmd->redirs)
		cmd->redirs = redir;
	else
	{
		last = cmd->redirs;
		while (last->next)
			last = last->next;
		last->next = redir;
	}
	return (0);
}
