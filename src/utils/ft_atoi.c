/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 11:30:03 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/22 14:55:43 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/headers/codexion_utils.h"
#include <limits.h>

int	ft_atoi(const char *str)
{
	int	result;
	int	digit;

	if (!str || !*str)
		return (-1);
	result = 0;
	while (*str)
	{
		if (*str < '0' || *str > '9')
			return (-1);
		digit = *str - '0';
		if (result > (INT_MAX - digit) / 10)
			return (-1);
		result = result * 10 + digit;
		str++;
	}
	return (result);
}
