/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 10:56:56 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/22 16:15:10 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdio.h>

int	main(int argc, char **argv)
{
	t_config	config;
	t_sim		sim;

	if (cx_parse_arguments(argc, argv, &config) < 0)
	{
		fprintf(stderr, "Error: invalid arguments\n");
		return (1);
	}
	if (cx_sim_init(&sim, &config) < 0)
	{
		cx_sim_destroy(&sim);
		fprintf(stderr, "Error: failed to initialize simulation\n");
		return (1);
	}
	cx_sim_destroy(&sim);
	return (0);
}
