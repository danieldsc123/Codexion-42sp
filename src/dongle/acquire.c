/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   acquire.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 10:56:56 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/22 16:15:10 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	cx_take_pair(t_coder *coder, t_ms now)
{
	t_sim	*sim;

	sim = coder->sim;
	if (coder->left_index == coder->right_index)
	{
		if (coder->queued)
		{
			cx_schedule_cancel_locked(coder);
			cx_take_one_locked(coder, coder->left_index, now);
		}
		return (0);
	}
	if (!cx_schedule_ready_locked(coder, now))
		return (0);
	cx_schedule_cancel_locked(coder);
	cx_take_one_locked(coder, coder->left_index, now);
	cx_take_one_locked(coder, coder->right_index, now);
	coder->last_compile_start = now;
	cx_log_locked(sim, coder->id, CX_LOG_COMPILE, now);
	pthread_cond_broadcast(&sim->changed);
	return (sim->stop_reason == CX_RUNNING);
}

static int	cx_await_pair(t_coder *coder)
{
	t_sim	*sim;
	t_ms	now;

	sim = coder->sim;
	while (sim->stop_reason == CX_RUNNING)
	{
		if (cx_clock_locked(sim, &now) < 0
			|| cx_monitor_check_locked(sim, now))
			break ;
		if (cx_take_pair(coder, now))
			return (1);
		if (sim->stop_reason == CX_RUNNING)
			cx_pause_locked(sim, now + 1);
	}
	cx_schedule_cancel_locked(coder);
	now = 0;
	cx_now_ms(&now);
	cx_dongles_release_locked(coder, now);
	return (0);
}

int	cx_dongles_acquire(t_coder *coder)
{
	t_sim	*sim;
	int		result;

	sim = coder->sim;
	pthread_mutex_lock(&sim->state_mutex);
	if (sim->stop_reason == CX_RUNNING && !coder->queued
		&& cx_schedule_submit_locked(coder) < 0)
		cx_stop_locked(sim, CX_ERROR, 0, 0);
	result = cx_await_pair(coder);
	pthread_mutex_unlock(&sim->state_mutex);
	return (result);
}
