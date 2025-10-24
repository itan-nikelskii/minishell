/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putchar_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 19:24:57 by acossari          #+#    #+#             */
/*   Updated: 2025/05/15 12:55:32 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/*
** ft_putchar_fd: write a character to a given file descriptor
** @c:  the character to write
** @fd: the file descriptor on which to write
** Return: none
*/
void	ft_putchar_fd(char c, int fd)
{
	write(fd, &c, 1);
}
