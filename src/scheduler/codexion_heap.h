/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion_heap.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 10:48:19 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/21 10:48:21 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_HEAP_H
# define CODEXION_HEAP_H

# include <stddef.h>
# include "parser/codexion_parse.h"

typedef struct s_request
{
	int					coder_id;
	unsigned long long	sequence;
	t_ms				deadline;
}	t_request;

typedef struct s_heap
{
	t_request	*items;
	size_t		size;
	size_t		capacity;
	t_policy	policy;
}	t_heap;

int				cx_heap_init(t_heap *heap, size_t capacity, t_policy policy);
void			cx_heap_destroy(t_heap *heap);
int				cx_heap_push(t_heap *heap, t_request request);
int				cx_heap_pop(t_heap *heap, t_request *request);
const t_request	*cx_heap_peek(const t_heap *heap);
int				cx_heap_remove(t_heap *heap, int coder_id);

#endif
