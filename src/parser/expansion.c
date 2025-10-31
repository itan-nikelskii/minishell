/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 14:08:46 by inikelsk          #+#    #+#             */
/*   Updated: 2025/10/31 13:23:31 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parser.h"

/* Expand the variable and return the result on success or NULL on failure. */
static char	*expand_variable(const char *s, size_t idx, size_t *j, char **envp)
{
	size_t	name_start;
	char	*name;
	char	*value;
	char	*result;

	name_start = idx;
	while (isalnum((unsigned char)s[idx]) || s[idx] == '_')
		idx++;
	name = strdup_range(s, name_start, idx);
	if (!name)
		return (NULL);
	value = get_env_value(name, envp);
	if (value)
		result = ft_strdup(value);
	else
		result = ft_strdup("");
	free(name);
	*j = idx;
	return (result);
}

static char	*get_expanded_value(const char *s, size_t *j, t_shell *shell)
{
	size_t	index;
	char	*expanded;

	index = *j + 1;
	if (s[index] == '?')
	{
		expanded = get_exit_status_str(shell);
		if (!expanded)
			return (NULL);
		*j = index + 1;
	}
	else if (ft_isalpha((unsigned char)s[index]) || s[index] == '_')
	{
		expanded = expand_variable(s, index, j, shell->envp);
		if (!expanded)
			return (NULL);
	}
	else
		return (NULL);
	return (expanded);
}

/* TODO: documentation (after proper refactoring for norminette) */
int	expand_dollar(const char *s, size_t *j, t_dyn_buf *buf, t_shell *shell)
{
	char	*expanded;

	expanded = get_expanded_value(s, j, shell);
	if (expanded)
	{
		if (append_str(buf, expanded) != 0)
		{
			free(expanded);
			return (-1);
		}
		free(expanded);
		return (0);
	}
	else
	{
		if (append_char(buf, '$') != 0)
			return (-1);
		*j += 1;
		return (0);
	}
}
