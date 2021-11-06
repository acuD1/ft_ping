/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/26 15:25:59 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/29 16:42:10 by arsciand         ###   ########.fr       */
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
# include <netinet/ip6.h>
# include <netinet/ip_icmp.h>
# include <netinet/icmp6.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <signal.h>
# include <sys/time.h>
# include <string.h>
# include <errno.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <net/if.h>

# define errno                  (*__errno_location ())

# define STRINGIZER(arg)        #arg
# define STR_VALUE(arg)         STRINGIZER(arg)
# define BUILD_VERSION_STRING   STR_VALUE(BUILDV)
# define BUILD_RELEASE_STRING   STR_VALUE(BUILDR)
# define BUILD_PATCH_STRING     STR_VALUE(BUILDP)
# define BUILD_DATE_STRING      STR_VALUE(DATE)

# define UNALLOWED_OPT          1ULL << 63
# define V_OPT                  1ULL << ('v' - 97)
# define H_OPT                  1ULL << ('h' - 97)
# define C_OPT                  1ULL << ('c' - 97)
# define D_OPT                  1ULL << ('d' - 97)
# define DD_OPT                 1ULL << ('D' - 39)
# define S_OPT                  1ULL << ('s' - 97)
# define F_OPT                  1ULL << ('f' - 97)
# define I_OPT                  1ULL << ('i' - 97)
# define L_OPT                  1ULL << ('l' - 97)
# define Q_OPT                  1ULL << ('q' - 97)
# define T_OPT                  1ULL << ('t' - 97)
# define N_OPT                  1ULL << ('n' - 97)
# define VV_OPT                 1ULL << ('V' - 39)
# define ALLOWED_OPT            "vhcdsDfilqtnV"
# define ALLOWED_OPT_ARG        "csilt"
# define ALLOWED_OPT_TAB        NULL
# define ALLOWED_OPT_TAB_ARG    NULL


# define PAYLOAD_SIZE           56
# define IPHDR_SIZE             20
# define IPV6HDR_SIZE           40
# define ICMPHDR_SIZE           8
# define TIMEVAL_SIZE           16
# define TTL                    64
# define MAX_MTU                1500

# define SEND_PACKET            0x0001
# define EXIT_PING              0x0002
# define PENDING_PACKET         0x0004

# define PACKET_RECEIVED        0x0001
# define PACKET_PENDING         0x0002

# define IPV4_MODE              2
# define IPV6_MODE              10

typedef struct                  s_conf
{
    double                      interval;
    int                         custom_iphdr;
    int                         so_debug;
    int32_t                     count;
    int32_t                     preload;
    int32_t                     timeout;
    pid_t                       pid;
    uint16_t                    payload_size;
    uint8_t                     ttl;
    uint8_t                     dns;
    uint8_t                     diff_dns;
    uint8_t                     local;
    char                        _PADDING(2);
}                               t_conf;

typedef struct                  s_ping_global
{
    volatile sig_atomic_t       sig_int;
}                               t_ping_global;

typedef struct                  s_ping_stats
{
    uint16_t                    ping_sent;
}                               t_ping_stats;

typedef struct                  s_ping_rtt
{
    double                      min;
    double                      max;
    double                      avg;
    double                      sum;
    double                      sum_square;
}                               t_ping_rtt;

typedef struct                  s_ping_ewma
{
    double                      weight;
    double                      ewma;
}                               t_ping_ewma;

typedef struct                  s_ancillary_data
{
    uint8_t                     ttl;
}                               t_ancillary_data;


typedef struct                  s_packet_data
{
    uint16_t                    sequence;
    uint8_t                     status;
    t_ancillary_data            ancillary_data;
    char                        _PADDING(4);
    struct timeval              time_sent;
    struct timeval              time_recv;
}                               t_packet_data;

typedef struct                  s_ping
{
    t_lst                      *packets;
    char                        *packet;
    uint64_t                    opts;
    size_t                      packet_size;
    int                         sockfd;
    uint16_t                    sequence;
    uint16_t                    received;
    uint16_t                    errors;
    uint16_t                     pipe;
    uint8_t                     mode;
    char                        buff_ip[INET6_ADDRSTRLEN];
    char                        buff_dns[NI_MAXHOST];
    char                        buff_target[NI_MAXHOST];
    char                        _PADDING(3);
    t_conf                      conf;
    struct timeval              start;
    struct timeval              end;
    struct timeval              last_send;
    struct sockaddr_storage     target;
}                               t_ping;

extern volatile sig_atomic_t    g_ping;

double                          calc_latency(void *t_start, void *t_end);
double                          calc_mdev(t_ping *ping, t_ping_rtt *ping_rtt);
uint8_t                         fetch_ping_rtt(void *content, void *context);
double                          calc_packet_loss(t_ping *ping);
uint8_t                         exec_ping(t_ping *ping);
void                            exit_routine(t_ping *ping, int8_t status);
void                            getaddrinfo_error_handler(
                                    char *target, int status);
void                            free_ping(t_ping *ping);
uint8_t                         set_opts_args(
                                    t_ping *ping, int argc, char **argv);
void                            print_unallowed_opt(t_opts_args *opts_args);
void                            print_usage(void);
void                            print_version(void);
uint8_t                         resolve_target(
                                    t_ping *ping, char *target);
void                            init_ping(t_ping *ping);
void                            print_init_handler(t_ping *ping);
void                            sig_handler(int signo);
void                            send_packet(
                                    t_ping *ping, struct timeval *current);
void                            setup_socket(t_ping *ping);
void                            gettimeofday_handler(t_ping *ping, void *time);
void                            icmp_error_handler(
                                    uint8_t type, uint8_t code,
                                    uint16_t sequence, char *source);
t_packet_data                   *recv_packet(
                                    t_ping *ping, char *buffer,
                                    ssize_t *bytes_recv,
                                    struct timeval *time_recv);
t_packet_data                   *validate_packet(
                                    t_ping *ping, ssize_t icmp_area_size,
                                    struct timeval *time_received,
                                    void *icmp_area);
char                            *inet_ntop_handler(t_ping *ping, uint32_t *addr);
void                            display_stats(t_ping *ping, t_ping_rtt *ping_rtt, t_ping_ewma *ping_ewma);
void                            fetch_stats(t_ping *ping);
void                            display_recv(
                                    t_ping *ping, void *buffer,
                                    t_packet_data *packet_data,
                                    ssize_t *bytes_received);
uint16_t                        in_cksum(void *buffer, size_t len);
void                            display_unowned(t_ping *ping, void *buffer, ssize_t *bytes_received);
void                            getnameinfo_error_handler(t_ping *ping, int status);
uint8_t                         inet_pton_handler(t_ping *ping, char *target);
void                            getnameinfo_handler(t_ping *ping);
void                            *find_ancillary_data(struct msghdr *msghdr, int cmsg_type);
void                            icmp6_error_handler(
                                    uint8_t type, uint8_t code, uint16_t sequence, char *source);


/* DEBUG */
void                            print_bytes(int bytes, void *msg);
void                            print_time(void *time);
void                            debug_packets(void *content);

#endif
