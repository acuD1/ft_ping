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
    dprintf(STDOUT_FILENO, "ft_ping: v.%s-%s-%s-%s\n",
        BUILD_VERSION_STRING,
        BUILD_RELEASE_STRING,
        BUILD_PATCH_STRING,
        BUILD_DATE_STRING);
}

void     print_usage(void)
{
    dprintf(STDOUT_FILENO,
        "Usage: ft_ping\t[-vhdDfqnV] [-c count] [-i interval]\n");
    dprintf(STDOUT_FILENO,
        "\t\t[-s packetsize] [-l preload] [-t ttl] destination\n");
}

void     print_unallowed_opt(t_opts_args *opts_args)
{
    if (opts_args->invalid)
        dprintf(STDERR_FILENO,
            "ft_ping: unrecognized option '--%s'\n", opts_args->invalid);
    else
        dprintf(STDERR_FILENO,
            "ft_ping: invalid option -- '%c'\n", (char)(opts_args->all % 128));
}

void    print_init_handler(t_ping *ping)
{

    if (ping->conf.diff_dns)
        dprintf(STDOUT_FILENO, "PING %s(%s (%s)) ", ping->buff_target,
        ping->buff_dns, ping->buff_ip);
    else
        dprintf(STDOUT_FILENO, "PING %s (%s) ", ping->buff_target,
            ft_strlen(ping->buff_ip) ? ping->buff_ip : ping->buff_target);

    if (ping->mode == IPV4_MODE)
        dprintf(STDOUT_FILENO, "%d(%zu) bytes of data.\n",
            ping->conf.payload_size, ping->packet_size);
    else
        dprintf(STDOUT_FILENO, "%d data bytes\n", ping->conf.payload_size);

}
