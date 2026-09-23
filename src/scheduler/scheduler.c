/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 10:56:56 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/22 16:15:10 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <limits.h>

static int	cx_submit_one(t_dongle *dongle, t_request request)
{
	int	result;

	pthread_mutex_lock(&dongle->mutex);
	result = cx_heap_push(&dongle->queue, request);
	pthread_mutex_unlock(&dongle->mutex);
	return (result);
}

int	cx_schedule_submit_locked(t_coder *coder)
{
	t_sim	*sim;

	sim = coder->sim;
	if (coder->queued || sim->next_sequence == ULLONG_MAX)
		return (-1);
	coder->request.coder_id = coder->id;
	coder->request.sequence = sim->next_sequence++;
	coder->request.deadline = coder->last_compile_start
		+ sim->config.time_to_burnout;
	if (cx_submit_one(&sim->dongles[coder->left_index], coder->request) < 0)
		return (-1);
	coder->queued = 1;
	if (coder->left_index != coder->right_index
		&& cx_submit_one(&sim->dongles[coder->right_index], coder->request) < 0)
	{
		cx_schedule_cancel_locked(coder);
		return (-1);
	}
	pthread_cond_broadcast(&sim->changed);
	return (0);
}

void	cx_schedule_cancel_locked(t_coder *coder)
{
	t_dongle	*dongle;

	if (!coder->queued)
		return ;
	dongle = &coder->sim->dongles[coder->left_index];
	pthread_mutex_lock(&dongle->mutex);
	cx_heap_remove(&dongle->queue, coder->id);
	pthread_mutex_unlock(&dongle->mutex);
	if (coder->left_index != coder->right_index)
	{
		dongle = &coder->sim->dongles[coder->right_index];
		pthread_mutex_lock(&dongle->mutex);
		cx_heap_remove(&dongle->queue, coder->id);
		pthread_mutex_unlock(&dongle->mutex);
	}
	coder->queued = 0;
}

static int	cx_one_ready(t_dongle *dongle, int id, t_ms now)
{
	const t_request	*first;
	int				ready;

	pthread_mutex_lock(&dongle->mutex);
	first = cx_heap_peek(&dongle->queue);
	ready = (!dongle->owner_id && now >= dongle->available_at
			&& first && first->coder_id == id);
	pthread_mutex_unlock(&dongle->mutex);
	return (ready);
}

int	cx_schedule_ready_locked(t_coder *coder, t_ms now)
{
	t_sim	*sim;

	sim = coder->sim;
	if (coder->left_index == coder->right_index)
		return (0);
	return (cx_one_ready(&sim->dongles[coder->left_index], coder->id, now)
		&& cx_one_ready(&sim->dongles[coder->right_index], coder->id, now));
}
