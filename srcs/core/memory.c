/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 16:35:54 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/18 14:23:15 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void    free_ping(t_ping *ping)
{
    ft_lstdel(&ping->packets, NULL);
    ft_strdel(&ping->packet);
}

void __attribute__ ((noreturn)) exit_routine(t_ping *ping, int8_t status)
{
    free_ping(ping);
    exit(status);
}
