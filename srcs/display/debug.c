/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 16:53:03 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/07 16:42:21 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include <time.h>

void    print_bytes(int bytes, void *msg)
{
    for (int i = 0; i < bytes; i++)
    {
        if (!(i & 15))
            printf("\n[DEBUG] %04X:  ", i);
        printf("%02X ", ((unsigned char*)msg)[i]);
    }
    printf("\n");
}

void   print_time(void *time)
{
    struct timeval tmp;

    ft_memset(&tmp, 0, sizeof(tmp));
    memcpy(&tmp, time, sizeof(tmp));
    print_bytes(sizeof(tmp), &tmp);
    dprintf(STDERR_FILENO, "[DEBUG] TIME SEC |%lu| USEC |%lu|\n", tmp.tv_sec, tmp.tv_usec);
}
