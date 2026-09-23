/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_order.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 10:56:56 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/22 16:15:10 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	cx_request_before(const t_request *a, const t_request *b,
	t_policy policy)
{
	if (policy == CX_EDF && a->deadline != b->deadline)
		return (a->deadline < b->deadline);
	if (a->sequence != b->sequence)
		return (a->sequence < b->sequence);
	return (a->coder_id < b->coder_id);
}

static void	cx_heap_swap(t_request *a, t_request *b)
{
	t_request	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

void	cx_heap_up(t_heap *heap, size_t index)
{
	size_t	parent;

	while (index)
	{
		parent = (index - 1) / 2;
		if (!cx_request_before(&heap->items[index], &heap->items[parent],
				heap->policy))
			break ;
		cx_heap_swap(&heap->items[index], &heap->items[parent]);
		index = parent;
	}
}

void	cx_heap_down(t_heap *heap, size_t index)
{
	size_t	child;

	while (index < heap->size / 2)
	{
		child = index * 2 + 1;
		if (child + 1 < heap->size && cx_request_before(&heap->items[child + 1],
				&heap->items[child], heap->policy))
			child++;
		if (!cx_request_before(&heap->items[child], &heap->items[index],
				heap->policy))
			break ;
		cx_heap_swap(&heap->items[index], &heap->items[child]);
		index = child;
	}
}

int	cx_heap_remove(t_heap *heap, int coder_id)
{
	size_t	index;

	index = 0;
	while (index < heap->size && heap->items[index].coder_id != coder_id)
		index++;
	if (index == heap->size)
		return (-1);
	heap->size--;
	if (index < heap->size)
	{
		heap->items[index] = heap->items[heap->size];
		if (index && cx_request_before(&heap->items[index],
				&heap->items[(index - 1) / 2], heap->policy))
			cx_heap_up(heap, index);
		else
			cx_heap_down(heap, index);
	}
	return (0);
}
