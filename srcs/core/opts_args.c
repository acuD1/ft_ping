/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   opts_args.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 16:43:47 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/06 14:39:22 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static uint8_t  set_opts_args_failure(t_opts_args *opts_args)
{
    free_opts_args(opts_args);
    return (FAILURE);
}

uint8_t         set_opts_args(t_ping *ping, int argc, char **argv)
{
    t_opts_conf opts_conf;
    t_opts_args opts_args;

    ft_memset(&opts_conf, 0, sizeof(t_opts_conf));
    ft_memset(&opts_args, 0, sizeof(t_opts_args));
    opts_conf.allowed_opt = ALLOWED_OPT;
    opts_conf.allowed_opt_arg = ALLOWED_OPT_ARG;
    opts_conf.allowed_opt_tab = ALLOWED_OPT_TAB;
    opts_conf.allowed_opt_tab_arg = ALLOWED_OPT_TAB_ARG;

    if (argc < 2)
    {
        print_usage();
        exit_routine(ping, FAILURE);
    }
    if (ft_get_opts_args(&opts_args, &opts_conf, argc, argv) != SUCCESS)
        return (set_opts_args_failure(&opts_args));
    if (opts_args.all & UNALLOWED_OPT)
    {
        print_unallowed_opt(&opts_args);
        print_usage();
        return (set_opts_args_failure(&opts_args));
    }
    if (opts_args.all & H_OPT)
    {
        print_version();
        print_usage();
        return (set_opts_args_failure(&opts_args));
    }
    if (opts_args.all & V_OPT)
    {
        dprintf(STDERR_FILENO, "NOT SUPPORTED YET\n");
        return (set_opts_args_failure(&opts_args));
    }
    if (ft_lstlen(opts_args.args) > 1)
    {
        dprintf(STDERR_FILENO, "ft_ping: too many arguments: hops not implemented\n");
        print_usage();
        return (set_opts_args_failure(&opts_args));
    }
    if (resolve_target_ipv4(ping,  get_arg(&opts_args.args, POSITION(0))->arg) != SUCCESS)
        return (set_opts_args_failure(&opts_args));
    free_opts_args(&opts_args);
    return (SUCCESS);
}
