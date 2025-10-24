/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dispatch.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 16:08:55 by acossari          #+#    #+#             */
/*   Updated: 2025/05/30 17:30:52 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/*
** dispatch: select and invoke the appropriate handler for a conversion specifier
** @fmt:  pointer to t_format containing
**        flags, width, precision, and specifier
** @ap:   pointer to the va_list of conversion arguments
** Return: number of characters printed, or 0 if specifier is invalid
*/
int	dispatch(t_format *fmt, va_list *ap)
{
	if (fmt->specifier == 'c')
		return (handle_char(fmt, ap));
	if (fmt->specifier == 's')
		return (handle_string(fmt, ap));
	if (fmt->specifier == 'p')
		return (handle_ptr(fmt, ap));
	if (fmt->specifier == 'd' || fmt->specifier == 'i')
		return (handle_int(fmt, ap));
	if (fmt->specifier == 'u')
		return (handle_unsigned(fmt, ap));
	if (fmt->specifier == 'x' || fmt->specifier == 'X')
		return (handle_hex(fmt, ap));
	if (fmt->specifier == '%')
		return (handle_percent(fmt, ap));
	return (0);
}
