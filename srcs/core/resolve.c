/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   resolve.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 13:34:50 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/06 14:33:57 by arsciand         ###   ########.fr       */
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

    /* Creating raw socket */
    // if ((g_core->sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
    // {
    //     printf("socket(): ERROR: %s , errno %d\n", strerror(errno), errno);
    //     exit_routine(FAILURE);
    // }

    // if (setsockopt(g_core->sockfd, IPPROTO_IP, IP_TTL,
    //     &g_core->conf.ttl, sizeof(g_core->conf.ttl)) != SUCCESS)
    // {
    //     printf("setsockopt(): ERROR: %s , errno %d\n", strerror(errno), errno);
    //     exit_routine(FAILURE);
    // }

    // if (setsockopt(g_core->sockfd, IPPROTO_IP, IP_HDRINCL,
    //     &g_core->conf.custom_iphdr, sizeof( g_core->conf.custom_iphdr)) != SUCCESS)
    // {
    //     printf("setsockopt(): ERROR: %s , errno %d\n", strerror(errno), errno);
    //     exit_routine(FAILURE);
    // }

    /* Cleaning */
    for (struct addrinfo *tmp = NULL; res; res = tmp)
    {
        tmp = res->ai_next;
        ft_memdel((void **)&res);
    }

    // if (signal(SIGALRM, signal_send_packet) == SIG_ERR)
    // {
    //     printf("signal(): ERROR: SIGALRM is invalid\n");
    //     exit_routine(FAILURE);
    // }
    // if (signal(SIGINT, signal_exit) == SIG_ERR)
    // {
    //     printf("signal(): ERROR: SIGINT is invalid\n");
    //     exit_routine(FAILURE);
    // }

    // g_core->pid = getpid();

    // if (gettimeofday(&g_core->start, NULL) < 0)
    // {
    //     printf("gettimeofday(): ERROR: %s , errno %d\n", strerror(errno), errno);
    //     exit_routine(FAILURE);
    // }

    return (SUCCESS);
}
