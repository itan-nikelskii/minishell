/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 16:08:07 by acossari          #+#    #+#             */
/*   Updated: 2025/05/30 17:48:10 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/*
** ft_printf: formatted output to standard output
** @format: format string containing plain text and conversion specifiers
** @...   : arguments corresponding to the conversion specifiers in format
** Return: total number of characters printed, or a negative value on error
*/
int	ft_printf(const char *format, ...)
{
	va_list	ap;
	int		printed;

	if (!format)
		return (-1);
	va_start(ap, format);
	printed = parse_format(format, &ap);
	va_end(ap);
	return (printed);
}
