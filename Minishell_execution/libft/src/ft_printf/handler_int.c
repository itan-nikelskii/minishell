/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler_int.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 16:00:13 by acossari          #+#    #+#             */
/*   Updated: 2025/05/30 17:31:18 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>
/*
** handle_int: convert a signed integer to string and print it
** @fmt:  pointer to t_format structure with flags, width, precision,
**        and specifier ('d' or 'i')
** @ap:   pointer to the va_list of conversion arguments
** Return: number of characters printed, or 0 if allocation fails
*/
int	handle_int(t_format *fmt, va_list *ap)
{
	char	*s;
	int		len;
	int		printed;

	(void)fmt;
	s = ft_itoa(va_arg(*ap, int));
	if (!s)
		return (0);
	len = ft_strlen(s);
	printed = write(1, s, len);
	free(s);
	return (printed);
}
