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
	heap->items = NULL;
	heap->size = 0;
	heap->capacity = 0;
	heap->policy = policy;
	if (!capacity || capacity > SIZE_MAX / sizeof(*heap->items))
		return (-1);
	if (policy != CX_FIFO && policy != CX_EDF)
		return (-1);
	heap->items = malloc(sizeof(*heap->items) * capacity);
	if (!heap->items)
		return (-1);
	heap->capacity = capacity;
	return (0);
}

void	cx_heap_destroy(t_heap *heap)
{
	free(heap->items);
	heap->items = NULL;
	heap->size = 0;
	heap->capacity = 0;
}

const t_request	*cx_heap_peek(const t_heap *heap)
{
	if (!heap->size)
		return (NULL);
	return (&heap->items[0]);
}

int	cx_heap_push(t_heap *heap, t_request request)
{
	if (heap->size == heap->capacity)
		return (-1);
	heap->items[heap->size] = request;
	heap->size++;
	cx_heap_up(heap, heap->size - 1);
	return (0);
}

int	cx_heap_pop(t_heap *heap, t_request *request)
{
	if (!heap->size)
		return (-1);
	*request = heap->items[0];
	heap->size--;
	if (heap->size)
	{
		heap->items[0] = heap->items[heap->size];
		cx_heap_down(heap, 0);
	}
	return (0);
}
