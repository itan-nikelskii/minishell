/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 14:21:34 by antoniocoss       #+#    #+#             */
/*   Updated: 2025/10/27 14:27:43 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Read input line based on interactive mode (FIX ISSUE 8 - Issue C)
 * @param shell: Shell state (to check interactive flag)
 * @return Line read from stdin, or NULL on EOF
 *
 * Uses readline() in interactive mode (with prompt + history)
 * Uses ft_get_next_line() in non-interactive (no echo, no prompt)
 */
char	*read_input_line(t_shell *shell)
{
	char	*line;
	size_t	len;

	if (shell->interactive)
		return (readline("minishell$ "));
	line = ft_get_next_line(STDIN_FILENO);
	if (!line)
		return (NULL);
	len = ft_strlen(line);
	if (len > 0 && line[len - 1] == '\n')
		line[len - 1] = '\0';
	return (line);
}

/**
 * Handle parse errors (FIX ISSUE 7: set exit status to 2)
 * @param result: Parse result with error message
 * @param line: Input line to free
 * @param shell: Shell state (for setting exit status)
 */
void	handle_parse_error(t_parse_result *result, char *line, t_shell *shell)
{
	if (result->error)
	{
		print_error(NULL, result->error);
		free(result->error);
	}
	shell->last_exit_status = 2;
	free(line);
}
