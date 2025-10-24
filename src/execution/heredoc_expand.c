/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_expand.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 18:59:01 by acossari          #+#    #+#             */
/*   Updated: 2025/10/22 21:34:37 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Expand $? to last exit status
 * @param j: Position pointer (updated to skip '?')
 * @param buf: Output buffer
 * @param shell: Shell state
 * @return: 0 on success, -1 on malloc failure
 */
static int	expand_exit_status(size_t *j, char **buf, t_shell *shell)
{
	char	*exp;

	exp = ft_itoa(shell->last_exit_status);
	if (!exp)
		return (-1);
	if (hd_append_str(buf, exp) != 0)
		return (free(exp), -1);
	free(exp);
	*j = *j + 2;
	return (0);
}

/**
 * Expand $VAR to environment variable value
 * @param str: Input string
 * @param j: Position pointer (updated to skip variable name)
 * @param buf: Output buffer
 * @param shell: Shell state
 * @return: 0 on success, -1 on malloc failure
 */
static int	expand_variable(const char *str, size_t *j, char **buf,
							t_shell *shell)
{
	char	*varname;
	char	*exp;
	size_t	i;

	i = *j + 1;
	varname = hd_extract_varname(str, i);
	if (!varname)
		return (-1);
	exp = hd_getenv(shell, varname);
	if (!exp)
		return (free(varname), -1);
	*j = i + ft_strlen(varname);
	free(varname);
	if (hd_append_str(buf, exp) != 0)
		return (free(exp), -1);
	free(exp);
	return (0);
}

/**
 * Handle $ expansion at position str[*j]
 * Expands $VAR and $?, keeps literal $ for invalid syntax
 * @param str: Input string
 * @param j: Position pointer (updated after expansion)
 * @param buf: Output buffer
 * @param shell: Shell state
 * @return: 0 on success, -1 on malloc failure
 */
static int	hd_expand_dollar(const char *str, size_t *j,
							char **buf, t_shell *shell)
{
	size_t	i;

	i = *j + 1;
	if (str[i] == '?')
		return (expand_exit_status(j, buf, shell));
	else if (ft_isalpha(str[i]) || str[i] == '_')
		return (expand_variable(str, j, buf, shell));
	else
	{
		if (hd_append_char(buf, '$') != 0)
			return (-1);
		*j = *j + 1;
		return (0);
	}
}

/*
** heredoc line expansion rules (general)
**
** Behavior:
** - If expand == false (quoted delimiter): return an exact copy of the line.
** - If expand == true  (unquoted delimiter): expand $-patterns as below.
**
** Patterns handled when expand == true:
** 1) "$?"                -> replaced with last exit status (as decimal)
**    ex:  "x$?y"         -> "x<status>y"
**
** 2) "$VAR"              -> replaced with the environment value of VAR
**    - Valid var name: [A-Za-z_][A-Za-z0-9_]*
**    - If VAR is unset: replaced with empty string
**    ex:  "hi $USER!"    -> "hi <USER_VALUE>!"
**         "$_A1x"        -> "<_A1_VALUE>x"
**         "$MISSING."    -> "."
**
** 3) Invalid after '$'   -> '$' kept literal, next char processed normally
**    (first char is NOT letter/_ or end-of-line)
**    ex:  "$1x"          -> "$1x"
**         "$@"           -> "$@"
**         "$"            -> "$"
**
** 4) Quotes inside the line are literal (no quote-removal in heredoc)
**    ex:  "say \"$USER\"" -> 'say "<USER_VALUE>"'   (if expand == true)
**         "say \"$USER\"" -> 'say "$USER"'          (if expand == false)
**
** 5) Multiple expansions are allowed in the same line
**    ex:  "$USER-$?"     -> "<USER_VALUE>-<status>"
**
** Notes:
** - Index movement is handled internally: "$?" consumes 2 chars, "$VAR"
**   consumes the whole var name, invalid cases consume only '$'.
*/

/**
 * Expand variables in heredoc line
 * Expands $VAR and $? if expand flag is true
 * Quotes are treated as literal characters (no quote processing in heredoc)
 * @param line: Input line from heredoc
 * @param shell: Shell state
 * @param expand: If true, expand variables; if false, return copy
 * @return: Allocated expanded string or NULL on error
 */
char	*hd_expand_line(const char *line, t_shell *shell, bool expand)
{
	char	*result;
	size_t	i;

	if (!expand)
		return (ft_strdup(line));
	result = ft_strdup("");
	if (!result)
		return (NULL);
	i = 0;
	while (line[i])
	{
		if (line[i] == '$')
		{
			if (hd_expand_dollar(line, &i, &result, shell) != 0)
				return (free(result), NULL);
		}
		else
		{
			if (hd_append_char(&result, line[i]) != 0)
				return (free(result), NULL);
			i++;
		}
	}
	return (result);
}
