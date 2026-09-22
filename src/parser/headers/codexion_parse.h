/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion_parse.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 10:48:10 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/22 14:30:52 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_PARSE_H
# define CODEXION_PARSE_H

# include "time/headers/codexion_time.h"

typedef enum e_policy
{
	CX_FIFO,
	CX_EDF
}	t_policy;

typedef struct s_config
{
	int			number_of_coders;
	t_ms		time_to_burnout;
	t_ms		time_to_compile;
	t_ms		time_to_debug;
	t_ms		time_to_refactor;
	int			number_of_compiles_required;
	t_ms		dongle_cooldown;
	t_policy	scheduler;
}	t_config;

int	cx_read_nonnegative_int(const char *text, int *value);
int	cx_parse_arguments(int argc, char **argv, t_config *config);

#endif
