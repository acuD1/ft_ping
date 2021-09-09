/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   usage.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 16:40:59 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/09 17:30:49 by arsciand         ###   ########.fr       */
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

void    print_init(t_ping *ping)
{
    struct sockaddr_in  *tmp = (struct sockaddr_in *)&ping->target;
    char                buff_ipv4[INET_ADDRSTRLEN];

    ft_memset(&buff_ipv4, 0, sizeof(buff_ipv4));

    #pragma clang diagnostic ignored "-Wcast-align"

    if (!(inet_ntop(tmp->sin_family, &tmp->sin_addr, buff_ipv4, sizeof(buff_ipv4))))
    {
        dprintf(STDERR_FILENO, "ft_ping: inet_ntop(): %s\n", strerror(errno));
        exit_routine(ping, FAILURE);
    }

    dprintf(STDOUT_FILENO, "PING %s %d(%d) bytes of data.\n", buff_ipv4,
        ping->conf.packet_size - ICMPHDR_SIZE - IPHDR_SIZE,
        ping->conf.packet_size);
}
