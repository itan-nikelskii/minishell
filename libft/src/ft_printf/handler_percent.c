/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler_percent.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 16:09:58 by acossari          #+#    #+#             */
/*   Updated: 2025/05/30 17:31:23 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/*
** handle_percent: print a literal '%' character
** @fmt:  pointer to t_format structure (unused)
** @ap:   pointer to the va_list of conversion arguments (unused)
** Return: number of characters printed (1 on success, -1 on error)
*/
int	handle_percent(t_format *fmt, va_list *ap)
{
	(void)fmt;
	(void)ap;
	return (write(1, "%", 1));
}
