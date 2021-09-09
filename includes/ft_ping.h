/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/26 15:25:59 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/09 14:12:32 by arsciand         ###   ########.fr       */
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
# define ICMPHDR_SIZE           8
# define TTL                    64
# define MAX_MTU                1500
# define START_SEQUENCE         1

# define SEND_PACKET            0x0001
# define EXIT_PING              0x0002

typedef struct                  s_conf
{
    int                         custom_iphdr;
    pid_t                       pid;
    uint16_t                    packet_size;
    uint8_t                      ttl;
    char                        _PADDING(1);
}                               t_conf;

typedef struct                  s_ping_global
{
    volatile sig_atomic_t       sig_int;
}                               t_ping_global;

typedef struct                  s_packet_data
{
    uint16_t                    sequence;
    uint8_t                     status;
    char                        _PADDING(5);
    struct timeval              time_sent;
    struct timeval              time_received;
}                               t_packet_data;

typedef struct                  s_ping
{
    t_lst                      *packets;
    int                         sockfd;
    uint16_t                    sequence;
    uint16_t                    errors;
    t_conf                      conf;
    char                        _PADDING(4);
    struct timeval              start;
    struct timeval              end;
    struct sockaddr_storage     target;
}                               t_ping;

extern volatile sig_atomic_t    g_ping;

uint8_t                          exec_ping(t_ping *ping);
void                             exit_routine(t_ping *ping, int8_t status);
void                             getaddrinfo_error_handler(char *target, int status);
void                             free_ping(t_ping *ping);
uint8_t                          set_opts_args(t_ping *ping, int argc, char **argv);
void                             print_unallowed_opt(t_opts_args *opts_args);
void                             print_usage(void);
void                             print_version(void);
uint8_t                          resolve_target_ipv4(t_ping *ping, char *target);
void                             init_ping(t_ping *ping);
void                             print_init(t_ping *ping);
void                             sig_handler(int signo);
void                             send_packet(t_ping *ping, char *packet);
void                             setup_socket(t_ping *ping);
void                             gettimeofday_handler(t_ping *ping, void *time);

/* DEBUG */
void                             print_bytes(int bytes, void *msg);
void                             print_time(void *time);
void                            debug_packets(void *content);

#endif
