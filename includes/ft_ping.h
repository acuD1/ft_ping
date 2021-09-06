/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/26 15:25:59 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/06 14:39:35 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_H
# define FT_PING_H

# pragma clang diagnostic ignored "-Wreserved-id-macro"
# define _GNU_SOURCE

# include "libft.h"
# include <unistd.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <signal.h>
# include <sys/time.h>
# include <string.h>
# include <errno.h>

# define errno                  (*__errno_location ())

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
# define ALLOWED_OPT_ARG        NULL
# define ALLOWED_OPT_TAB        NULL
# define ALLOWED_OPT_TAB_ARG    NULL
# define UNALLOWED_OPT          1ULL << 63
# define V_OPT                  1ULL << ('v' - 97)
# define H_OPT                  1ULL << ('h' - 97)
# define PACKET_SIZE            84
# define IPHDR_SIZE             20
# define TTL                    64
# define MTU                    1500

typedef struct                  s_icmp_area
{
    struct icmphdr              icmphdr;
    char                        message[PACKET_SIZE - sizeof(struct icmphdr) - IPHDR_SIZE];
}                               t_icmp_area;

typedef struct                  s_icmp_packet_v4
{
    struct  iphdr               iphdr;
    t_icmp_area                 icmp_area;
}                               t_icmp_packet_v4;

typedef struct                  s_conf
{
    int                         custom_iphdr;
    int                         mtu;
    int                         packet_size;
    int                         ttl;
}                               t_conf;

typedef struct                  s_ping_global
{
    volatile sig_atomic_t       sig_int;
}                               t_ping_global;

typedef struct                  s_ping
{
    // t_opts_args                 *opts_args;
    t_conf                      conf;
    t_icmp_packet_v4            packet;
    int                         sockfd;
    char                        *target_ipv4;
    uint16_t                    sequence;
    uint16_t                    errors;
    pid_t                       pid;
    struct timeval              start;
    struct timeval              end;
    struct sockaddr_storage     target;
}                               t_ping;

extern  t_ping_global           *g_ping_global;

uint8_t                          exec_ft_ping(void);
void                             exit_routine(t_ping *ping, int8_t status);
void                             getaddrinfo_error_handler(char *target, int status);
void                             free_ping(t_ping *ping);
uint8_t                          set_opts_args(t_ping *ping, int argc, char **argv);
void                             print_unallowed_opt(t_opts_args *opts_args);
void                             print_usage(void);
void                             print_version(void);
void                             signal_exit(int signo);
void                             signal_send_packet(int signo);
uint8_t                          resolve_target_ipv4(t_ping *ping, char *target);
void                             init_ping(t_ping *ping);

/* DEBUG */
void                             print_bytes(int bytes, void *msg);

#endif
