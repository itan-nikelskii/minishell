/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 19:34:30 by acossari          #+#    #+#             */
/*   Updated: 2025/05/15 15:22:41 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/*
** ft_putnbr_fd: write an integer as ASCII to a given file descriptor
** @n:  the integer to write
** @fd: the file descriptor on which to write
** Return: none
**
** Recursively handles negative values by writing '-' then digits.
*/
void	ft_putnbr_fd(int n, int fd)
{
	long	nbr;

	nbr = n;
	if (nbr < 0)
	{
		ft_putchar_fd('-', fd);
		nbr = -nbr;
	}
	if (nbr >= 10)
		ft_putnbr_fd((int)(nbr / 10), fd);
	ft_putchar_fd((char)((nbr % 10) + '0'), fd);
}
