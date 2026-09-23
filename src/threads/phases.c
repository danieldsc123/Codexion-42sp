/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   phases.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 10:56:56 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/22 16:15:10 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	cx_coder_phase(t_coder *coder, t_log_event event, t_ms duration)
{
	t_sim	*sim;
	t_ms	now;
	int		active;

	sim = coder->sim;
	pthread_mutex_lock(&sim->state_mutex);
	active = cx_clock_locked(sim, &now) == 0;
	if (active)
		active = !cx_monitor_check_locked(sim, now);
	if (active)
		active = cx_log_locked(sim, coder->id, event, now) == 0;
	pthread_mutex_unlock(&sim->state_mutex);
	if (!active)
		return (0);
	return (cx_wait_until(sim, now + duration));
}

int	cx_coder_debug(t_coder *coder)
{
	return (cx_coder_phase(coder, CX_LOG_DEBUG,
			coder->sim->config.time_to_debug));
}

int	cx_coder_refactor(t_coder *coder)
{
	return (cx_coder_phase(coder, CX_LOG_REFACTOR,
			coder->sim->config.time_to_refactor));
}
