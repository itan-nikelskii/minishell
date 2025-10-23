/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 11:38:57 by acossari          #+#    #+#             */
/*   Updated: 2025/10/16 18:22:48 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include <limits.h>

/**
 * Parse sign and return starting index
 * @param str string to parse
 * @param sign Pointer to sign variable (-1 or 1)
 * @return Index after sign character(s)
 */
static int	parse_sign(const char *str, int *sign)
{
	int	i;

	i = 0;
	*sign = 1;
	if (str[i] == '+' || str[i] == '-')
	{
		if (!ft_isdigit(str[i + 1]))
			return (-1);
		if (str[i] == '-')
			*sign = -1;
		i++;
	}
	return (i);
}

/**
 * Validate exit argument and parse to long long
 * @param str string to validate
 * @param out Pointer to store parsed value
 * @return 1 if valid, 0 if invalid or overflow
 */
int	is_valid_exit_arg(const char *str, long long *out)
{
	int			sign;
	int			i;
	int			digit;
	long long	acc;

	if (!str || !*str)
		return (0);
	i = parse_sign(str, &sign);
	if (i == -1)
		return (0);
	acc = 0;
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		digit = str[i] - '0';
		if (acc > (LLONG_MAX - digit) / 10)
			return (0);
		acc = acc * 10 + digit;
		i++;
	}
	*out = acc * sign;
	return (1);
}
