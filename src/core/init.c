/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 18:12:08 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/22 16:44:08 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	cx_allocate_arrays(t_sim *sim)
{
	size_t	count;

	count = sim->config.number_of_coders;
	if (count > SIZE_MAX / sizeof(*sim->coders)
		|| count > SIZE_MAX / sizeof(*sim->dongles))
		return (-1);
	sim->coders = malloc(sizeof(*sim->coders) * count);
	if (!sim->coders)
		return (-1);
	sim->dongles = malloc(sizeof(*sim->dongles) * count);
	if (!sim->dongles)
	{
		free(sim->coders);
		sim->coders = NULL;
		return (-1);
	}
	return (0);
}

static void	cx_init_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		ft_memset(&sim->coders[i], 0, sizeof(sim->coders[i]));
		sim->coders[i].id = i + 1;
		sim->coders[i].left_index = i;
		sim->coders[i].right_index = (i + 1) % sim->config.number_of_coders;
		sim->coders[i].sim = sim;
		i++;
	}
}

static int	cx_init_sync(t_sim *sim)
{
	if (pthread_mutex_init(&sim->state_mutex, NULL) != 0)
		return (-1);
	sim->lifecycle.state_mutex_ready = 1;
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
		return (-1);
	sim->lifecycle.log_mutex_ready = 1;
	if (pthread_cond_init(&sim->changed, NULL) != 0)
		return (-1);
	sim->lifecycle.condition_ready = 1;
	return (0);
}

int	cx_sim_init(t_sim *sim, const t_config *config)
{
	ft_memset(sim, 0, sizeof(*sim));
	cx_copy_config(&sim->config, config);
	sim->stop_reason = CX_RUNNING;
	if (cx_allocate_arrays(sim) < 0)
		return (-1);
	cx_init_coders(sim);
	if (cx_init_sync(sim) < 0)
		return (-1);
	return (0);
}
