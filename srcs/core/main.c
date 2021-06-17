/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/26 15:33:32 by arsciand          #+#    #+#             */
/*   Updated: 2021/06/16 12:47:16 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

t_core  *g_core = NULL;

static uint8_t init_core(void)
{
    struct  addrinfo    hints;
    struct  addrinfo    *res                        = NULL;
    char                buff_ipv4[INET_ADDRSTRLEN];
    int8_t              status                      = 0;

    ft_memset(&hints, 0, sizeof(struct addrinfo));
    ft_memset(&buff_ipv4, 0, INET_ADDRSTRLEN);

    /* Preparing getaddrinfo struct */
    hints.ai_flags      = AI_V4MAPPED | AI_ADDRCONFIG;
    hints.ai_family     = AF_INET;
    hints.ai_socktype   = SOCK_RAW;
    hints.ai_protocol   = IPPROTO_ICMP;

    if ((status = (int8_t)getaddrinfo(
        g_core->opts_args->args[0], NULL, &hints, &res))
        != SUCCESS)
        getaddrinfo_error_handler(status);

    /**/
    #pragma clang diagnostic ignored "-Wcast-align"
    /**/

    /* Filling up ipv4 stringg addresse */
    if (!(inet_ntop(res->ai_family,
        &((struct sockaddr_in *)res->ai_addr)->sin_addr, buff_ipv4,
        sizeof(buff_ipv4))))
        exit_routine(FAILURE);

    if (!(g_core->target_ipv4 = ft_strdup(buff_ipv4)))
        exit_routine(FAILURE);

    /* Setting up sockaddr storage structure */
	((struct sockaddr_in *)&g_core->target_addr)->sin_addr.s_addr
        = ((struct sockaddr_in*)res->ai_addr)->sin_addr.s_addr;
    ((struct sockaddr_in *)&g_core->target_addr)->sin_port = htons(0);
    ((struct sockaddr_in *)&g_core->target_addr)->sin_family = res->ai_family;

    /* Creating raw socket */
    if ((g_core->sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
    {
        printf("socket(): ERROR: %s , errno %d\n", strerror(errno), errno);
        exit_routine(FAILURE);
    }

    if (setsockopt(g_core->sockfd, IPPROTO_IP, IP_TTL,
        &g_core->conf.ttl, sizeof(g_core->conf.ttl)) != SUCCESS)
    {
        printf("setsockopt(): ERROR: %s , errno %d\n", strerror(errno), errno);
        exit_routine(FAILURE);
    }

    if (setsockopt(g_core->sockfd, IPPROTO_IP, IP_HDRINCL,
        &g_core->conf.custom_iphdr, sizeof( g_core->conf.custom_iphdr)) != SUCCESS)
    {
        printf("setsockopt(): ERROR: %s , errno %d\n", strerror(errno), errno);
        exit_routine(FAILURE);
    }

    /* Cleaning */
    for (struct addrinfo *tmp = NULL; res; res = tmp)
    {
        tmp = res->ai_next;
        free(res);
    }

    if (signal(SIGALRM, signal_send_packet) == SIG_ERR)
    {
        printf("signal(): ERROR: SIGALRM is invalid\n");
        exit_routine(FAILURE);
    }
    if (signal(SIGINT, signal_exit) == SIG_ERR)
    {
        printf("signal(): ERROR: SIGINT is invalid\n");
        exit_routine(FAILURE);
    }

    g_core->pid = getpid();

    if (gettimeofday(&g_core->start, NULL) < 0)
    {
        printf("gettimeofday(): ERROR: %s , errno %d\n", strerror(errno), errno);
        exit_routine(FAILURE);
    }

    return (SUCCESS);
}

int             main(int argc, char *argv[])
{
    if (!(g_core = ft_memalloc(sizeof(t_core))))
        exit_routine(FAILURE);

    /* Default conf */
    g_core->conf.custom_iphdr   = TRUE;
    g_core->conf.mtu            = MTU;
    g_core->conf.packet_size    = PACKET_SIZE;
    g_core->conf.ttl            = TTL;

    if (get_opts_args_handler(argc, argv) != SUCCESS)
        exit_routine(FAILURE);

    /* Check if ft_ping executed as root */
    if (getuid())
    {
        fprintf(stderr, "ft_ping: socket: Operation not permitted\n");
        exit_routine(FAILURE);
    }

    if (init_core() != SUCCESS)
        exit_routine(FAILURE);
    else
        exec_ft_ping();
    free_core();
    return (EXIT_SUCCESS);
}
