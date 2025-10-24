/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bzero.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 16:08:27 by acossari          #+#    #+#             */
/*   Updated: 2025/05/13 19:49:20 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/*
** ft_bzero: erase n bytes of a memory area by writing NUL bytes
** @s: pointer to the memory area to clear
** @n: number of bytes to set to zero
** Return: none
*/
void	ft_bzero(void *s, size_t n)
{
	ft_memset(s, 0, n);
}
