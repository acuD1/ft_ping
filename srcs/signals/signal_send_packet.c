/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_send_packet.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 16:30:37 by arsciand          #+#    #+#             */
/*   Updated: 2021/06/15 14:56:49 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static uint16_t    icmp_checksum(void *icmp_packet)
{
    uint16_t    *buff       =   icmp_packet;
    uint32_t    sum         =   0;
    size_t      packet_size =   g_core->conf.packet_size;

    while (packet_size) {
        sum += *buff++;
        packet_size -= 2;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return (~sum);
}

void                signal_send_packet(int signo)
{
    (void)signo;
    ssize_t         bytes_sent  = 0;
    uint64_t		j           = 0;

    ft_memset(&g_core->packet, 0, sizeof(t_icmp_packet_v4));

    /* ICMP Payload */
    while (j < g_core->conf.packet_size - sizeof(struct icmphdr) - 1)
        g_core->packet.msg[j++] = 0x42;
    g_core->packet.msg[j] = '\0';

    /* ICMP Header */
    g_core->packet.header.type = ICMP_ECHO;
    g_core->packet.header.un.echo.id = g_core->pid;
    g_core->packet.header.un.echo.sequence = g_core->sequence++;
    g_core->packet.header.checksum = icmp_checksum(&g_core->packet);

    bytes_sent = sendto(g_core->sockfd, &g_core->packet,
                    g_core->conf.packet_size, MSG_DONTWAIT,
                    (const struct sockaddr *)&g_core->target_addr,
                    sizeof(g_core->target_addr));
    printf("Sending ... |%zu|\n", bytes_sent);
    // if (bytes_sent == -1)
    // {
    //     printf("sendto(): ERROR: %s , errno %d\n", strerror(errno), errno);
    //     exit_routine(FAILURE);
    // }
    alarm(1);
}
