/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler_char.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 16:09:13 by acossari          #+#    #+#             */
/*   Updated: 2025/05/30 17:31:07 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/*
** handle_char: print a single character argument
** @fmt:  pointer to t_format with flags, width, precision, specifier
** @ap:   pointer to the va_list of conversion arguments
** Return: number of characters printed (1 on success, -1 on error)
*/
int	handle_char(t_format *fmt, va_list *ap)
{
	char	c;

	(void)fmt;
	c = (char)va_arg(*ap, int);
	return (write(1, &c, 1));
}
