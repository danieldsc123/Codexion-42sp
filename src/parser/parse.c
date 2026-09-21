/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 10:56:56 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/21 13:17:36 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <string.h>

int	cx_read_nonnegative_int(const char *text, int *value)
{
	int	parsed_number;

	parsed_number = ft_atoi(text);
	if (parsed_number < 0)
		return (-1);
	*value = parsed_number;
	return (0);
}

static int	cx_read_time_ms(const char *text, t_ms *value)
{
	int	parsed_number;

	if (cx_read_nonnegative_int(text, &parsed_number) < 0)
		return (-1);
	*value = parsed_number;
	return (0);
}

static int	cx_read_all_times(char **argv, t_config *parsed_config)
{
	if (cx_read_time_ms(argv[2], &parsed_config->time_to_burnout) < 0
		|| cx_read_time_ms(argv[3], &parsed_config->time_to_compile) < 0
		|| cx_read_time_ms(argv[4], &parsed_config->time_to_debug) < 0
		|| cx_read_time_ms(argv[5], &parsed_config->time_to_refactor) < 0
		|| cx_read_time_ms(argv[7], &parsed_config->dongle_cooldown) < 0)
		return (-1);
	return (0);
}

int	cx_parse_arguments(int argc, char **argv, t_config *config)
{
	t_config	parsed_config;

	if (argc != 9)
		return (-1);
	if (cx_read_nonnegative_int(argv[1], &parsed_config.number_of_coders) < 0
		|| parsed_config.number_of_coders == 0)
		return (-1);
	if (cx_read_all_times(argv, &parsed_config) < 0)
		return (-1);
	if (cx_read_nonnegative_int(argv[6],
			&parsed_config.number_of_compiles_required) < 0)
		return (-1);
	if (strcmp(argv[8], "fifo") == 0)
		parsed_config.scheduler = CX_FIFO;
	else if (strcmp(argv[8], "edf") == 0)
		parsed_config.scheduler = CX_EDF;
	else
		return (-1);
	*config = parsed_config;
	return (0);
}
