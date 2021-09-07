/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/07 11:21:42 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/07 11:27:08 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void    setup_socket(t_ping *ping)
{
    if ((ping->sockfd = socket(((struct sockaddr_in *)&ping->target)->sin_family, SOCK_RAW, IPPROTO_ICMP)) == -1)
    {
        printf("socket(): ERROR: %s , errno %d\n", strerror(errno), errno);
        exit_routine(ping, FAILURE);
    }

    if (setsockopt(ping->sockfd, IPPROTO_IP, IP_TTL,
        &ping->conf.ttl, sizeof(ping->conf.ttl)) != SUCCESS)
    {
        printf("setsockopt(): ERROR: %s , errno %d\n", strerror(errno), errno);
        exit_routine(ping, FAILURE);
    }

    if (setsockopt(ping->sockfd, IPPROTO_IP, IP_HDRINCL,
        &ping->conf.custom_iphdr, sizeof(ping->conf.custom_iphdr)) != SUCCESS)
    {
        printf("setsockopt(): ERROR: %s , errno %d\n", strerror(errno), errno);
        exit_routine(ping, FAILURE);
    }
}
