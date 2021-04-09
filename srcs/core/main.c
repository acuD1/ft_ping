/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/26 15:33:32 by arsciand          #+#    #+#             */
/*   Updated: 2021/04/09 16:58:24 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include <stdio.h>

static void print_version()
{
    fprintf(stdout, "ft_ping: v.%s-%s-%s-%s\n",
        BUILD_VERSION_STRING,
        BUILD_RELEASE_STRING,
        BUILD_PATCH_STRING,
        BUILD_DATE_STRING);
}

static void print_usage(void)
{
    fprintf(stderr, "Usage: ft_ping [-vh] <destination>\n");
}

static void print_unallowed_opt(uint64_t opt) {
    fprintf(stderr, "ft_ping: invalid option -- '%c'\n", (char)(opt % 128));
}

static uint8_t get_opt(int argc, char **argv, t_core *core)
{
    if (argc < 2)
    {
        print_version();
        print_usage();
        return (FAILURE);
    }
    core->opt = ft_getopts(argc, argv, ALLOWED_OPT);
    if (core->opt & UNALLOWED_OPT)
    {
        print_version();
        print_unallowed_opt(core->opt);
        print_usage();
        return (FAILURE);
    }
    if (core->opt & H_OPT)
    {
        print_version();
        print_usage();
        return (FAILURE);
    }
    if (core->opt & V_OPT)
    {
        fprintf(stderr, "NOT SUPPORTED YET\n");
        // return (FAILURE);
    }
    return (SUCCESS);
}

static uint8_t init_core(t_core *core)
{
    (void)core;

    return (SUCCESS);
}

int     main(int argc, char *argv[])
{
    t_core core;

    ft_memset(&core, 0, sizeof(t_core));
    if (init_core(&core) != SUCCESS)
    {
        LOG_FATAL("MALLOC ERROR ...");
        return (EXIT_FAILURE);
    }
    if (get_opt(argc, argv, &core) != SUCCESS)
        exit(FAILURE);
    return (EXIT_SUCCESS);
}
