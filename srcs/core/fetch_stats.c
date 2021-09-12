/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fetch_stats.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/12 16:59:08 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/12 16:59:47 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

/* https://github.com/iputils/iputils/issues/193 */

void     fetch_stats(t_ping *ping)
{
    struct sockaddr_in  *tmp = (struct sockaddr_in *)&ping->target;
    t_ping_rtt          ping_rtt;

    ft_memset(&ping_rtt, 0, sizeof(t_ping_rtt));
    inet_ntop_handler(ping, (uint32_t *)&tmp->sin_addr);
    ft_lstiter_ctx(ping->packets, &ping_rtt, fetch_ping_rtt);
    display_stats(ping, &ping_rtt);
}
