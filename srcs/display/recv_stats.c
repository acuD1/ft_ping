/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   recv_stats.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/12 16:57:22 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/12 17:01:41 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void     display_recv(
                    t_ping *ping, void *buffer, t_packet_data *packet_data,
                    ssize_t *bytes_received)
{
    struct iphdr *iphdr = (struct iphdr *)buffer;

    dprintf(STDOUT_FILENO,
        "%zd bytes from %s: icmp->seq=%hu ttl=%hhu time=%.2lf ms\n",
        *bytes_received - IPHDR_SIZE, inet_ntop_handler(ping, &iphdr->saddr),
        packet_data->sequence, iphdr->ttl,
        calc_latency(&packet_data->time_sent, &packet_data->time_recv));
}

void     display_stats(t_ping *ping, t_ping_rtt *ping_rtt)
{
    dprintf(STDOUT_FILENO, "--- %s ping statistics ---\n", ping->buff_ipv4);
    dprintf(STDOUT_FILENO, "%hu packets transmitted, %hu received, ",
        ping->sequence, ping->received);
    if (ping->errors)
        dprintf(STDOUT_FILENO, "+%hu errors, ", ping->errors);
    dprintf(STDOUT_FILENO, "%.f%% packet loss, time %.f ms\n",
        calc_packet_loss(ping), calc_latency(&ping->start, &ping->end));
    if (ping->received)
        dprintf(STDOUT_FILENO,
            "rtt min/avg/max/mdev = %.3lf/%.3lf/%.3lf/%.3lf ms\n",
            ping_rtt->min, ping_rtt->avg, ping_rtt->max,
            calc_mdev(ping, ping_rtt));
    else
        dprintf(STDOUT_FILENO, "\n");
}

