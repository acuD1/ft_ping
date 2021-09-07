/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 13:37:02 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/07 11:26:30 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

volatile sig_atomic_t g_ping = 0;

void    init_ping(t_ping *ping)
{
    ft_memset(ping, 0, sizeof(t_ping));
    ping->conf.custom_iphdr   = TRUE;
    // ping->conf.mtu            = MTU;
    ping->conf.packet_size    = PACKET_SIZE;
    ping->conf.ttl            = TTL;
    signal(SIGINT, sig_handler);
    signal(SIGALRM, sig_handler);
}