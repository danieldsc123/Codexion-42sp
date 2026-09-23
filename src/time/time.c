/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 10:56:56 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/22 16:15:10 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <sys/time.h>
#include <errno.h>

int	cx_now_ms(t_ms *now)
{
	struct timeval	time;

	if (gettimeofday(&time, NULL) != 0)
		return (-1);
	*now = (t_ms)time.tv_sec * 1000 + time.tv_usec / 1000;
	return (0);
}

void	cx_to_timespec(t_ms deadline, struct timespec *result)
{
	result->tv_sec = deadline / 1000;
	result->tv_nsec = (deadline % 1000) * 1000000;
}

int	cx_pause_locked(t_sim *sim, t_ms deadline)
{
	struct timespec	timeout;
	int				result;

	cx_to_timespec(deadline, &timeout);
	result = pthread_cond_timedwait(&sim->changed, &sim->state_mutex, &timeout);
	if (result != 0 && result != ETIMEDOUT)
	{
		cx_stop_locked(sim, CX_ERROR, 0, 0);
		return (-1);
	}
	return (0);
}

int	cx_clock_locked(t_sim *sim, t_ms *now)
{
	if (cx_now_ms(now) < 0)
	{
		cx_stop_locked(sim, CX_ERROR, 0, 0);
		return (-1);
	}
	return (0);
}

int	cx_wait_until(t_sim *sim, t_ms deadline)
{
	t_ms	now;
	int		result;

	result = 0;
	pthread_mutex_lock(&sim->state_mutex);
	while (sim->stop_reason == CX_RUNNING)
	{
		if (cx_clock_locked(sim, &now) < 0)
			break ;
		if (cx_monitor_check_locked(sim, now))
			break ;
		if (now >= deadline)
		{
			result = 1;
			break ;
		}
		if (cx_pause_locked(sim, deadline) < 0)
			break ;
	}
	pthread_mutex_unlock(&sim->state_mutex);
	return (result);
}
