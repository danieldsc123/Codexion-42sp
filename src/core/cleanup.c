/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/22 15:34:56 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/22 15:53:15 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	cx_destroy_sync(t_sim *sim)
{
	if (sim->lifecycle.condition_ready)
	{
		pthread_cond_destroy(&sim->changed);
		sim->lifecycle.condition_ready = 0;
	}
	if (sim->lifecycle.log_mutex_ready)
	{
		pthread_mutex_destroy(&sim->log_mutex);
		sim->lifecycle.log_mutex_ready = 0;
	}
	if (sim->lifecycle.state_mutex_ready)
	{
		pthread_mutex_destroy(&sim->state_mutex);
		sim->lifecycle.state_mutex_ready = 0;
	}
}

void	cx_sim_destroy(struct s_sim *sim)
{
	while (sim->lifecycle.dongles_ready > 0)
	{
		sim->lifecycle.dongles_ready--;
		cx_dongle_destroy(&sim->dongles[sim->lifecycle.dongles_ready]);
	}
	cx_destroy_sync(sim);
	if (sim->coders)
	{
		free(sim->coders);
		sim->coders = NULL;
	}
	if (sim->dongles)
	{
		free(sim->dongles);
		sim->dongles = NULL;
	}
}
