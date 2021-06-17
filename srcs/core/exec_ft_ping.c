/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_ft_ping.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/29 14:30:22 by arsciand          #+#    #+#             */
/*   Updated: 2021/06/17 14:17:25 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

// uint8_t     is_owned_packet(void *packet_buffer)
// {
//     struct icmp *icmp = packet_buffer + sizeof(struct ip);

//     printf("icmp->icmp_hun.ih_idseq.icd_id |%d| |%d|\n", icmp->icmp_hun.ih_idseq.icd_id, htons(g_core->pid));
//     return (icmp->icmp_hun.ih_idseq.icd_id == htons(g_core->pid) ? TRUE : FALSE);
// }

uint8_t     is_packet_valid(void *packet_buffer)
{
    t_icmp_packet_v4    *received_packet = packet_buffer;
    char                buff_ipv4[INET_ADDRSTRLEN];
;

    // printf("\n------- icmp->type = %d\n\n", received_packet->icmp_area.icmphdr.type);
    // ft_memset(&buff_ipv4, 0, INET_ADDRSTRLEN);
    // inet_ntop(AF_INET, &((struct iphdr *)packet_buffer)->saddr, buff_ipv4, sizeof(buff_ipv4));
    // fprintf(stderr, "From %s icmp_seq=%hu %s\n", buff_ipv4, g_core->sequence, "TEST");
    // ft_memset(&buff_ipv4, 0, INET_ADDRSTRLEN);
    // inet_ntop(AF_INET, &((struct iphdr *)packet_buffer)->daddr, buff_ipv4, sizeof(buff_ipv4));
    // fprintf(stderr, "To %s icmp_seq=%hu %s\n", buff_ipv4, g_core->sequence, "TEST");

    // if (is_owned_packet(packet_buffer) != TRUE)
    //     return (FALSE);
    struct icmp *icmp = packet_buffer + 20;
    printf("packet chksum: %d\n", icmp->icmp_hun.ih_idseq.icd_id);
    if (received_packet->icmp_area.icmphdr.type != ICMP_ECHOREPLY)
    {
        ft_memset(&buff_ipv4, 0, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &received_packet->iphdr.saddr, buff_ipv4, sizeof(buff_ipv4));
        fprintf(stderr, "From %s icmp_seq=%hu %s\n", buff_ipv4, g_core->sequence, "TEST");
    }
    return (TRUE);
}

void        analyse_packet(void *packet_buffer, char *control_buffer, ssize_t bytes_received)
{
    (void)packet_buffer;
    (void)bytes_received;
    (void)control_buffer;

    // printf("Analysing ...\n");
}

uint8_t     exec_ft_ping(void)
{
    char                    packet_buffer[g_core->conf.mtu];
    char                    control_buffer[1024];
    ssize_t                 bytes_received = 0;
    struct sockaddr_storage tmp;
    struct msghdr           target_msg;
    struct iovec            iov[1];

    printf("PING %s %lu(%d) bytes of data.\n", g_core->target_ipv4,
        g_core->conf.packet_size - sizeof(struct icmphdr) - IPHDR_SIZE,
        g_core->conf.packet_size);

    signal_send_packet(SIGALRM);

    while (1)
    {
        ft_memset(&packet_buffer, 0, sizeof(g_core->conf.packet_size + sizeof(struct ip)));
        ft_memset(&control_buffer, 0, sizeof(control_buffer));
        ft_memset(&tmp, 0, sizeof(struct sockaddr_storage));
        ft_memset(&target_msg, 0, sizeof(struct msghdr));
        ft_memset(&iov, 0, sizeof(struct iovec));

        /* Filling up msghdr structure */
        iov[0].iov_len = sizeof(packet_buffer);
        iov[0].iov_base = &packet_buffer;
        target_msg.msg_iov = iov;
        target_msg.msg_iovlen = 1;
        ft_memcpy(&tmp, &g_core->target_addr, sizeof(struct sockaddr_storage));
        target_msg.msg_name = (struct sockaddr_in *)&tmp;
        target_msg.msg_namelen = sizeof((struct sockaddr_in *)&tmp);
        target_msg.msg_flags = 0;
        target_msg.msg_control = &control_buffer;
        target_msg.msg_controllen = sizeof(control_buffer);
        bytes_received = recvmsg(g_core->sockfd, &target_msg, 0);
        if (bytes_received == -1)
        {
            printf("recvmsg(): ERROR: %s , errno %d\n -> s |%zd|\n", strerror(errno), errno, bytes_received);
            exit_routine(FAILURE);
        }

        // if (is_owned_packet(packet_buffer))
        // {
            if (is_packet_valid(packet_buffer) == TRUE)
            {
                printf("seq: %d\n", g_core->sequence);
                analyse_packet(packet_buffer, control_buffer, bytes_received);
                printf("Reveived ...|%zu|\n\n", bytes_received);
            }
        // }
    }
    return (SUCCESS);
}
