/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 10:56:56 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/22 16:15:10 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	cx_finish_compile(t_coder *coder, int completed)
{
	t_sim	*sim;
	t_ms	now;

	sim = coder->sim;
	pthread_mutex_lock(&sim->state_mutex);
	now = 0;
	if (cx_clock_locked(sim, &now) == 0)
		cx_monitor_check_locked(sim, now);
	cx_dongles_release_locked(coder, now);
	if (completed && sim->stop_reason == CX_RUNNING
		&& coder->compiles_completed < sim->config.number_of_compiles_required)
	{
		coder->compiles_completed++;
		if (coder->compiles_completed
			== sim->config.number_of_compiles_required)
			sim->satisfied_count++;
		cx_monitor_check_locked(sim, now);
	}
	pthread_mutex_unlock(&sim->state_mutex);
}

int	cx_coder_compile(t_coder *coder)
{
	t_ms	deadline;
	int		completed;

	if (cx_dongles_acquire(coder) != 1)
		return (0);
	pthread_mutex_lock(&coder->sim->state_mutex);
	deadline = coder->last_compile_start + coder->sim->config.time_to_compile;
	pthread_mutex_unlock(&coder->sim->state_mutex);
	completed = cx_wait_until(coder->sim, deadline);
	cx_finish_compile(coder, completed);
	return (completed);
}

void	*cx_coder_routine(void *argument)
{
	t_coder	*coder;

	coder = argument;
	if (cx_wait_for_start(coder->sim) != 1)
		return (NULL);
	while (cx_coder_compile(coder) == 1)
	{
		if (cx_coder_debug(coder) != 1 || cx_coder_refactor(coder) != 1)
			break ;
	}
	return (NULL);
}
