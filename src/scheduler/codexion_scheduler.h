/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion_scheduler.h                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 10:48:30 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/21 10:48:32 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_SCHEDULER_H
# define CODEXION_SCHEDULER_H

# include "scheduler/codexion_heap.h"

struct	s_coder;

int		cx_request_before(const t_request *a, const t_request *b,
			t_policy	policy);
int		cx_schedule_submit_locked(struct s_coder *coder);
void	cx_schedule_cancel_locked(struct s_coder *coder);
int		cx_schedule_ready_locked(struct s_coder *coder, t_ms now);

#endif
