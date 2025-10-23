/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler_hex.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 16:01:08 by acossari          #+#    #+#             */
/*   Updated: 2025/05/30 17:31:13 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>
/*
** handle_hex: convert an unsigned integer to hexadecimal and print it
** @fmt:  pointer to t_format structure with flags, width, precision,
**        and specifier ('x' or 'X')
** @ap:   pointer to the va_list of conversion arguments
** Return: number of characters printed, or 0 if allocation fails
*/
int	handle_hex(t_format *fmt, va_list *ap)
{
	unsigned int	n;
	char			*s;
	int				len;
	int				printed;

	(void)fmt;
	n = va_arg(*ap, unsigned int);
	s = ft_utoa_base(n, 16, fmt->specifier);
	if (!s)
		return (0);
	len = ft_strlen(s);
	printed = write(1, s, len);
	free(s);
	return (printed);
}
