/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loss.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/10 15:41:29 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/11 12:47:27 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

double calc_packet_loss(t_ping *ping)
{
    double packet_nonreceived = ping->sequence - ping->received;
    if (packet_nonreceived > 0)
    {
        packet_nonreceived = packet_nonreceived / ping->sequence;
        return (packet_nonreceived * 100.0);
    }
    return (0);
}
