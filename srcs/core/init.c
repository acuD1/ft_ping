/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 13:37:02 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/06 14:32:25 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

t_ping_global   *g_ping_global = NULL;

void    init_ping(t_ping *ping)
{
    if (!(g_ping_global = ft_memalloc(sizeof(t_ping_global))))
        exit_routine(ping, FAILURE);
    ft_memset(ping, 0, sizeof(t_ping));
    ping->conf.custom_iphdr   = TRUE;
    ping->conf.mtu            = MTU;
    ping->conf.packet_size    = PACKET_SIZE;
    ping->conf.ttl            = TTL;
}
