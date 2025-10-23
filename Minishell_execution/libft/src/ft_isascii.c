/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isascii.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 16:09:00 by acossari          #+#    #+#             */
/*   Updated: 2025/05/13 13:41:50 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/*
** ft_isascii: check if c is a 7-bit ASCII character
** @c: the character to test
** Return: 1 if in range 0–127, 0 otherwise
*/
int	ft_isascii(int c)
{
	return (c >= 0 && c <= 127);
}
