/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler_ptr.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 16:01:29 by acossari          #+#    #+#             */
/*   Updated: 2025/05/30 17:31:30 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>
/*
** handle_ptr: convert a pointer to hexadecimal notation and print it
** @fmt:  pointer to t_format structure with flags, width, precision,
**        and specifier ('p')
** @ap:   pointer to the va_list of conversion arguments
** Return: number of characters printed, or 0 if allocation fails
*/
int	handle_ptr(t_format *fmt, va_list *ap)
{
	void	*ptr;
	char	*s;
	int		len;
	int		printed;

	(void)fmt;
	ptr = va_arg(*ap, void *);
	if (!ptr)
		return (write(1, "(nil)", 5));
	s = ft_utoa_base((unsigned long)ptr, 16, 'x');
	if (!s)
		return (0);
	printed = write(1, "0x", 2);
	len = ft_strlen(s);
	printed += write(1, s, len);
	free(s);
	return (printed);
}
