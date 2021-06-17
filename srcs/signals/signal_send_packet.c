/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_send_packet.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 16:30:37 by arsciand          #+#    #+#             */
/*   Updated: 2021/06/17 13:14:12 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static uint16_t    icmp_checksum(void *icmp_packet)
{
    uint16_t    *buff               =   icmp_packet;
    uint32_t    sum                 =   0;
    size_t      icmp_payload_size   =   64;

    while (icmp_payload_size) {
        sum += *buff++;
        icmp_payload_size -= 2;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return (~sum);
}

void                signal_send_packet(int signo)
{
    (void)signo;
    ssize_t         bytes_sent  = 0;
    // uint64_t		j           = 0;

    ft_memset(&g_core->packet, 0, sizeof(t_icmp_packet_v4));

    /* IP Header */
    g_core->packet.iphdr.version = 4;
    g_core->packet.iphdr.ihl = IPHDR_SIZE / 4;
    g_core->packet.iphdr.tos = 0;
    g_core->packet.iphdr.tot_len = htons(PACKET_SIZE);
    g_core->packet.iphdr.id = 0;
    g_core->packet.iphdr.frag_off = htons(0);
    g_core->packet.iphdr.ttl = g_core->conf.ttl;
    g_core->packet.iphdr.protocol = IPPROTO_ICMP;
    g_core->packet.iphdr.check = 0;
    g_core->packet.iphdr.saddr = INADDR_ANY;
    g_core->packet.iphdr.daddr = ((struct sockaddr_in *)&g_core->target_addr)->sin_addr.s_addr;

    /* ICMP Payload */
    ft_memset(g_core->packet.icmp_area.message, 0x42, sizeof(g_core->packet.icmp_area.message));

    /* ICMP Header */
    g_core->packet.icmp_area.icmphdr.type = ICMP_ECHO;
    g_core->packet.icmp_area.icmphdr.un.echo.id = htons(g_core->pid);
    g_core->packet.icmp_area.icmphdr.un.echo.sequence = htons(g_core->sequence++);
    g_core->packet.icmp_area.icmphdr.checksum = icmp_checksum(&g_core->packet.icmp_area);

    bytes_sent = sendto(g_core->sockfd, &g_core->packet,
                    g_core->conf.packet_size, MSG_DONTWAIT,
                    ( struct sockaddr_in *)&g_core->target_addr,
                    sizeof(g_core->target_addr));
    if (bytes_sent == -1)
    {
        // printf("sendto(): ERROR: %s , errno %d\n", strerror(errno), errno);
        // exit_routine(FAILURE);
        g_core->errors++;
        printf("error |%d|\n", g_core->errors);
    }
    alarm(1);
}
