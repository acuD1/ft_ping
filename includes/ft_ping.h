/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/26 15:25:59 by arsciand          #+#    #+#             */
/*   Updated: 2021/06/16 10:46:51 by arsciand         ###   ########.fr       */
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
int       errno;

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
# define PACKET_SIZE            64
# define TTL                    64
# define MTU                    1500

typedef struct                  s_icmp_packet_v4
{
    // struct iphdr                iphdr;
    struct icmphdr              header;
    char                        msg[PACKET_SIZE - sizeof(struct icmphdr)]; // - sizeof(struct iphdr)];
}                               t_icmp_packet_v4;

typedef struct                  s_conf
{
    int                         mtu;
    int                         packet_size;
    int                         ttl;
}                               t_conf;

typedef struct                  s_core
{
    t_opts_args                 *opts_args;
    t_conf                      conf;
    t_icmp_packet_v4            packet;
    int                         sockfd;
    volatile sig_atomic_t       sig_int;
    char                        *target_ipv4;
    uint16_t                    sequence;
    pid_t                       pid;
    struct timeval              start;
    struct timeval              end;
    struct sockaddr_storage     target_addr;
}                               t_core;

extern  t_core                  *g_core;

uint8_t                          exec_ft_ping(void);
void                             exit_routine(int8_t status);
void                             getaddrinfo_error_handler(int8_t status);
void                             free_core(void);
uint8_t                          get_opts_args_handler(int argc, char **argv);
void                             print_unallowed_opt(t_opts_args *opts_args);
void                             print_usage(void);
void                             print_version(void);
void                             signal_exit(int signo);
void                             signal_send_packet(int signo);

/* DEBUG */
void                             print_bytes(int bytes, void *msg);

#endif
