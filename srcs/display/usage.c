/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   usage.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 16:40:59 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/30 19:19:40 by arsciand         ###   ########.fr       */
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

void    print_init_handler(t_ping *ping)
{
    if (ping->mode == IPV4_MODE)
        inet_ntop_handler(ping, (uint32_t *)&((struct sockaddr_in *)&ping->target)->sin_addr);
    else
        inet_ntop_handler(ping, (uint32_t *)&((struct sockaddr_in6 *)&ping->target)->sin6_addr);

    if (ping->conf.diff_dns)
    {
        dprintf(STDOUT_FILENO, "PING %s(%s (%s)) ", ping->buff_target, ping->buff_dns, ping->buff_ip);
    }
    else
    {
        dprintf(STDOUT_FILENO, "PING %s (%s) ", ping->conf.dns ? ping->buff_dns : ping->buff_ip, ping->buff_ip);
    }

    if (ping->mode == IPV4_MODE)
        dprintf(STDOUT_FILENO, "%d(%zu) bytes of data.\n", ping->conf.payload_size, ping->packet_size);
    else
        dprintf(STDOUT_FILENO, "%d data bytes\n", ping->conf.payload_size);

}
