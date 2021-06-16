/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   opts_args.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 16:43:47 by arsciand          #+#    #+#             */
/*   Updated: 2021/05/25 16:44:01 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

uint8_t  get_opts_args_handler(int argc, char **argv)
{
    t_opts_conf opts_conf;

    ft_memset(&opts_conf, 0, sizeof(t_opts_conf));
    opts_conf.allowed_opt = ALLOWED_OPT;
    if (argc < 2)
    {
        print_usage();
        return (FAILURE);
    }
    if (!(g_core->opts_args = ft_get_opts_and_args(argc, argv, &opts_conf)))
        return (FAILURE);
    if (g_core->opts_args->all & UNALLOWED_OPT)
    {
        print_unallowed_opt(g_core->opts_args);
        print_usage();
        return (FAILURE);
    }
    if (g_core->opts_args->all & H_OPT)
    {
        print_version();
        print_usage();
        return (FAILURE);
    }
    if (g_core->opts_args->all & V_OPT)
    {
        fprintf(stderr, "NOT SUPPORTED YET\n");
        return (FAILURE);
    }
    if (ft_tablen((const char * const *)g_core->opts_args->args) > 1)
    {
        fprintf(stderr, "ft_ping: too many arguments: hops not implemented\n");
        print_usage();
        return (FAILURE);
    }
    return (SUCCESS);
}
