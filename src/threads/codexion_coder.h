/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion_coder.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 10:48:40 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/21 10:48:42 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_CODER_H
# define CODEXION_CODER_H

# include <pthread.h>
# include "scheduler/codexion_heap.h"

struct	s_sim;

typedef struct s_coder
{
	int				id;
	int				left_index;
	int				right_index;
	pthread_t		thread;
	struct s_sim	*sim;
	t_ms			last_compile_start;
	int				compiles_completed;
	t_request		request;
	int				queued;
}	t_coder;

void	*cx_coder_routine(void *argument);
int		cx_coder_compile(t_coder *coder);
int		cx_coder_debug(t_coder *coder);
int		cx_coder_refactor(t_coder *coder);

#endif
