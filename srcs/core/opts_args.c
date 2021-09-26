/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   opts_args.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 16:43:47 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/26 13:23:18 by arsciand         ###   ########.fr       */
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
    t_opts_conf     opts_conf;
    t_opts_args     opts_args;
    t_opt_set_db    *tmp   = NULL;

    ft_memset(&opts_conf, 0, sizeof(t_opts_conf));
    ft_memset(&opts_args, 0, sizeof(t_opts_args));
    opts_conf.allowed_opt           = ALLOWED_OPT;
    opts_conf.allowed_opt_arg       = ALLOWED_OPT_ARG;
    opts_conf.allowed_opt_tab       = ALLOWED_OPT_TAB;
    opts_conf.allowed_opt_tab_arg   = ALLOWED_OPT_TAB_ARG;

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
    if ((tmp = get_opt_set_db(&opts_args.opt_set, "c")) != NULL)
    {
        if (tmp->arg)
        {
            long count = ft_atol(tmp->arg);
            if (count > INT32_MAX || count <= 0)
            {
                dprintf(STDERR_FILENO, "ft_ping: bad number of packets to transmit.\n");
                return (set_opts_args_failure(&opts_args));
            }
            ping->conf.count = (int32_t)count;
        }
        else
        {
            dprintf(STDERR_FILENO, "ft_ping: option requires an argument -- 'c'\n");
            print_usage();
            return (set_opts_args_failure(&opts_args));
        }
    }
    if ((tmp = get_opt_set_db(&opts_args.opt_set, "s")) != NULL)
    {
        if (tmp->arg)
        {
            int packet_size = ft_atoi(tmp->arg);
            if (packet_size < 0)
            {
                dprintf(STDERR_FILENO, "ft_ping: illegal negative packet size %d\n", packet_size);
                return (set_opts_args_failure(&opts_args));
            }
            if (packet_size > 1472)
            {
                dprintf(STDERR_FILENO, "ft_ping: packet size too large: %d. (Maximum 1472)\n", packet_size);
                    return (set_opts_args_failure(&opts_args));
            }
            ping->conf.payload_size = (uint16_t)packet_size;
        }
        else
        {
            dprintf(STDERR_FILENO, "ft_ping: option requires an argument -- 's'\n");
            print_usage();
            return (set_opts_args_failure(&opts_args));
        }
    }
    if ((tmp = get_opt_set_db(&opts_args.opt_set, "i")) != NULL)
    {
        if (tmp->arg)
        {
            double packet_size = atof(tmp->arg);
            if (packet_size < 0 || packet_size > (INT32_MAX / 1000))
            {
                dprintf(STDERR_FILENO, "ft_ping: bad timing interval\n");
                return (set_opts_args_failure(&opts_args));
            }
            ping->conf.interval = packet_size;
        }
        else
        {
            dprintf(STDERR_FILENO, "ft_ping: option requires an argument -- 'i'\n");
            print_usage();
            return (set_opts_args_failure(&opts_args));
        }
    }
    if ((tmp = get_opt_set_db(&opts_args.opt_set, "l")) != NULL)
    {
        if (tmp->arg)
        {
            int32_t preload = (int32_t)ft_atol(tmp->arg);
            if (preload <= 0 || preload > (UINT16_MAX + 1))
            {
                dprintf(STDERR_FILENO, "ft_ping: bad preload value, should be 1..65536\n");
                return (set_opts_args_failure(&opts_args));
            }
            ping->conf.preload = preload;
        }
        else
        {
            dprintf(STDERR_FILENO, "ft_ping: option requires an argument -- 'l'\n");
            print_usage();
            return (set_opts_args_failure(&opts_args));
        }
    }
    if ((tmp = get_opt_set_db(&opts_args.opt_set, "t")) != NULL)
    {
        if (tmp->arg)
        {
            long ttl = ft_atol(tmp->arg);
            if (ttl > UINT8_MAX || ttl < 0)
            {
                dprintf(STDERR_FILENO, "ft_ping: ttl %ld out of range, should be 1..255\n", ttl);
                return (set_opts_args_failure(&opts_args));
            }
            ping->conf.ttl = (uint8_t)ttl;
        }
        else
        {
            dprintf(STDERR_FILENO, "ft_ping: option requires an argument -- 'w'\n");
            print_usage();
            return (set_opts_args_failure(&opts_args));
        }
    }
    if (ft_lstlen(opts_args.args) > 1)
    {
        dprintf(STDERR_FILENO,
            "ft_ping: too many arguments: hops not implemented\n");
        print_usage();
        return (set_opts_args_failure(&opts_args));
    }
    if (ft_lstlen(opts_args.args) == 0)
    {
        print_usage();
        exit_routine(ping, FAILURE);
    }
    if (resolve_target_ipv4(ping,
        get_arg(&opts_args.args, POSITION(0))->arg) != SUCCESS)
        return (set_opts_args_failure(&opts_args));
    ping->opts = opts_args.all;
    free_opts_args(&opts_args);
    return (SUCCESS);
}
