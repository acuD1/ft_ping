/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_ping.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/29 14:30:22 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/12 17:05:20 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

uint8_t         exec_ping(t_ping *ping)
{
    char            buffer[MAX_MTU];
    struct timeval  current;
    ssize_t         bytes_recv      = 0;
    char            *packet         = NULL;
    t_packet_data   *packet_data    = NULL;

    if (!(packet = ft_memalloc(ping->conf.packet_size)))
        exit_routine(ping, FAILURE);

    print_init(ping);
    setup_socket(ping);
    gettimeofday_handler(ping, &ping->start);

    while (1)
    {
        gettimeofday_handler(ping, &current);
        if (g_ping & SEND_PACKET)
            send_packet(ping, packet, &current);
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
    ft_strdel(&packet);

    return (SUCCESS);
}
