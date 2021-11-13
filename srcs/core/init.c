/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 13:37:02 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/26 12:34:02 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

volatile sig_atomic_t g_ping = 0;

void    init_ping(t_ping *ping)
{
    ft_memset(ping, 0, sizeof(t_ping));

    ping->conf.ok = TRUE;
    ping->conf.payload_size = PAYLOAD_SIZE;
    ping->conf.ttl          = TTL;
    ping->conf.pid          = getpid();
    ping->conf.count        = 1;

    signal(SIGINT, sig_handler);
    signal(SIGALRM, sig_handler);

    g_ping |= SEND_PACKET;
}
