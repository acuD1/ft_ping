/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/26 15:25:59 by arsciand          #+#    #+#             */
/*   Updated: 2021/04/30 20:09:13 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_H
# define FT_PING_H

# define _GNU_SOURCE

# include "libft.h"
# include <unistd.h>
# include <netinet/in.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <arpa/inet.h>


# define FAILURE                2
# define SUCCESS                0
# define FALSE                  0
# define TRUE                   1
# define STRINGIZER(arg)        #arg
# define STR_VALUE(arg)         STRINGIZER(arg)
# define BUILD_VERSION_STRING   STR_VALUE(BUILDV)
# define BUILD_RELEASE_STRING   STR_VALUE(BUILDR)
# define BUILD_PATCH_STRING     STR_VALUE(BUILDP)
# define BUILD_DATE_STRING      STR_VALUE(DATE)
# define ALLOWED_OPT            "vh"
# define UNALLOWED_OPT          1ULL << 63
# define V_OPT                  1ULL << ('v' - 97)
# define H_OPT                  1ULL << ('h' - 97)

typedef struct                  s_core
{
    t_opts_args                 *opts_args;
    struct addrinfo             *res;
    char                        target_ipv4[INET_ADDRSTRLEN];
}                               t_core;

#endif
