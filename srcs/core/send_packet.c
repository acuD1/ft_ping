/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_packet.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 16:35:34 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/24 16:53:00 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static void     setup_iphdr(t_ping *ping, void *packet)
{
    struct iphdr *iphdr = (struct iphdr *)packet;

    iphdr->version      = 4;
    iphdr->ihl          = IPHDR_SIZE / 4;
    iphdr->tos          = 0;
    iphdr->tot_len      = htons(ping->conf.payload_size
                            + IPHDR_SIZE + ICMPHDR_SIZE);
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
    char    *payload            = (char *)packet;
    size_t  custom_payload_size = 0;

    if (ping->conf.payload_size >= TIMEVAL_SIZE)
        custom_payload_size = TIMEVAL_SIZE;
    for (size_t i = custom_payload_size; i < ping->conf.payload_size; i++)
        payload[i] = 0x42;
}

static void     setup_timeval(
                    void *packet, t_packet_data *packet_data,
                    struct timeval *current)
{
    ft_memcpy(packet, current, TIMEVAL_SIZE);
    ft_memcpy(&packet_data->time_sent, packet, TIMEVAL_SIZE);

}

static void     setup_icmphdr(t_ping *ping, void *packet)
{
    struct icmphdr *icmphdr     = (struct icmphdr *)packet;

    icmphdr->type               = ICMP_ECHO;
    icmphdr->un.echo.id         = htons((uint16_t)ping->conf.pid);
    icmphdr->un.echo.sequence   = htons(ping->sequence);
    icmphdr->checksum           = in_cksum(packet,
                                    ping->conf.payload_size + ICMPHDR_SIZE);
}

void    send_packet(t_ping *ping, struct timeval *current)
{
    ssize_t         bytes_sent  = 0;
    t_packet_data   packet_data;

    ping->sequence++;
    ft_memset(ping->packet, 0,
        ping->conf.payload_size + IPHDR_SIZE + ICMPHDR_SIZE);
    ft_memset(&packet_data, 0, sizeof(packet_data));
    packet_data.sequence    =   ping->sequence;
    packet_data.status      |=  PACKET_PENDING;
    setup_iphdr(ping, ping->packet);
    if (ping->conf.payload_size >= TIMEVAL_SIZE)
        setup_timeval(ping->packet + IPHDR_SIZE + ICMPHDR_SIZE,
            &packet_data, current);
    setup_payload(ping, ping->packet + IPHDR_SIZE + ICMPHDR_SIZE);
    setup_icmphdr(ping, ping->packet + IPHDR_SIZE);

    bytes_sent = sendto(ping->sockfd, ping->packet,
                    ping->conf.payload_size + IPHDR_SIZE + ICMPHDR_SIZE,
                    MSG_DONTWAIT, (struct sockaddr_in *)&ping->target,
                    sizeof(struct sockaddr_in));

    if (bytes_sent == -1)
        ping->errors++;
    if (!(ft_lstappend(&ping->packets,
            ft_lstnew(&packet_data, sizeof(t_packet_data)))))
        exit_routine(ping, FAILURE);
    if (ping->sequence > 0 && ping->received == 0)
        ft_memcpy(&ping->end, current, TIMEVAL_SIZE);
    if (ping->opts & C_OPT)
        ping->conf.count--;
}
