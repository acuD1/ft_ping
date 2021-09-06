/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 16:38:12 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/06 14:33:20 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void     getaddrinfo_error_handler(char *target, int status)
{
    switch (status)
    {
        case EAI_ADDRFAMILY:
            fprintf(stderr,
                "ft_ping: %s: Address family for hostname not supported\n",
                target);
            break ;
        case EAI_AGAIN:
            fprintf(stderr,
                "ft_ping: %s: Temporary failure in name resolution\n",
                target);
            break ;
        case EAI_NODATA:
            fprintf(stderr,
                "ft_ping: %s: No address associated with hostname\n",
                target);
            break ;
        case EAI_NONAME:
            fprintf(stderr,
                "ft_ping: %s: Name or service not known\n",
                target);
            break ;
        case EAI_SYSTEM:
            fprintf(stderr,
                "ft_ping: %s: System error\n",
                target);
            break ;
    }
}
