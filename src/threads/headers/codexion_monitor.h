/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion_monitor.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 10:48:48 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/21 10:48:50 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_MONITOR_H
# define CODEXION_MONITOR_H

# include "core/headers/codexion_init.h"

void	*cx_monitor_routine(void *argument);
int		cx_monitor_check_locked(t_sim *sim, t_ms now);
void	cx_stop_locked(t_sim *sim, t_stop_reason reason,
			int coder_id, t_ms	now);

#endif
