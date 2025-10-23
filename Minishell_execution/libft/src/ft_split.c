/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 18:52:15 by acossari          #+#    #+#             */
/*   Updated: 2025/05/15 10:52:41 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/*
** word_len: returns the number of characters until the next delimiter c
** @s: pointer to the start of the word
** @c: delimiter character
** Return: length of the word (count of non-delimiter characters)
*/
static size_t	word_len(const char *s, char c)
{
	size_t	len;

	len = 0;
	while (s[len] && s[len] != c)
		len++;
	return (len);
}

/*
** count_words: counts the number of words in s separated by delimiter c
** @s: null-terminated string to scan
** @c: delimiter character
** Return: number of words found
*/
static size_t	count_words(const char *s, char c)
{
	size_t	i;
	size_t	count;

	i = 0;
	count = 0;
	while (s[i])
	{
		while (s[i] && s[i] == c)
			i++;
		if (s[i] && s[i] != c)
		{
			count++;
			while (s[i] && s[i] != c)
				i++;
		}
	}
	return (count);
}

/*
** free_split: frees allocated words on error and the array itself
** @arr: array of strings to free
** @used: number of elements successfully allocated
** Return: NULL always
*/
static char	**free_split(char **arr, size_t used)
{
	size_t	i;

	i = 0;
	while (i < used)
	{
		free(arr[i]);
		i++;
	}
	free(arr);
	return (NULL);
}

/*
** ft_split: split a string into an array of substrings
** @s: the null-terminated string to split
** @c: delimiter character
** Return: array of substrings (must be freed),
**         terminated by NULL, or NULL on allocation failure
*/
char	**ft_split(char const *s, char c)
{
	size_t	i;
	size_t	w;
	size_t	words;
	char	**arr;

	if (!s)
		return (NULL);
	words = count_words(s, c);
	arr = malloc((words + 1) * sizeof(char *));
	if (!arr)
		return (NULL);
	i = 0;
	w = 0;
	while (w < words)
	{
		while (s[i] && s[i] == c)
			i++;
		arr[w] = ft_substr(s, i, word_len(s + i, c));
		if (arr[w] == NULL)
			return (free_split(arr, w));
		w++;
		i += word_len(s + i, c);
	}
	arr[w] = NULL;
	return (arr);
}
