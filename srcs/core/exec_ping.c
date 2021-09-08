/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_ping.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/29 14:30:22 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/08 17:50:50 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

// // uint8_t     is_owned_packet(void *packet_buffer)
// // {
// //     struct icmp *icmp = packet_buffer + sizeof(struct ip);

// //     printf("icmp->icmp_hun.ih_idseq.icd_id |%d| |%d|\n", icmp->icmp_hun.ih_idseq.icd_id, htons(g_core->pid));
// //     return (icmp->icmp_hun.ih_idseq.icd_id == htons(g_core->pid) ? TRUE : FALSE);
// // }

// uint8_t     is_packet_valid(void *packet_buffer)
// {
//     t_icmp_packet_v4    *received_packet = packet_buffer;
//     char                buff_ipv4[INET_ADDRSTRLEN];
// ;

//     // printf("\n------- icmp->type = %d\n\n", received_packet->icmp_area.icmphdr.type);
//     // ft_memset(&buff_ipv4, 0, INET_ADDRSTRLEN);
//     // inet_ntop(AF_INET, &((struct iphdr *)packet_buffer)->saddr, buff_ipv4, sizeof(buff_ipv4));
//     // fprintf(stderr, "From %s icmp_seq=%hu %s\n", buff_ipv4, g_core->sequence, "TEST");
//     // ft_memset(&buff_ipv4, 0, INET_ADDRSTRLEN);
//     // inet_ntop(AF_INET, &((struct iphdr *)packet_buffer)->daddr, buff_ipv4, sizeof(buff_ipv4));
//     // fprintf(stderr, "To %s icmp_seq=%hu %s\n", buff_ipv4, g_core->sequence, "TEST");

//     // if (is_owned_packet(packet_buffer) != TRUE)
//     //     return (FALSE);
//     struct icmp *icmp = packet_buffer + 20;
//     printf("packet chksum: %d\n", icmp->icmp_hun.ih_idseq.icd_id);
//     if (received_packet->icmp_area.icmphdr.type != ICMP_ECHOREPLY)
//     {
//         ft_memset(&buff_ipv4, 0, INET_ADDRSTRLEN);
//         inet_ntop(AF_INET, &received_packet->iphdr.saddr, buff_ipv4, sizeof(buff_ipv4));
//         fprintf(stderr, "From %s icmp_seq=%hu %s\n", buff_ipv4, g_core->sequence, "TEST");
//     }
//     return (TRUE);
// }

// void        analyse_packet(void *packet_buffer, char *control_buffer, ssize_t bytes_received)
// {
//     (void)packet_buffer;
//     (void)bytes_received;
//     (void)control_buffer;

//     // printf("Analysing ...\n");
// }

static uint8_t packet_found(ssize_t payload_size, void *payload, uint16_t sequence)
{
    (void)payload_size;
    (void)payload;
    (void)sequence;
    return (SUCCESS);
}

static void     analyse_packet(t_ping *ping, ssize_t bytes_received, struct timeval *time_received, void *buffer)
{
    (void)ping;
    (void)bytes_received;
    (void)time_received;
    struct icmphdr *response = (struct icmphdr *)(buffer);

    #ifdef DEBUG
        dprintf(STDERR_FILENO, "[DEBUG] icmphdr->type |%s|\n", response->type == ICMP_ECHOREPLY ? "ICMP_REPLY" : ft_itoa(response->type));
    #endif

    if (response->type == ICMP_ECHOREPLY && htons(response->un.echo.id) == ping->conf.pid && htons(response->un.echo.sequence) <= ping->sequence
        && packet_found(bytes_received - (ssize_t)(IPHDR_SIZE - ICMPHDR_SIZE), (char *)buffer + ICMPHDR_SIZE, response->un.echo.sequence) == SUCCESS)
    {
        #ifdef DEBUG
            dprintf(STDERR_FILENO, "[DEBUG] icmphdr->un.ech.sequence |%hu|\n", htons(response->un.echo.sequence));
            dprintf(STDERR_FILENO, "[DEBUG] icmphdr->un.ech.id |%hu|\n", htons(response->un.echo.id));
            print_bytes((int)bytes_received - IPHDR_SIZE - ICMPHDR_SIZE, (char *)buffer + ICMPHDR_SIZE);
            print_time((char *)buffer + ((uint64_t)bytes_received - sizeof(struct timeval) - IPHDR_SIZE));
            dprintf(STDERR_FILENO, "[DEBUG] TIME SEC |%lu| USEC |%lu| (RECEIVED) \n", time_received->tv_sec, time_received->tv_usec);
        #endif
    }

    #ifdef DEBUG
        if (response->type != ICMP_ECHOREPLY || htons(response->un.echo.id) != ping->conf.pid || htons(response->un.echo.sequence) > ping->sequence)
            dprintf(STDERR_FILENO, "\n/!\\\n[DEBUG] Wrong packet !!!\n/!\\\n\n");
    #endif
}

static void     retrieve_response(t_ping *ping)
{
    ssize_t                 bytes_received  = 0;
    char                    buffer[MAX_MTU];
    char                    control_buffer[1000];
    struct sockaddr_storage tmp;
    struct msghdr           msghdr;
    struct iovec            msg_iov[1];
    struct timeval          time_received;

    ft_memset(&buffer, 0, sizeof(buffer));
    ft_memset(&control_buffer, 0, sizeof(control_buffer));
    ft_memset(&msghdr, 0, sizeof(msghdr));
    ft_memset(&tmp, 0, sizeof(struct sockaddr_storage));
    ft_memset(msg_iov, 0, sizeof(msg_iov));

    msg_iov->iov_base   = buffer;
    msg_iov->iov_len    = sizeof(buffer);
    ft_memcpy(&tmp, &ping->target, sizeof(struct sockaddr_storage));
    msghdr.msg_name = (struct sockaddr_in *)&tmp;
    msghdr.msg_namelen = sizeof((struct sockaddr_in *)&tmp);
    msghdr.msg_iov      = msg_iov;
    msghdr.msg_iovlen   = 1;
    msghdr.msg_flags    = 0;

    bytes_received = recvmsg(ping->sockfd, &msghdr, MSG_DONTWAIT);
    if (bytes_received != -1)
    {
        gettimeofday_handler(ping, &time_received);
        #ifdef DEBUG
            dprintf(STDERR_FILENO, "---\n[DEBUG] bytes_received |%zu|\n", bytes_received);
            print_bytes((int)bytes_received, &buffer);
        #endif

        analyse_packet(ping, bytes_received, &time_received, buffer + IPHDR_SIZE);
    }
}

uint8_t         exec_ping(t_ping *ping)
{
    char    *packet = NULL;

    if (!(packet = ft_memalloc(ping->conf.packet_size)))
        exit_routine(ping, FAILURE);

    print_init(ping);

    setup_socket(ping);

    send_packet(ping, packet);
    while (1)
    {
        if (g_ping & SEND_PACKET)
        {
            send_packet(ping, packet);
            g_ping = 0;
        }
        if (g_ping & EXIT_PING)
        {

            #ifdef DEBUG
                dprintf(STDERR_FILENO, "[DEBUG] EXIT_PING\n");
            #endif

            break ;
        }
        retrieve_response(ping);

    }
    ft_strdel(&packet);
    // fetch_responses(ping);
    return (SUCCESS);
}
