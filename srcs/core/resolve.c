/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   resolve.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 13:34:50 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/30 19:18:53 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static void resolve_local(t_ping *ping)
{
    struct ifaddrs  *ifap       = NULL;
    void            *in_addr    = NULL;

    if (getifaddrs(&ifap) == -1)
    {
        dprintf(STDERR_FILENO, "ft_ping: getifaddrs(): %s\n", strerror(errno));
        exit_routine(ping, FAILURE);
    }

    /**/
    #pragma clang diagnostic ignored "-Wcast-align"
    /**/

    for (struct ifaddrs *ifa = ifap; ifa; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
            continue;
        if (!(ifa->ifa_flags & IFF_UP))
            continue;
        if (ifa->ifa_addr->sa_family == ping->mode)
        {
            switch (ifa->ifa_addr->sa_family)
            {
                case AF_INET:
                {
                    struct sockaddr_in *s4
                        = (struct sockaddr_in *)ifa->ifa_addr;
                    if (s4->sin_addr.s_addr == htonl(INADDR_LOOPBACK))
                        in_addr = &s4->sin_addr;
                    break;
                }
                case AF_INET6:
                {
                    struct sockaddr_in6 *s6
                        = (struct sockaddr_in6 *)ifa->ifa_addr;
                    if (IN6_IS_ADDR_LOOPBACK(&s6->sin6_addr))
                        in_addr = &s6->sin6_addr;
                    break;
                }
                default:
                    continue;
            }
        }
    }
    if (!inet_ntop(ping->mode, in_addr, ping->buff_ip, sizeof(ping->buff_ip)))
    {
        dprintf(STDERR_FILENO, "ft_ping: inet_ntop(): %s\n", strerror(errno));
        freeifaddrs(ifap);
        exit_routine(ping, FAILURE);
    }
    freeifaddrs(ifap);
}

uint8_t     resolve_target(t_ping *ping, char *target)
{
    struct  addrinfo    hints;
    struct  addrinfo    *res                        = NULL;
    int                 status                      = 0;

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

    /**/
    #pragma clang diagnostic ignored "-Wcast-align"
    /**/

    for (struct addrinfo *tmp = res; tmp != NULL; tmp = tmp->ai_next)
    {
        if (tmp->ai_family == AF_INET6)
        {
            ping->mode |= IPV6_MODE;
            ((struct sockaddr_in6 *)&ping->target)->sin6_addr
                = ((struct sockaddr_in6 *)res->ai_addr)->sin6_addr;
            ((struct sockaddr_in6 *)&ping->target)->sin6_port
                = htons(0);
            ((struct sockaddr_in6 *)&ping->target)->sin6_family
                = (sa_family_t)res->ai_family;
            if (inet_pton_handler(ping, target) != TRUE)
            {
                getnameinfo_handler(ping);
                if (ft_strequ(target, ping->buff_dns) == FALSE)
                    ping->conf.diff_dns = TRUE;
            }
            break ;
        }
        if (tmp->ai_family == AF_INET)
        {
            ping->mode |= IPV4_MODE;
            ((struct sockaddr_in *)&ping->target)->sin_addr.s_addr
                = ((struct sockaddr_in *)res->ai_addr)->sin_addr.s_addr;
            ((struct sockaddr_in *)&ping->target)->sin_port
                = htons(0);
            ((struct sockaddr_in *)&ping->target)->sin_family
                = (sa_family_t)res->ai_family;
            if (inet_pton_handler(ping, target) != TRUE)
            {
                getnameinfo_handler(ping);
                if (ft_strequ(target, ping->buff_dns) == FALSE)
                    ping->conf.diff_dns = TRUE;
            }
            break ;
        }
    }

    ft_strcpy(ping->buff_target, target);

    ping->packet_size = ping->conf.payload_size;
    ping->packet_size += ping->mode == IPV4_MODE ? IPHDR_SIZE : IPV6HDR_SIZE;
    ping->packet_size += ICMPHDR_SIZE;

    /* Cleaning */
    for (struct addrinfo *tmp = NULL; res; res = tmp)
    {
        tmp = res->ai_next;
        ft_memdel((void **)&res);
    }

    if ((ping->mode == IPV4_MODE
        && !*((uint32_t *)&((struct sockaddr_in *)&ping->target)->sin_addr))
        || (ping->mode == IPV6_MODE && !*((uint32_t *)&((struct sockaddr_in6 *)&ping->target)->sin6_addr)))
    {
        resolve_local(ping);
        if (inet_pton_handler(ping, target) != TRUE)
            ping->conf.diff_dns = TRUE;
        ping->conf.local = TRUE;
    }

    dprintf(STDERR_FILENO, "[DEBUG] TARGET\t|%s|\n[DEBUG] DNS\t|%s|\n[DEBUG] IP\t|%s|\n", ping->buff_target, ping->buff_dns, ping->buff_ip);

    return (SUCCESS);
}
