/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   resolve.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 13:34:50 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/26 16:51:44 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

uint8_t resolve_target(t_ping *ping, char *target)
{
    struct  addrinfo    hints;
    struct  addrinfo    *res                        = NULL;
    int                 status                      = 0;
    unsigned char       buff[sizeof(struct in6_addr)];

    ft_memset(&hints, 0, sizeof(struct addrinfo));

    /* Preparing getaddrinfo struct */
    hints.ai_flags      = AI_V4MAPPED | AI_ALL | AI_ADDRCONFIG;
    hints.ai_family     = AF_UNSPEC;
    hints.ai_socktype   = SOCK_RAW;
    hints.ai_protocol   = IPPROTO_ICMP;

    if ((status = getaddrinfo(target, NULL, &hints, &res)) != SUCCESS)
    {
        getaddrinfo_error_handler(target, status);
        return (FAILURE);
    }

    // uint8_t numeric = TRUE;
    // dprintf(STDERR_FILENO, "CHECKING IF NUMERIC INPUT\n");
    // for (size_t i = 0; target[i]; i++)
    // {
    //     if ()
    // }

    /**/
    #pragma clang diagnostic ignored "-Wcast-align"
    /**/

    for (struct addrinfo *tmp = res; tmp != NULL; tmp = tmp->ai_next)
    {
        if (tmp->ai_family == PF_INET6)
        {
            ping->mode |= IPV6_MODE;
            dprintf(STDERR_FILENO, "IPV6\n");
            // exit_routine(ping, FAILURE);
            ((struct sockaddr_in6 *)&ping->target)->sin6_addr.__in6_u
                = ((struct sockaddr_in6 *)res->ai_addr)->sin6_addr.__in6_u;
            ((struct sockaddr_in6 *)&ping->target)->sin6_port
                = htons(0);
            ((struct sockaddr_in6 *)&ping->target)->sin6_family
                = (sa_family_t)res->ai_family;
            break ;
        }
        if (tmp->ai_family == PF_INET)
        {
            ping->mode |= IPV4_MODE;
            if (inet_pton(AF_INET, target, buff))
            {
                dprintf(STDERR_FILENO, "NUMERIC\n");
                ping->opts = ping->opts | 1ULL << ('n' - 97);
            }
            /* Setting up sockaddr storage structure */
            ((struct sockaddr_in *)&ping->target)->sin_addr.s_addr
                = ((struct sockaddr_in *)res->ai_addr)->sin_addr.s_addr;
            ((struct sockaddr_in *)&ping->target)->sin_port
                = htons(0);
            ((struct sockaddr_in *)&ping->target)->sin_family
                = (sa_family_t)res->ai_family;
            break ;
        }
    }


    /* Cleaning */
    for (struct addrinfo *tmp = NULL; res; res = tmp)
    {
        tmp = res->ai_next;
        ft_memdel((void **)&res);
    }

    // getnameinfo()

    return (SUCCESS);
}
