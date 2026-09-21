/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 18:12:08 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/21 18:16:12 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <string.h>

/* Base state only; allocation and synchronization setup are still pending. */
int	cx_sim_init(t_sim *sim, const t_config *config)
{
	memset(sim, 0, sizeof(*sim));
	sim->config = *config;
	sim->stop_reason = CX_RUNNING;
	return (0);
}
