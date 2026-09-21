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
