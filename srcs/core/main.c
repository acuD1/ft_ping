/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/26 15:33:32 by arsciand          #+#    #+#             */
/*   Updated: 2021/04/30 15:30:10 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include <stdio.h>

/* Build informations */
static void     print_version()
{
    fprintf(stdout, "ft_ping: v.%s-%s-%s-%s\n",
        BUILD_VERSION_STRING,
        BUILD_RELEASE_STRING,
        BUILD_PATCH_STRING,
        BUILD_DATE_STRING);
}

static void     print_usage(void)
{
    fprintf(stderr, "Usage: ft_ping [-vh] <destination>\n");
}

static void     print_unallowed_opt(t_opts_args *opts_args) {
    if (opts_args->invalid)
        fprintf(stderr, "ft_ping: unrecognized option '--%s'\n", opts_args->invalid);
    else
        fprintf(stderr, "ft_ping: invalid option -- '%c'\n", (char)(opts_args->all % 128));
}

/* Options parser */
static uint8_t  get_opts_args_handler(int argc, char **argv, t_core *core)
{
    t_opts_conf opts_conf;

    ft_memset(&opts_conf, 0, sizeof(t_opts_conf));
    opts_conf.allowed_opt = ALLOWED_OPT;
    // opts_conf.allowed_opt_tab = ALLOWED_OPT_TAB;
    if (argc < 2)
    {
        print_usage();
        return (FAILURE);
    }
    if (!(core->opts_args = ft_get_opts_and_args(argc, argv, &opts_conf)))
        return (FAILURE);
    if (core->opts_args->all & UNALLOWED_OPT)
    {
        print_unallowed_opt(core->opts_args);
        print_usage();
        return (FAILURE);
    }
    if (core->opts_args->all & H_OPT)
    {
        print_version();
        print_usage();
        return (FAILURE);
    }
    if (core->opts_args->all & V_OPT)
    {
        fprintf(stderr, "NOT SUPPORTED YET\n");
        return (FAILURE);
    }
    if (ft_tablen((const char **)core->opts_args->args) > 1)
    {
        fprintf(stderr, "ft_ping: too many arguments: hops not implemented\n");
        print_usage();
        return (FAILURE);
    }
    return (SUCCESS);
}

static uint8_t init_core(t_core *core)
{
    // char    ip_string[INET_ADDRSTRLEN];
    (void)core;

    return (SUCCESS);
}

int8_t  exit_routine(t_core *core, int8_t status, uint8_t do_exit)
{
    free_opts_args(core->opts_args);
    if (do_exit == TRUE)
        exit(status);
    return (status);
}

int             main(int argc, char *argv[])
{
    t_core  core;

    ft_memset(&core, 0, sizeof(t_core));
    if (get_opts_args_handler(argc, argv, &core) != SUCCESS)
        return (exit_routine(&core, FAILURE, TRUE));

    /* Check if ft_ping executed as root */
    // if (getuid())
    // {
    //     fprintf(stderr, "ft_ping: socket: Operation not permitted\n");
    //     return (exit_routine(&core, FAILURE, TRUE));
    // }

    if (init_core(&core) != SUCCESS)
        return (exit_routine(&core, EXIT_FAILURE, FALSE));
    // else
    //     exec_ft_ping(&core);
    debug_opts_args(core.opts_args);
    return (exit_routine(&core, EXIT_SUCCESS, FALSE));
}
