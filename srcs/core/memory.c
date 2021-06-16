/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 16:35:54 by arsciand          #+#    #+#             */
/*   Updated: 2021/06/14 12:14:13 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void    free_core(void)
{
    free_opts_args(g_core->opts_args);
    ft_strdel(&g_core->target_ipv4);
    free(g_core);
}

void   exit_routine(int8_t status)
{
    free_core();
    exit(status);
}
