/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion_log.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 10:47:59 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/21 10:48:01 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_LOG_H
# define CODEXION_LOG_H

# include "time/headers/codexion_time.h"

struct	s_sim;

typedef enum e_log_event
{
	CX_LOG_DONGLE,
	CX_LOG_COMPILE,
	CX_LOG_DEBUG,
	CX_LOG_REFACTOR,
	CX_LOG_BURNOUT
}	t_log_event;

int	cx_log_locked(struct s_sim *sim, int coder_id,
		t_log_event event, t_ms	now);

#endif
