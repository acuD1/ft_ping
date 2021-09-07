/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_packet.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 16:35:34 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/07 12:09:11 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static uint16_t    in_cksum(void *buffer, size_t len)
{
	uint16_t *tmp   = (uint16_t *)buffer;
	uint32_t sum    = 0;

	while (len > 1)
	{
		sum += *tmp++;
		len -= 2;
	}

	if (len > 0)
		sum += *(uint8_t *)tmp;

	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);

	return ((uint16_t)~sum);
}

void    send_packet(t_ping *ping, char *payload)
{
    ssize_t             bytes_sent  = 0;
    static uint16_t     id          = 42000;
    t_icmp_packet_v4    packet;

    ft_memset(&packet, 0, sizeof(t_icmp_packet_v4));

    dprintf(STDERR_FILENO, "[DEBUG] packet_size |%d|\n", ping->conf.packet_size);

    /* IP Header */
    packet.iphdr.version    = 4;
    packet.iphdr.ihl        = IPHDR_SIZE / 4;
    packet.iphdr.tos        = 0;
    packet.iphdr.tot_len    = htons(ping->conf.packet_size);
    packet.iphdr.id         = 0;
    packet.iphdr.frag_off   = htons(0);
    packet.iphdr.ttl        = ping->conf.ttl;
    packet.iphdr.protocol   = IPPROTO_ICMP;
    packet.iphdr.check      = 0;
    packet.iphdr.saddr      = INADDR_ANY;
    packet.iphdr.daddr      = ((struct sockaddr_in *)&ping->target)->sin_addr.s_addr;

    /* ICMP Payload */
    // ft_memcpy(&packet.icmp_area.message, &payload, sizeof(payload));
    (void)payload;
    ft_memset(&packet.icmp_area.message, 0x42, 56 - 1);
    dprintf(STDERR_FILENO, "[DEBUG] payload |%s| |%zu|\n", packet.icmp_area.message, ft_strlen(packet.icmp_area.message));

    /* ICMP Header */
    packet.icmp_area.icmphdr.type               = ICMP_ECHO;
    packet.icmp_area.icmphdr.un.echo.id         = htons(id++);
    packet.icmp_area.icmphdr.un.echo.sequence   = htons(ping->sequence++);
    packet.icmp_area.icmphdr.checksum           = in_cksum(&packet.icmp_area, 8 + 56);


    bytes_sent = sendto(ping->sockfd, &packet,
                    ping->conf.packet_size, MSG_DONTWAIT,
                    (struct sockaddr_in *)&ping->target,
                    sizeof(struct sockaddr_in));
    if (bytes_sent == -1)
    {
        printf("sendto(): ERROR: %s , errno %d\n", strerror(errno), errno);
        // exit_routine(FAILURE);
        ping->errors++;
        printf("[DEBUG] error |%d|\n", ping->errors);
    }


}
//     ssize_t         bytes_sent  = 0;
//     // uint64_t		j           = 0;

//     ft_memset(&g_core->packet, 0, sizeof(t_icmp_packet_v4));

//     /* IP Header */
//     g_core->packet.iphdr.version = 4;
//     g_core->packet.iphdr.ihl = IPHDR_SIZE / 4;
//     g_core->packet.iphdr.tos = 0;
//     g_core->packet.iphdr.tot_len = htons(PACKET_SIZE);
//     g_core->packet.iphdr.id = 0;
//     g_core->packet.iphdr.frag_off = htons(0);
//     g_core->packet.iphdr.ttl = g_core->conf.ttl;
//     g_core->packet.iphdr.protocol = IPPROTO_ICMP;
//     g_core->packet.iphdr.check = 0;
//     g_core->packet.iphdr.saddr = INADDR_ANY;
//     g_core->packet.iphdr.daddr = ((struct sockaddr_in *)&g_core->target_addr)->sin_addr.s_addr;

//     /* ICMP Payload */
//     ft_memset(g_core->packet.icmp_area.message, 0x42, sizeof(g_core->packet.icmp_area.message));

//     /* ICMP Header */
//     g_core->packet.icmp_area.icmphdr.type = ICMP_ECHO;
//     g_core->packet.icmp_area.icmphdr.un.echo.id = htons(g_core->pid);
//     g_core->packet.icmp_area.icmphdr.un.echo.sequence = htons(g_core->sequence++);
//     g_core->packet.icmp_area.icmphdr.checksum = icmp_checksum(&g_core->packet.icmp_area);

//     bytes_sent = sendto(g_core->sockfd, &g_core->packet,
//                     g_core->conf.packet_size, MSG_DONTWAIT,
//                     ( struct sockaddr_in *)&g_core->target_addr,
//                     sizeof(g_core->target_addr));
//     if (bytes_sent == -1)
//     {
//         // printf("sendto(): ERROR: %s , errno %d\n", strerror(errno), errno);
//         // exit_routine(FAILURE);
//         g_core->errors++;
//         printf("error |%d|\n", g_core->errors);
//     }
