/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler_str.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 16:09:43 by acossari          #+#    #+#             */
/*   Updated: 2025/05/30 17:31:35 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/*
** handle_string: print a null-terminated string argument
** @fmt: pointer to t_format structure with flags, width, precision,
**       and specifier ('s')
** @ap:  pointer to the va_list of conversion arguments
** Return: number of characters printed, or -1 on write error
*/
int	handle_string(t_format *fmt, va_list *ap)
{
	char	*s;
	int		len;

	(void)fmt;
	s = va_arg(*ap, char *);
	if (!s)
		s = "(null)";
	len = ft_strlen(s);
	return (write(1, s, len));
}
