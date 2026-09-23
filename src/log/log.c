/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: danda-si <danda-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 10:56:56 by danda-si          #+#    #+#             */
/*   Updated: 2026/09/22 16:15:10 by danda-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <unistd.h>
#include <errno.h>

static int	cx_append_number(char *buffer, t_ms number)
{
	char	digits[24];
	int		size;
	int		index;

	size = 0;
	while (number > 9)
	{
		digits[size++] = '0' + number % 10;
		number /= 10;
	}
	digits[size++] = '0' + number;
	index = 0;
	while (size)
		buffer[index++] = digits[--size];
	return (index);
}

static const char	*cx_event_text(t_log_event event)
{
	if (event == CX_LOG_DONGLE)
		return ("has taken a dongle\n");
	if (event == CX_LOG_COMPILE)
		return ("is compiling\n");
	if (event == CX_LOG_DEBUG)
		return ("is debugging\n");
	if (event == CX_LOG_REFACTOR)
		return ("is refactoring\n");
	return ("burned out\n");
}

static int	cx_write_line(char *buffer, int length)
{
	ssize_t	written;
	int		offset;

	offset = 0;
	while (offset < length)
	{
		written = write(1, buffer + offset, length - offset);
		if (written < 0 && errno == EINTR)
			continue ;
		if (written <= 0)
			return (-1);
		offset += written;
	}
	return (0);
}

int	cx_log_locked(t_sim *sim, int id, t_log_event event, t_ms now)
{
	char		buffer[96];
	const char	*message;
	int			length;
	int			result;

	if (sim->stop_reason != CX_RUNNING && event != CX_LOG_BURNOUT)
		return (0);
	if (now < sim->start_ms)
		now = sim->start_ms;
	length = cx_append_number(buffer, now - sim->start_ms);
	buffer[length++] = ' ';
	length += cx_append_number(buffer + length, id);
	buffer[length++] = ' ';
	message = cx_event_text(event);
	while (*message)
		buffer[length++] = *message++;
	pthread_mutex_lock(&sim->log_mutex);
	result = cx_write_line(buffer, length);
	pthread_mutex_unlock(&sim->log_mutex);
	if (result < 0)
		cx_stop_locked(sim, CX_ERROR, 0, now);
	return (result);
}
