/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler_unsigned.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 16:00:43 by acossari          #+#    #+#             */
/*   Updated: 2025/05/30 17:31:42 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>
/*
** handle_unsigned: convert an unsigned integer to string and print it
** @fmt:  pointer to t_format structure with flags, width, precision,
**        and specifier ('u')
** @ap:   pointer to the va_list of conversion arguments
** Return: number of characters printed, or 0 if allocation fails
*/
int	handle_unsigned(t_format *fmt, va_list *ap)
{
	char	*s;
	int		len;
	int		printed;

	(void)fmt;
	s = ft_utoa(va_arg(*ap, unsigned int));
	if (!s)
		return (0);
	len = ft_strlen(s);
	printed = write(1, s, len);
	free(s);
	return (printed);
}
