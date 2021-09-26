/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/07 16:33:30 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/26 15:29:28 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void    gettimeofday_handler(t_ping *ping, void *time)
{
    if (gettimeofday(time, NULL) < 0)
    {
        dprintf(STDERR_FILENO, "ft_ping: gettimeofday(): %s\n",
            strerror(errno));
        exit_routine(ping, FAILURE);
    }
}

double  calc_latency(void *t_start, void *t_end)
{
    struct timeval tmp_t_start  = *(struct timeval *)t_start;
    struct timeval tmp_t_end    = *(struct timeval *)t_end;
    double start;
    double end;

    start = (double)tmp_t_start.tv_sec * 1000000.0 + (double)tmp_t_start.tv_usec;
    end = (double)tmp_t_end.tv_sec * 1000000.0 + (double)tmp_t_end.tv_usec;

    return ((end - start) / 1000.0);
}

/*  Standard Derivation
** _      ________________________________________
**  \    / SUM(x * x)   (   SUM(x)      SUM(x)  )
**   \  /  ---------- - ( --------- * --------- )
**    \/        n       (     n           n     )
*/

double  calc_mdev(t_ping *ping, t_ping_rtt *ping_rtt)
{
    double mdev             = 0;
    double tmp              = 0;
    double sum_square_avg   = 0;
    double square_avg       = 0;

    #pragma clang diagnostic ignored "-Wfloat-equal"

    if (ping_rtt->sum_square > 0.0 && ping->received)
        sum_square_avg = ping_rtt->sum_square / ping->received;
    if (ping_rtt->avg > 0.0)
        square_avg = ping_rtt->avg * ping_rtt->avg;

    if (sum_square_avg > 0.0 && square_avg > 0.0)
    {
        mdev = (sum_square_avg - square_avg) / 2;
        while (mdev != tmp)
        {
            tmp = mdev;
            mdev = ((sum_square_avg - square_avg) / tmp) + (tmp / 2);
        }
    }

    return (mdev);
}

uint8_t fetch_ping_rtt(void *content, void *context)
{
    static uint16_t n               = 0;
    double          latency         = 0;
    t_ping_rtt      *ping_rtt       = (t_ping_rtt *)context;
    t_packet_data   *packet_data    = (t_packet_data *)content;

    if (packet_data->status & PACKET_RECEIVED)
    {
        n++;
        latency = calc_latency(&packet_data->time_sent,
                    &packet_data->time_recv);
        ping_rtt->sum += latency;
        ping_rtt->sum_square += latency * latency;
        if (ping_rtt->min == 0 || ping_rtt->min > latency)
            ping_rtt->min = latency;
        if (ping_rtt->max == 0 || ping_rtt->max < latency)
            ping_rtt->max = latency;
        ping_rtt->avg = ping_rtt->sum / n;
    }

    return (SUCCESS);
}

double calc_packet_loss(t_ping *ping)
{
    double packet_nonreceived = ping->sequence - ping->received;
    if (packet_nonreceived > 0)
    {
        packet_nonreceived = packet_nonreceived / ping->sequence;
        return (packet_nonreceived * 100.0);
    }
    return (0);
}
