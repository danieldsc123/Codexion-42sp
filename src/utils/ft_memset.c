/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/22 14:33:09 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/22 14:55:13 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/headers/codexion_utils.h"

void	*ft_memset(void *s, int c, size_t n)
{
	unsigned char	*ptr;

	ptr = (unsigned char *)s;
	while (n--)
	{
		*ptr++ = (unsigned char)c;
	}
	return (s);
}
