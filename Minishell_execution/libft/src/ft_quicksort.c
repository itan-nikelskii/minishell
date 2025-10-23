/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_quicksort.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/09 12:20:00 by acossari          #+#    #+#             */
/*   Updated: 2025/10/02 23:30:08 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/** 
 * Swap two integers.
 *
 * @param a Pointer to the first integer.
 * @param b Pointer to the second integer.
 */
static void	ft_swap_int(int *a, int *b)
{
	int	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

/** 
 * Partition function for quicksort.
 * It selects a pivot and partitions the array around the pivot.
 *
 * @param arr Pointer to the integer array.
 * @param low Starting index of the array segment to be partitioned.
 * @param high Ending index of the array segment to be partitioned.
 * @return The index of the pivot after partitioning.
 */
static int	partition(int arr[], int low, int high)
{
	int	pivot;
	int	i;
	int	j;
	int	mid;

	mid = low + (high - low) / 2;
	pivot = arr[mid];
	i = low - 1;
	j = high + 1;
	while (1)
	{
		i++;
		while (arr[i] < pivot)
			i++;
		j--;
		while (arr[j] > pivot)
			j--;
		if (i >= j)
			return (j);
		ft_swap_int(&arr[i], &arr[j]);
	}
}

/** 
 * Recursive quicksort function (Hoare's partitioning scheme).
 * 
 * @param arr Pointer to the integer array.
 * @param lo Lower index of the array segment to be sorted.
 * @param hi Higher index of the array segment to be sorted.
 */
static void	quicksort(int *arr, int lo, int hi)
{
	int	pivot;

	if (lo < hi)
	{
		pivot = partition(arr, lo, hi);
		quicksort(arr, lo, pivot);
		quicksort(arr, pivot + 1, hi);
	}
}

/**
 * Sorts an integer array in ascending order using the quicksort algorithm.
 * This is a general-purpose sorting function for integer arrays.
 * Time complexity: O(n log n) average case, O(n²) worst case.
 * Space complexity: O(log n) due to recursion stack.
 * 
 * @param arr Pointer to the integer array to be sorted in-place
 * @param n Number of elements in the array (must be > 0)
 */
void	ft_quicksort(int *arr, int n)
{
	if (!arr || n <= 1)
		return ;
	quicksort(arr, 0, n - 1);
}
