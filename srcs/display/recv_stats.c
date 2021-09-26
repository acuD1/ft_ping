/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   recv_stats.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/12 16:57:22 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/26 12:05:25 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void     display_recv(
                    t_ping *ping, void *buffer, t_packet_data *packet_data,
                    ssize_t *bytes_received)
{
    struct iphdr *iphdr     = (struct iphdr *)buffer;
    double latency          = 0;

    if (ping->opts & DD_OPT)
        dprintf(STDOUT_FILENO, "[%ld.%ld] ",
            packet_data->time_recv.tv_sec, packet_data->time_recv.tv_usec);
    dprintf(STDOUT_FILENO, "%zd bytes from %s: icmp->seq=%hu ttl=%hhu",
        *bytes_received - IPHDR_SIZE, inet_ntop_handler(ping, &iphdr->saddr),
        packet_data->sequence, iphdr->ttl);
    if (ping->conf.payload_size >= TIMEVAL_SIZE)
    {
        latency = calc_latency(&packet_data->time_sent, &packet_data->time_recv);
        if (latency < 0.1)
            dprintf(STDOUT_FILENO, " time=%.3lf ms", latency);
        else
            dprintf(STDOUT_FILENO, " time=%.2lf ms", latency);
    }
    dprintf(STDOUT_FILENO, "\n");
}

void     display_unowned(t_ping *ping, void *buffer, ssize_t *bytes_received)
{
    struct iphdr *iphdr     = (struct iphdr *)buffer;

    dprintf(STDERR_FILENO,
        "ft_ping: received an unowned packet: %zu bytes from %s\n", *bytes_received,
        inet_ntop_handler(ping, &iphdr->saddr));
}

void     display_stats(t_ping *ping, t_ping_rtt *ping_rtt, t_ping_ewma *ping_ewma)
{
    if (ping->conf.count == 0)
        dprintf(STDOUT_FILENO, "\n");
    dprintf(STDOUT_FILENO, "--- %s ping statistics ---\n", ping->buff_ipv4);
    dprintf(STDOUT_FILENO, "%hu packets transmitted, %hu received, ",
        ping->sequence, ping->received);
    if (ping->errors)
        dprintf(STDOUT_FILENO, "+%hu errors, ", ping->errors);
    dprintf(STDOUT_FILENO, "%.f%% packet loss, time %.f ms\n",
        calc_packet_loss(ping), calc_latency(&ping->start, &ping->end));
    if (ping->received && ping->conf.payload_size >= TIMEVAL_SIZE)
    {
        dprintf(STDOUT_FILENO,
            "rtt min/avg/max/mdev = %.3lf/%.3lf/%.3lf/%.3lf ms",
            ping_rtt->min, ping_rtt->avg, ping_rtt->max,
            calc_mdev(ping, ping_rtt));
        if (ping->pipe > 1)
            dprintf(STDOUT_FILENO, ", pipe %d", ping->pipe);
        if (ping->opts & F_OPT)
        {
            dprintf(STDOUT_FILENO, ", ipg/ewma %.3f/%.3f ms",
                calc_latency(&ping->start, &ping->end) / (ping->sequence - 1),
                    ping_ewma->ewma);
        }
        dprintf(STDOUT_FILENO, "\n");
    }
    else
        dprintf(STDOUT_FILENO, "\n");
}

