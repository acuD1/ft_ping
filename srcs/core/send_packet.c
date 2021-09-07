/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_packet.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 16:35:34 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/07 16:36:36 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static uint16_t in_cksum(void *buffer, size_t len)
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

static void     setup_iphdr(t_ping *ping, void *packet)
{
    struct iphdr *iphdr = (struct iphdr *)packet;

    iphdr->version      = 4;
    iphdr->ihl          = IPHDR_SIZE / 4;
    iphdr->tos          = 0;
    iphdr->tot_len      = htons(ping->conf.packet_size);
    iphdr->id           = 0;
    iphdr->frag_off     = htons(0);
    iphdr->ttl          = ping->conf.ttl;
    iphdr->protocol     = IPPROTO_ICMP;
    iphdr->check        = 0;
    iphdr->saddr        = INADDR_ANY;
    iphdr->daddr        = ((struct sockaddr_in *)&ping->target)->sin_addr.s_addr;
}

static void     setup_payload(t_ping *ping, void *packet)
{
    char *payload = (char *)packet;

    for (size_t i = 0; i < ping->conf.packet_size - IPHDR_SIZE - ICMPHDR_SIZE - sizeof(struct timeval); i++)
        payload[i] = 0x42;
}

static void     setup_timeval(t_ping *ping, void *packet)
{
    gettimeofday_handler(ping, packet);
}

static void     setup_icmphdr(t_ping *ping, void *packet)
{
    static uint16_t id          = 42000;
    struct icmphdr *icmphdr     = (struct icmphdr *)packet;

    icmphdr->type               = ICMP_ECHO;
    icmphdr->un.echo.id         = htons(id++);
    icmphdr->un.echo.sequence   = htons(ping->sequence++);
    icmphdr->checksum           = in_cksum(packet, ping->conf.packet_size - IPHDR_SIZE);
}

void    send_packet(t_ping *ping, char *packet)
{
    ssize_t             bytes_sent  = 0;

    ft_memset(packet, 0, ping->conf.packet_size);
    setup_iphdr(ping, packet);
    setup_payload(ping, packet + IPHDR_SIZE + ICMPHDR_SIZE);
    setup_timeval(ping, packet + ping->conf.packet_size - sizeof(struct timeval));
    setup_icmphdr(ping, packet + IPHDR_SIZE);

    dprintf(STDERR_FILENO, "[DEBUG] packet_size |%d|\n", ping->conf.packet_size);
    print_bytes(ping->conf.packet_size, packet);
    print_time(packet + ping->conf.packet_size - sizeof(struct timeval));

    bytes_sent = sendto(ping->sockfd, packet, ping->conf.packet_size, MSG_DONTWAIT,
                    (struct sockaddr_in *)&ping->target, sizeof(struct sockaddr_in));
    if (bytes_sent == -1)
    {
        ping->errors++;
    }
}
