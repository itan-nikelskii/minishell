/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_parser.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 13:32:59 by inikelsk          #+#    #+#             */
/*   Updated: 2025/10/30 17:08:25 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parser.h"

/* Free tokens. */
void	free_tokens(t_token *token)
{
	t_token	*next_token;

	while (token)
	{
		next_token = token->next;
		free(token->text);
		free(token);
		token = next_token;
	}
}

/* Free a NULL-terminated argv array. */
static void	free_argv(char **argv)
{
	size_t	i;

	if (!argv)
		return ;
	i = 0;
	while (argv[i])
	{
		free(argv[i]);
		i++;
	}
	free(argv);
}

/* Free a linked list of redirections. */
static void	free_redirs(t_redir *redir)
{
	t_redir	*next_redir;

	while (redir)
	{
		next_redir = redir->next;
		free(redir->target);
		if (redir->hd_path)
			free(redir->hd_path);
		free(redir);
		redir = next_redir;
	}
}

/* Free commands and their redirs/argv. */
void	free_commands(t_command *cmd)
{
	t_command	*next_command;

	while (cmd)
	{
		next_command = cmd->next;
		free_argv(cmd->argv);
		free_redirs(cmd->redirs);
		free(cmd);
		cmd = next_command;
	}
}

/* Free the dynamic buffer struct. */
void dynamic_buf_free(t_dyn_buf *dynamic_buf)
{
	free(dynamic_buf->buf);
	free(dynamic_buf);
}
