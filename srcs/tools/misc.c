/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   misc.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/12 16:46:20 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/12 16:51:38 by arsciand         ###   ########.fr       */
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
