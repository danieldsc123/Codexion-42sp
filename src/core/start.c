/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 10:56:56 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/22 16:15:10 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	cx_wait_for_start(t_sim *sim)
{
	int	active;

	pthread_mutex_lock(&sim->state_mutex);
	sim->ready_count++;
	pthread_cond_broadcast(&sim->changed);
	while (!sim->started && sim->stop_reason == CX_RUNNING)
	{
		if (pthread_cond_wait(&sim->changed, &sim->state_mutex) != 0)
			cx_stop_locked(sim, CX_ERROR, 0, 0);
	}
	active = sim->stop_reason == CX_RUNNING;
	pthread_mutex_unlock(&sim->state_mutex);
	return (active);
}

static int	cx_create_threads(t_sim *sim)
{
	int	index;

	if (pthread_create(&sim->monitor, NULL, cx_monitor_routine, sim) != 0)
		return (-1);
	sim->lifecycle.monitor_created = 1;
	index = 0;
	while (index < sim->config.number_of_coders)
	{
		if (pthread_create(&sim->coders[index].thread, NULL,
				cx_coder_routine, &sim->coders[index]) != 0)
			return (-1);
		sim->lifecycle.coders_created++;
		index++;
	}
	return (0);
}

static void	cx_open_gate(t_sim *sim)
{
	int	index;
	int	position;

	if (cx_clock_locked(sim, &sim->start_ms) < 0)
		return ;
	index = 0;
	while (index < sim->config.number_of_coders)
	{
		position = (index % ((sim->config.number_of_coders + 1) / 2)) * 2;
		if (index >= (sim->config.number_of_coders + 1) / 2)
			position++;
		sim->coders[position].last_compile_start = sim->start_ms;
		if (cx_schedule_submit_locked(&sim->coders[position]) < 0)
		{
			cx_stop_locked(sim, CX_ERROR, 0, 0);
			return ;
		}
		index++;
	}
	sim->started = 1;
	pthread_cond_broadcast(&sim->changed);
}

int	cx_sim_start(t_sim *sim)
{
	int	result;

	if (!sim->config.number_of_compiles_required)
	{
		sim->stop_reason = CX_COMPLETE;
		return (0);
	}
	result = cx_create_threads(sim);
	pthread_mutex_lock(&sim->state_mutex);
	if (result < 0)
		cx_stop_locked(sim, CX_ERROR, 0, 0);
	while (sim->stop_reason == CX_RUNNING
		&& sim->ready_count != sim->config.number_of_coders + 1)
	{
		if (pthread_cond_wait(&sim->changed, &sim->state_mutex) != 0)
			cx_stop_locked(sim, CX_ERROR, 0, 0);
	}
	if (sim->stop_reason == CX_RUNNING)
		cx_open_gate(sim);
	result = -(sim->stop_reason == CX_ERROR);
	pthread_mutex_unlock(&sim->state_mutex);
	return (result);
}

int	cx_sim_join(t_sim *sim)
{
	int	index;

	while (sim->lifecycle.coders_joined < sim->lifecycle.coders_created)
	{
		index = sim->lifecycle.coders_joined;
		if (pthread_join(sim->coders[index].thread, NULL) != 0)
			return (-1);
		sim->lifecycle.coders_joined++;
	}
	if (sim->lifecycle.monitor_created && !sim->lifecycle.monitor_joined)
	{
		if (pthread_join(sim->monitor, NULL) != 0)
			return (-1);
		sim->lifecycle.monitor_joined = 1;
	}
	return (0);
}
