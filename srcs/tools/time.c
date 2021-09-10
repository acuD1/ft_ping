/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/07 16:33:30 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/10 17:41:57 by arsciand         ###   ########.fr       */
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

double  calc_latency(void *t_start, void *t_end)
{
    struct timeval tmp_t_start = *(struct timeval *)t_start;
    struct timeval tmp_t_end = *(struct timeval *)t_end;
	double start;
	double end;

	start = tmp_t_start.tv_sec * 1000000.0 + tmp_t_start.tv_usec;
	end = tmp_t_end.tv_sec * 1000000.0 + tmp_t_end.tv_usec;
	return ((end - start) / 1000.0);
}

double  calc_mdev(t_ping *ping, t_ping_rtt *ping_rtt)
{
    double mdev = 0;
    double tmp  = 0;

    mdev = ((ping_rtt->sum_square / ping->received) - (ping_rtt->avg * ping_rtt->avg)) / 2;
    while (mdev != tmp)
    {
        tmp = mdev;
        mdev = (((ping_rtt->sum_square / ping->received)
                - (ping_rtt->avg * ping_rtt->avg)) / tmp + tmp) / 2;
    }
    return (mdev);
}

uint8_t    fetch_ping_rtt(void *content, void *context)
{
    static uint16_t n               = 0;
    double          latency         = 0;
    t_ping_rtt      *ping_rtt       = (t_ping_rtt *)context;
    t_packet_data   *packet_data    = (t_packet_data *)content;

    n++;
    latency = calc_latency(&packet_data->time_sent, &packet_data->time_received);
    ping_rtt->sum += latency;
    ping_rtt->sum_square += latency * latency;
    if (ping_rtt->min == 0 || ping_rtt->min > latency)
        ping_rtt->min = latency;
    if (ping_rtt->max == 0 || ping_rtt->max < latency)
        ping_rtt->max = latency;
    ping_rtt->avg = ping_rtt->sum / n;
    return (SUCCESS);
}
