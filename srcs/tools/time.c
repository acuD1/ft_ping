/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/07 16:33:30 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/09 17:16:43 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void    gettimeofday_handler(t_ping *ping, void *time)
{
    if (gettimeofday(time, NULL) < 0)
    {
        dprintf(STDERR_FILENO, "ft_ping: gettimeofday(): %s\n", strerror(errno));
        exit_routine(ping, FAILURE);
    }
}


double  calc_latency(t_packet_data *packet_data)
{
	double sent;
	double recv;

	sent = packet_data->time_sent.tv_sec * 1000000.0 + packet_data->time_sent.tv_usec;
	recv = packet_data->time_received.tv_sec * 1000000.0 + packet_data->time_received.tv_usec;
	return ((recv - sent) / 1000.0);
}
