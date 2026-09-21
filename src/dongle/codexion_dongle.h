/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion_dongle.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 10:47:47 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/21 10:47:51 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_DONGLE_H
# define CODEXION_DONGLE_H

# include <pthread.h>
# include "scheduler/codexion_heap.h"

struct	s_coder;

typedef struct s_dongle
{
	int				id;
	int				owner_id;
	t_ms			available_at;
	pthread_mutex_t	mutex;
	t_heap			queue;
}	t_dongle;

int		cx_dongle_init(t_dongle *dongle, int id, t_policy policy);
void	cx_dongle_destroy(t_dongle *dongle);
int		cx_dongles_acquire(struct s_coder *coder);
void	cx_dongles_release_locked(struct s_coder *coder, t_ms now);

#endif
