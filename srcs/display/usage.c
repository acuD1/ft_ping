/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   usage.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 16:40:59 by arsciand          #+#    #+#             */
/*   Updated: 2021/06/14 12:15:16 by arsciand         ###   ########.fr       */
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
