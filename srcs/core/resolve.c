/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   resolve.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 13:34:50 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/10 15:05:42 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

uint8_t resolve_target_ipv4(t_ping *ping, char *target)
{
    struct  addrinfo    hints;
    struct  addrinfo    *res                        = NULL;
    int                 status                      = 0;

    ft_memset(&hints, 0, sizeof(struct addrinfo));

    /* Preparing getaddrinfo struct */
    hints.ai_flags      = AI_V4MAPPED | AI_ADDRCONFIG;
    hints.ai_family     = AF_INET;
    hints.ai_socktype   = SOCK_RAW;
    hints.ai_protocol   = IPPROTO_ICMP;

    if ((status = getaddrinfo(target, NULL, &hints, &res)) != SUCCESS)
    {
        getaddrinfo_error_handler(target, status);
        return (FAILURE);
    }

    /**/
    #pragma clang diagnostic ignored "-Wcast-align"
    /**/

    /* Setting up sockaddr storage structure */
    ((struct sockaddr_in *)&ping->target)->sin_addr.s_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr.s_addr;
    ((struct sockaddr_in *)&ping->target)->sin_port = htons(0);
    ((struct sockaddr_in *)&ping->target)->sin_family = (sa_family_t)res->ai_family;

    /* Cleaning */
    for (struct addrinfo *tmp = NULL; res; res = tmp)
    {
        tmp = res->ai_next;
        ft_memdel((void **)&res);
    }

    return (SUCCESS);
}
