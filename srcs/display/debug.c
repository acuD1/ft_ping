/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 16:53:03 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/07 13:44:01 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void    print_bytes(int bytes, void *msg)
{
    for (int i = 0; i < bytes; i++)
    {
        if (!(i & 15))
            printf("\n%04X:  ", i);
        printf("%02X ", ((unsigned char*)msg)[i]);
    }
    printf("\n");
}
