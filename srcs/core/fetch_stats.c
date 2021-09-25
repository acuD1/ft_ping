/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fetch_stats.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/12 16:59:08 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/25 12:30:24 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

/* https://github.com/iputils/iputils/issues/193 */

static uint8_t  fetch_ewma(void *content, void *context)
{
    t_ping_ewma     *ping_ewma      = (t_ping_ewma *)context;
    t_packet_data   *packet_data    = (t_packet_data *)content;
    double          latency         = 0;

    #pragma clang diagnostic ignored "-Wfloat-equal"

    if (packet_data->status & PACKET_RECEIVED)
    {
        latency = calc_latency(&packet_data->time_sent, &packet_data->time_recv);
        if (ping_ewma->ewma == 0)
        {
            ping_ewma->ewma = latency;
            return (SUCCESS);
        }
        ping_ewma->ewma = (latency * ping_ewma->weight) + (ping_ewma->ewma * (1.000 - ping_ewma->weight));
    }
    return (SUCCESS);
}

void     fetch_stats(t_ping *ping)
{
    struct sockaddr_in  *tmp = (struct sockaddr_in *)&ping->target;
    t_ping_rtt          ping_rtt;
    t_ping_ewma         ping_ewma;

    ft_memset(&ping_rtt, 0, sizeof(t_ping_rtt));
    ft_memset(&ping_ewma, 0, sizeof(t_ping_ewma));
    inet_ntop_handler(ping, (uint32_t *)&tmp->sin_addr);
    ft_lstiter_ctx(ping->packets, &ping_rtt, fetch_ping_rtt);
    if (ping->opts & F_OPT && ping->received)
    {
        ping_ewma.weight = (2.000 / (ping->received + 1.000));
        ft_lstiter_ctx(ping->packets, &ping_ewma, fetch_ewma);
    }
    display_stats(ping, &ping_rtt, &ping_ewma);
}
