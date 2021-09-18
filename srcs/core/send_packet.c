/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_packet.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 16:35:34 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/18 14:22:52 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

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
    iphdr->daddr
        = ((struct sockaddr_in *)&ping->target)->sin_addr.s_addr;
}

static void     setup_payload(t_ping *ping, void *packet)
{
    char    *payload    = (char *)packet;
    size_t  payload_end = ping->conf.packet_size
                            - IPHDR_SIZE - ICMPHDR_SIZE
                            - sizeof(struct timeval);

    for (size_t i = 0; i < payload_end; i++)
        payload[i] = 0x42;
}

static void     setup_timeval(
                    void *packet, t_packet_data *packet_data,
                    struct timeval *current)
{
    ft_memcpy(packet, current, sizeof(struct timeval));
    ft_memcpy(&packet_data->time_sent, packet, sizeof(struct timeval));

}

static void     setup_icmphdr(t_ping *ping, void *packet)
{
    struct icmphdr *icmphdr     = (struct icmphdr *)packet;

    icmphdr->type               = ICMP_ECHO;
    icmphdr->un.echo.id         = htons((uint16_t)ping->conf.pid);
    icmphdr->un.echo.sequence   = htons(ping->sequence);
    icmphdr->checksum           = in_cksum(packet,
                                    ping->conf.packet_size - IPHDR_SIZE);
}

void    send_packet(t_ping *ping, struct timeval *current)
{
    ssize_t         bytes_sent  = 0;
    t_packet_data   packet_data;

    ping->sequence++;
    ft_memset(ping->packet, 0, ping->conf.packet_size);
    ft_memset(&packet_data, 0, sizeof(packet_data));
    packet_data.sequence    =   ping->sequence;
    packet_data.status      |=  PACKET_PENDING;
    setup_iphdr(ping, ping->packet);
    setup_payload(ping, ping->packet + IPHDR_SIZE + ICMPHDR_SIZE);
    setup_timeval(ping->packet + ping->conf.packet_size - sizeof(struct timeval),
        &packet_data, current);
    setup_icmphdr(ping, ping->packet + IPHDR_SIZE);

    bytes_sent = sendto(ping->sockfd, ping->packet, ping->conf.packet_size,
                    MSG_DONTWAIT, (struct sockaddr_in *)&ping->target,
                    sizeof(struct sockaddr_in));

    if (bytes_sent == -1)
        ping->errors++;
    if (!(ft_lstappend(&ping->packets,
            ft_lstnew(&packet_data, sizeof(t_packet_data)))))
        exit_routine(ping, FAILURE);
    if (ping->sequence > 0 && ping->received == 0)
        ft_memcpy(&ping->end, current, sizeof(struct timeval));
    alarm(1);
    g_ping = 0;
}
