/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   misc.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/12 16:46:20 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/12 17:53:05 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

char    *inet_ntop_handler(t_ping *ping, uint32_t *addr)
{
    ft_memset(&ping->buff_ipv4, 0, sizeof(ping->buff_ipv4));
    if (!(inet_ntop(AF_INET, addr, ping->buff_ipv4, sizeof(ping->buff_ipv4))))
    {
        dprintf(STDERR_FILENO, "ft_ping: inet_ntop(): %s\n", strerror(errno));
        exit_routine(ping, FAILURE);
    }

    return (ping->buff_ipv4);
}

uint16_t in_cksum(void *buffer, size_t len)
{
    uint16_t *tmp   = (uint16_t *)buffer;
    uint32_t sum    = 0;

    while (len > 1)
    {
        sum += *tmp++;
        len -= 2;
    }

    if (len > 0)
        sum += *(uint8_t *)tmp;

    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    return ((uint16_t)~sum);
}
