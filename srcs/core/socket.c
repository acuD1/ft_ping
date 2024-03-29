/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/07 11:21:42 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/30 19:46:27 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void    setup_socket(t_ping *ping)
{
    if (ping->mode == IPV4_MODE)
    {
        if ((ping->sockfd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
        {
            dprintf(STDERR_FILENO, "ft_ping: socket(): %s\n", strerror(errno));
            exit_routine(ping, FAILURE);
        }
        if (setsockopt(ping->sockfd, IPPROTO_IP, IP_HDRINCL,
            &ping->conf.ok,
            sizeof(ping->conf.ok)) != SUCCESS)
        {
            dprintf(STDERR_FILENO, "ft_ping: setsockopt(): %s\n",
                strerror(errno));
            exit_routine(ping, FAILURE);
        }
        if (setsockopt(ping->sockfd, IPPROTO_IP, IP_TTL,
            &ping->conf.ttl, sizeof(ping->conf.ttl)) != SUCCESS)
        {
            dprintf(STDERR_FILENO, "ft_ping: setsockopt(): %s\n",
                strerror(errno));
            exit_routine(ping, FAILURE);
        }
    }
    else
    {
        if ((ping->sockfd = socket(PF_INET6, SOCK_RAW, IPPROTO_ICMPV6)) == -1)
        {
            dprintf(STDERR_FILENO, "ft_ping: socket(): %s\n", strerror(errno));
            exit_routine(ping, FAILURE);
        }
        if (setsockopt(ping->sockfd, IPPROTO_IPV6, IPV6_HDRINCL,
            &ping->conf.ok, sizeof(ping->conf.ok)) != SUCCESS)
        {
            dprintf(STDERR_FILENO, "ft_ping: setsockopt(): %s\n",
                strerror(errno));
            exit_routine(ping, FAILURE);
        }
        if (setsockopt(ping->sockfd, IPPROTO_IPV6, IPV6_RECVHOPLIMIT,
            &ping->conf.ok, sizeof(ping->conf.ok)) != SUCCESS)
        {
            dprintf(STDERR_FILENO, "ft_ping: setsockopt(): %s\n",
                strerror(errno));
            exit_routine(ping, FAILURE);
        }
        if (setsockopt(ping->sockfd, IPPROTO_IPV6, IPV6_RECVPKTINFO,
            &ping->conf.ok, sizeof(ping->conf.ok)) != SUCCESS)
        {
            dprintf(STDERR_FILENO, "ft_ping: setsockopt(): %s\n",
                strerror(errno));
            exit_routine(ping, FAILURE);
        }
    }
    if (ping->opts & D_OPT)
    {
        ping->conf.so_debug = 1;
        if (setsockopt(ping->sockfd, SOL_SOCKET, SO_DEBUG,
            &ping->conf.so_debug, sizeof(ping->conf.so_debug)) != SUCCESS)
        {
            dprintf(STDERR_FILENO, "ft_ping: setsockopt(): %s\n",
                strerror(errno));
            exit_routine(ping, FAILURE);
        }
    }
}
