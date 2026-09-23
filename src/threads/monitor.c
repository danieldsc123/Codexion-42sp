/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 10:56:56 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/22 16:15:10 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	cx_stop_locked(t_sim *sim, t_stop_reason reason, int id, t_ms now)
{
	if (sim->stop_reason != CX_RUNNING)
		return ;
	sim->stop_reason = reason;
	sim->burnout_id = id;
	if (reason == CX_BURNOUT)
	{
		if (cx_log_locked(sim, id, CX_LOG_BURNOUT, now) < 0)
			sim->stop_reason = CX_ERROR;
	}
	pthread_cond_broadcast(&sim->changed);
}

static int	cx_oldest_coder(t_sim *sim)
{
	int	index;
	int	oldest;

	oldest = 0;
	index = 1;
	while (index < sim->config.number_of_coders)
	{
		if (sim->coders[index].last_compile_start
			< sim->coders[oldest].last_compile_start)
			oldest = index;
		index++;
	}
	return (oldest);
}

int	cx_monitor_check_locked(t_sim *sim, t_ms now)
{
	int	oldest;

	if (sim->stop_reason != CX_RUNNING)
		return (1);
	oldest = cx_oldest_coder(sim);
	if (now >= sim->coders[oldest].last_compile_start
		+ sim->config.time_to_burnout)
		cx_stop_locked(sim, CX_BURNOUT, oldest + 1, now);
	else if (sim->satisfied_count == sim->config.number_of_coders)
		cx_stop_locked(sim, CX_COMPLETE, 0, now);
	return (sim->stop_reason != CX_RUNNING);
}

void	*cx_monitor_routine(void *argument)
{
	t_sim	*sim;
	t_ms	now;

	sim = argument;
	if (cx_wait_for_start(sim) != 1)
		return (NULL);
	pthread_mutex_lock(&sim->state_mutex);
	while (sim->stop_reason == CX_RUNNING)
	{
		if (cx_clock_locked(sim, &now) < 0
			|| cx_monitor_check_locked(sim, now))
			break ;
		if (cx_pause_locked(sim, sim->coders[cx_oldest_coder(sim)]
				.last_compile_start + sim->config.time_to_burnout) < 0)
			break ;
	}
	pthread_mutex_unlock(&sim->state_mutex);
	return (NULL);
}
