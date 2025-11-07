/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 14:14:24 by inikelsk          #+#    #+#             */
/*   Updated: 2025/11/08 00:13:08 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser.h"

/* Check for invalid redirection sequences:
 * more than two redir signs in a row;
 * <>, ><, <|;
 * any valid redir sign followed by space(s) and another redir sign.
Return the problematic character if found, '\0' otherwise. (FIX 48) */
static char	is_invalid_redir_sequence(const char *line, size_t i)
{
	size_t	j;
	int		count;
	char	this;
	char	next;

	j = i;
	count = 0;
	this = line[i];
	next = line[i + 1];
	if (this == '<' && next == '|')	// FIX 43: reject <| (invalid in bash)
		return ('|');	// FIX 43: return '|' as problematic character
	while (line[j] == '<' || line[j] == '>')
	{
		count++;
		j++;
	}
	if (count > 2)
		return (line[i + 2]);	// FIX 48: return specific char instead of bool
	if ((this == '<' && next == '>') || (this == '>' && next == '<'))
		return (next);	// FIX 48: return specific char instead of bool
	while (ft_isspace(line[j]))
		j++;
	if (line[j] == '<' || line[j] == '>')
		return (line[j]);	// FIX 48: return specific char instead of bool
	return ('\0');	// FIX 48: return '\0' instead of false
}

/* Handle the redirection (<, <<, >, >>, >|) chars: determine the redirection
type, create a new token, append it to the token list, and update *i depending
on the number of chars in the redirection type (1 or 2).
Return 0 on success, -1 on malloc failure, or -(256 + char) on syntax error.
(FIX 48) */
int	create_token_redirection(const char *line, size_t *i, t_token_list *list)
{
	t_token	*token;
	char	bad_char;	// FIX 48: store specific problematic character

	bad_char = is_invalid_redir_sequence(line, *i);	// FIX 48
	if (bad_char)	// FIX 48: check if invalid char found
		return (-(256 + (unsigned char)bad_char));	// FIX 48: encode char in return
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
		else if (line[*i + 1] && line[*i + 1] == '|')	// FIX 43: recognize >|
			token = new_token(TOKEN_REDIR_OUT, ft_strdup(">|"));	// FIX 43: clobber
		else
			token = new_token(TOKEN_REDIR_OUT, ft_strdup(">"));
	}
	if (line[*i + 1] && (line[*i + 1] == '<' || line[*i + 1] == '>'
			|| line[*i + 1] == '|'))	// FIX 43: include '|' for >| operator
		*i += 2;	// FIX 43: advance 2 chars for >| (and <<, >>)
	else
		*i += 1;
	if (!token || append_token(list, token) != 0)
		return (-1);
	return (0);
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
