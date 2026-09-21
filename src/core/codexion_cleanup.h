/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion_cleanup.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 10:49:48 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/21 10:49:50 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_CLEANUP_H
# define CODEXION_CLEANUP_H

struct	s_sim;

int		cx_sim_join(struct s_sim *sim);
void	cx_sim_destroy(struct s_sim *sim);

#endif
