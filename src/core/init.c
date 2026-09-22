/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 18:12:08 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/22 16:27:49 by danda-si         ###   ########.fr       */
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

int	cx_sim_init(t_sim *sim, const t_config *config)
{
	ft_memset(sim, 0, sizeof(*sim));
	sim->config = *config;
	sim->stop_reason = CX_RUNNING;
	if (cx_allocate_arrays(sim) < 0)
		return (-1);
	return (0);
}
