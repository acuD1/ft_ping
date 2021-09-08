/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 15:48:18 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/08 14:59:20 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void    sig_handler(int signo)
{
    switch (signo)
    {
        case SIGALRM:
            g_ping |= SEND_PACKET;
            break;
        case SIGINT:
            g_ping |= EXIT_PING;
            break;
        default:
            g_ping = 0;
            break;
    }
}
