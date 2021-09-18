/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_ping.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/29 14:30:22 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/18 16:18:31 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

uint8_t         exec_ping(t_ping *ping)
{
    char            buffer[MAX_MTU];
    struct timeval  current;
    ssize_t         bytes_recv      = 0;
    t_packet_data   *packet_data    = NULL;

    if (!(ping->packet = ft_memalloc(ping->conf.packet_size)))
        exit_routine(ping, FAILURE);

    print_init(ping);
    setup_socket(ping);
    gettimeofday_handler(ping, &ping->start);

    while (ping->conf.count)
    {
        gettimeofday_handler(ping, &current);
        if (g_ping & SEND_PACKET)
            send_packet(ping, &current);
        if (g_ping & EXIT_PING)
        {
            dprintf(STDOUT_FILENO, "\n");
            break ;
        }
        ft_memset(&buffer, 0, sizeof(buffer));
        if (!(packet_data = recv_packet(ping, buffer, &bytes_recv, &current)))
            continue ;
        else
            display_recv(ping, buffer, packet_data, &bytes_recv);
    }

    fetch_stats(ping);

    return (SUCCESS);
}
