/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion_time.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 10:48:58 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/21 10:49:00 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_TIME_H
# define CODEXION_TIME_H

# include <time.h>

struct				s_sim;

typedef long long	t_ms;

int		cx_now_ms(t_ms *now);
void	cx_to_timespec(t_ms deadline, struct timespec *result);
int		cx_wait_until(struct s_sim *sim, t_ms deadline);

int		cx_pause_locked(struct s_sim *sim, t_ms deadline);
int		cx_clock_locked(struct s_sim *sim, t_ms *now);

#endif
