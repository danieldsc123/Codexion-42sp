/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 10:56:56 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/22 16:15:10 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	cx_dongle_init(t_dongle *dongle, int id, t_policy policy)
{
	ft_memset(dongle, 0, sizeof(*dongle));
	dongle->id = id;
	if (pthread_mutex_init(&dongle->mutex, NULL) != 0)
		return (-1);
	if (cx_heap_init(&dongle->queue, 2, policy) < 0)
	{
		pthread_mutex_destroy(&dongle->mutex);
		return (-1);
	}
	return (0);
}

void	cx_dongle_destroy(t_dongle *dongle)
{
	cx_heap_destroy(&dongle->queue);
	pthread_mutex_destroy(&dongle->mutex);
}

static void	cx_release_one(t_coder *coder, int index, t_ms now)
{
	t_dongle	*dongle;

	dongle = &coder->sim->dongles[index];
	pthread_mutex_lock(&dongle->mutex);
	if (dongle->owner_id == coder->id)
	{
		dongle->owner_id = 0;
		dongle->available_at = now + coder->sim->config.dongle_cooldown;
	}
	pthread_mutex_unlock(&dongle->mutex);
}

void	cx_dongles_release_locked(t_coder *coder, t_ms now)
{
	cx_release_one(coder, coder->left_index, now);
	if (coder->left_index != coder->right_index)
		cx_release_one(coder, coder->right_index, now);
	pthread_cond_broadcast(&coder->sim->changed);
}

void	cx_take_one_locked(t_coder *coder, int index, t_ms now)
{
	t_dongle	*dongle;

	dongle = &coder->sim->dongles[index];
	pthread_mutex_lock(&dongle->mutex);
	dongle->owner_id = coder->id;
	pthread_mutex_unlock(&dongle->mutex);
	cx_log_locked(coder->sim, coder->id, CX_LOG_DONGLE, now);
}
