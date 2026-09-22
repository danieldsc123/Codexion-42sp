/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/22 16:58:21 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/22 17:01:47 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	cx_heap_init(t_heap *heap, size_t capacity, t_policy policy)
{
	heap->items = malloc(sizeof(t_request) * capacity);
	if (!heap->items)
		return (-1);
	heap->size = 0;
	heap->capacity = capacity;
	heap->policy = policy;
	return (0);
}

void	cx_heap_destroy(t_heap *heap)
{
	free(heap->items);
	heap->items = NULL;
	heap->size = 0;
	heap->capacity = 0;
}
