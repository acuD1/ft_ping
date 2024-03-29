/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_ping.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/29 14:30:22 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/26 16:14:43 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static void handle_interval(t_ping *ping)
{
    if (ping->conf.interval >= 1.0)
        alarm((uint32_t)ping->conf.interval);
    else
    {
        double interval = ping->conf.interval * 1000000.0;
        if (interval < 10000.0)
            interval = 10000.0;
        ualarm((uint32_t)interval, 0);
    }
}

uint8_t         exec_ping(t_ping *ping)
{
    char            buffer[MAX_MTU];
    struct timeval  current;
    struct timeval  last_send;
    struct timeval  res;
    ssize_t         bytes_recv      = 0;
    t_packet_data   *packet_data    = NULL;

    if (!(ping->packet = ft_memalloc(ping->packet_size)))
        exit_routine(ping, FAILURE);

    print_init_handler(ping);
    setup_socket(ping);
    gettimeofday_handler(ping, &ping->start);

    while (ping->conf.count)
    {
        gettimeofday_handler(ping, &current);
        if (ping->opts & L_OPT)
        {
            while (--ping->conf.preload > 0)
            {
                gettimeofday_handler(ping, &current);
                send_packet(ping, &current);
            }
        }
        if (g_ping & SEND_PACKET && (ping->opts & F_OPT) == 0)
        {
            if (ping->opts & C_OPT)
                gettimeofday_handler(ping, &last_send);
            send_packet(ping, &current);
            if (ping->opts & I_OPT)
                handle_interval(ping);
            else
                alarm(1);
            g_ping = 0;
        }
        if (g_ping & EXIT_PING)
        {
            dprintf(STDOUT_FILENO, "\n");
            break ;
        }
        if (ping->opts & F_OPT && g_ping & SEND_PACKET)
        {
            gettimeofday_handler(ping, &last_send);
            send_packet(ping, &current);
            g_ping = 0;
            g_ping |= PENDING_PACKET;
            dprintf(STDOUT_FILENO, ".");
        }
        ft_memset(&buffer, 0, sizeof(buffer));
        if (ping->opts & C_OPT && ping->conf.count == 0)
        {
            struct timeval  timeout;
            while (1)
            {
                ft_memset(&res, 0, sizeof(struct timeval));
                ft_memset(&timeout, 0, sizeof(struct timeval));
                gettimeofday_handler(ping, &timeout);
                timersub(&timeout, &last_send, &res);
                if ((double)res.tv_sec > 1.0)
                    break;
                if (!(packet_data =
                        recv_packet(ping, buffer, &bytes_recv, &timeout)))
                    continue ;
                if ((ping->opts & Q_OPT) == 0)
                {
                    if (ping->opts & F_OPT)
                    {
                        dprintf(STDOUT_FILENO, "\b");
                        g_ping |= SEND_PACKET;
                    }
                    else
                        display_recv(ping, buffer, packet_data, &bytes_recv);
                    break ;
                }
            }
        }
        else if (!(packet_data =
                    recv_packet(ping, buffer, &bytes_recv, &current)))
        {
            if (ping->opts & F_OPT)
            {
                ft_memset(&res, 0, sizeof(struct timeval));
                timersub(&current, &ping->start, &res);
                double total_micro_second = (double)res.tv_sec * 1000000.0;
                total_micro_second += (double)res.tv_usec;
                if (!ping->received || (ping->received
                        && ((double)ping->received / total_micro_second)
                        < 0.0001))
                {
                    ft_memset(&res, 0, sizeof(struct timeval));
                    timersub(&current, &last_send, &res);
                    if ((double)res.tv_usec > 20000.0
                        && g_ping & PENDING_PACKET)
                    {
                        gettimeofday_handler(ping, &last_send);
                        g_ping |= SEND_PACKET;
                    }
                }
            }
        }
        else
        {
            if ((ping->opts & Q_OPT) == 0)
            {
                if (ping->opts & F_OPT)
                {
                    dprintf(STDOUT_FILENO, "\b");
                    g_ping |= SEND_PACKET;
                }
                else
                    display_recv(ping, buffer, packet_data, &bytes_recv);
            }
        }
    }

    fetch_stats(ping);

    return (SUCCESS);
}
