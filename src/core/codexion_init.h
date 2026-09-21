/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion_init.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 10:49:28 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/21 10:49:30 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_INIT_H
# define CODEXION_INIT_H

# include <pthread.h>
# include "parser/codexion_parse.h"
# include "dongle/codexion_dongle.h"
# include "threads/codexion_coder.h"

typedef enum e_stop_reason
{
	CX_RUNNING,
	CX_BURNOUT,
	CX_COMPLETE,
	CX_ERROR
}	t_stop_reason;

typedef struct s_lifecycle
{
	int	state_mutex_ready;
	int	log_mutex_ready;
	int	condition_ready;
	int	dongles_ready;
	int	coders_created;
	int	coders_joined;
	int	monitor_created;
	int	monitor_joined;
}	t_lifecycle;

typedef struct s_sim
{
	t_config			config;
	t_coder				*coders;
	t_dongle			*dongles;
	pthread_t			monitor;
	pthread_mutex_t		state_mutex;
	pthread_mutex_t		log_mutex;
	pthread_cond_t		changed;
	t_ms				start_ms;
	unsigned long long	next_sequence;
	int					ready_count;
	int					started;
	int					satisfied_count;
	t_stop_reason		stop_reason;
	int					burnout_id;
	t_lifecycle			lifecycle;
}	t_sim;

int	cx_sim_init(t_sim *sim, const t_config *config);
int	cx_sim_start(t_sim *sim);
int	cx_wait_for_start(t_sim *sim);

#endif
