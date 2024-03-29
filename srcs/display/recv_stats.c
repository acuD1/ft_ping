/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   recv_stats.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/12 16:57:22 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/29 16:41:14 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void    display_recv(
            t_ping *ping, void *buffer, t_packet_data *packet_data,
            ssize_t *bytes_received)
{
    uint8_t         ttl         = 0;
    ssize_t         bytes       = 0;
    double          latency     = 0;

    if (ping->opts & DD_OPT)
        dprintf(STDOUT_FILENO, "[%ld.%ld] ",
            packet_data->time_recv.tv_sec, packet_data->time_recv.tv_usec);
    if (ping->mode == IPV4_MODE)
    {
        ttl = ((struct iphdr *)buffer)->ttl;
        bytes = *bytes_received - IPHDR_SIZE;
        if (ping->conf.local == FALSE)
        {
            struct iphdr *ip = (struct iphdr *)buffer;
            ft_memset(ping->buff_ip, 0, sizeof(ping->buff_ip));
            inet_ntop(AF_INET, &ip->saddr, ping->buff_ip, INET_ADDRSTRLEN);
        }
    }
    else
    {
        ttl = packet_data->ancillary_data.ttl;
        bytes = *bytes_received;
    }

    if (ping->conf.dns && (ping->opts & N_OPT) == 0)
    {
        dprintf(STDOUT_FILENO, "%zd bytes from %s (%s): icmp_seq=%hu ttl=%hhu",
           bytes, ping->buff_dns, ping->buff_ip,
            packet_data->sequence, ttl);
    }
    else
    {
        dprintf(STDOUT_FILENO, "%zd bytes from %s: icmp_seq=%hu ttl=%hhu",
            bytes, ping->buff_ip,
            packet_data->sequence, ttl);
    }

    if (ping->conf.payload_size >= TIMEVAL_SIZE)
    {
        latency =
            calc_latency(&packet_data->time_sent, &packet_data->time_recv);
        if (latency < 0.1)
            dprintf(STDOUT_FILENO, " time=%.3lf ms", latency);
        else
            dprintf(STDOUT_FILENO, " time=%.2lf ms", latency);
    }
    dprintf(STDOUT_FILENO, "\n");
}

void    display_unowned(t_ping *ping, void *buffer, ssize_t *bytes_received)
{
    struct iphdr *iphdr     = (struct iphdr *)buffer;

    dprintf(STDERR_FILENO,
        "ft_ping: received an unowned packet: %zu bytes from %s\n",
        *bytes_received,
    inet_ntop_handler(ping, &iphdr->saddr));
}

void    display_stats(
            t_ping *ping, t_ping_rtt *ping_rtt, t_ping_ewma *ping_ewma)
{
    if (ping->conf.count == 0)
        dprintf(STDOUT_FILENO, "\n");
    dprintf(STDOUT_FILENO, "--- %s ping statistics ---\n", ping->buff_target);
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

