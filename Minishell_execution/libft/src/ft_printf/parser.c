/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 16:08:35 by acossari          #+#    #+#             */
/*   Updated: 2025/05/30 17:32:35 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/*
** init_format: reset a t_format structure to default values
** @fmt: pointer to the t_format structure to initialize
*/
static void	init_format(t_format *fmt)
{
	int	i;

	i = 0;
	while (i < 6)
		fmt->flags[i++] = '\0';
	fmt->width = 0;
	fmt->precision = -1;
	fmt->specifier = '\0';
}

/*
** parse_format: process the format string and dispatch conversions
** @format: null-terminated format string with plain text and specifiers
** @ap:      pointer to the va_list of arguments
** Return: total count of characters written.
*/
int	parse_format(const char *format, va_list *ap)
{
	t_format	fmt;
	int			printed;
	int			i;
	int			res;

	printed = 0;
	i = 0;
	while (format[i])
	{
		if (format[i] == '%' && format[i + 1])
		{
			i++;
			init_format(&fmt);
			fmt.specifier = format[i];
			res = dispatch(&fmt, ap);
			printed += res;
		}
		else
			printed += write(1, &format[i], 1);
		i++;
	}
	return (printed);
}
