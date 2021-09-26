/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   usage.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 16:40:59 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/26 16:47:29 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

/* Build informations */
void     print_version(void)
{
    fprintf(stdout, "ft_ping: v.%s-%s-%s-%s\n",
        BUILD_VERSION_STRING,
        BUILD_RELEASE_STRING,
        BUILD_PATCH_STRING,
        BUILD_DATE_STRING);
}

void     print_usage(void)
{
    fprintf(stderr, "Usage: ft_ping [-vh] <destination>\n");
}

void     print_unallowed_opt(t_opts_args *opts_args)
{
    if (opts_args->invalid)
        fprintf(stderr,
            "ft_ping: unrecognized option '--%s'\n", opts_args->invalid);
    else
        fprintf(stderr,
            "ft_ping: invalid option -- '%c'\n", (char)(opts_args->all % 128));
}

static void    print_init_ipv4(t_ping *ping)
{
    struct sockaddr_in  *tmp = (struct sockaddr_in *)&ping->target;
    char                buff[INET_ADDRSTRLEN];
    char                hbuf[NI_MAXHOST];

    ft_memset(&buff, 0, sizeof(buff));
    ft_memset(&hbuf, 0, sizeof(hbuf));

    #pragma clang diagnostic ignored "-Wcast-align"

    if (!(inet_ntop(tmp->sin_family, &tmp->sin_addr, buff,
            sizeof(buff))))
    {
        dprintf(STDERR_FILENO, "ft_ping: inet_ntop(): %s\n", strerror(errno));
        exit_routine(ping, FAILURE);
    }

    if ((ping->opts & N_OPT) == 0)
    {
        if (getnameinfo((struct sockaddr *)&ping->target, sizeof(struct sockaddr), hbuf, sizeof(hbuf),
                NULL, 0, NI_NAMEREQD))
            printf("could not resolve hostname");
        else
            printf("host=%s\n", hbuf);
    }
    else
    {
        dprintf(STDERR_FILENO, "TF\n");
    }

    dprintf(STDOUT_FILENO, "PING %s %d(%d) bytes of data.\n", buff,
        ping->conf.payload_size,
        ping->conf.payload_size + IPHDR_SIZE + ICMPHDR_SIZE);
}

static void    print_init_ipv6(t_ping *ping)
{
    struct sockaddr_in6 *tmp = (struct sockaddr_in6 *)&ping->target;
    char                buff[INET6_ADDRSTRLEN];

    ft_memset(&buff, 0, sizeof(buff));

    #pragma clang diagnostic ignored "-Wcast-align"

    if (!(inet_ntop(tmp->sin6_family, &tmp->sin6_addr, buff,
            sizeof(buff))))
    {
        dprintf(STDERR_FILENO, "ft_ping: inet_ntop(): %s\n", strerror(errno));
        exit_routine(ping, FAILURE);
    }

    dprintf(STDOUT_FILENO, "PING %s %d(%d) bytes of data.\n", buff,
        ping->conf.payload_size,
        ping->conf.payload_size + IPHDR_SIZE + ICMPHDR_SIZE);
}

void    print_init_handler(t_ping *ping)
{
    if (ping->mode & IPV4_MODE)
        print_init_ipv4(ping);
    if (ping->mode & IPV6_MODE)
        print_init_ipv6(ping);
}
