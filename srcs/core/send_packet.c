/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_packet.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 16:35:34 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/30 19:47:39 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static void     resolve_source(t_ping *ping, struct sockaddr_in6 *source)
{
    struct ifaddrs  *ifap   = NULL;

    if (getifaddrs(&ifap) == -1)
    {
        printf("[DEBUG] getifaddrs(): ERROR: %s , errno %d\n", strerror(errno), errno);
        exit_routine(ping, FAILURE);
    }

    #pragma clang diagnostic ignored "-Wcast-align"

    for (struct ifaddrs *ifa = ifap; ifa; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET6
            && !(ifa->ifa_flags & (IFF_LOOPBACK))
            && (ifa->ifa_flags & (IFF_RUNNING)))
        {
            source->sin6_addr = ((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            break ;
        }
    }

    freeifaddrs(ifap);
}

static void     setup_iphdr(t_ping *ping, void *packet)
{
    if (ping->mode == IPV4_MODE)
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
    else
    {
        struct ip6_hdr *ip6_hdr = (struct ip6_hdr *)packet;
        struct sockaddr_in6 source;

        resolve_source(ping, &source);

        ft_memset(ip6_hdr, 0, sizeof(struct ip6_hdr));
        ip6_hdr->ip6_ctlun.ip6_un1.ip6_un1_flow = htonl((6 << 28) | (0 << 20) | 0);
        ip6_hdr->ip6_ctlun.ip6_un1.ip6_un1_plen = htons(ping->conf.payload_size + ICMPHDR_SIZE);
        ip6_hdr->ip6_ctlun.ip6_un1.ip6_un1_nxt = IPPROTO_ICMPV6;
        ip6_hdr->ip6_ctlun.ip6_un1.ip6_un1_hlim = 64;
        ip6_hdr->ip6_src = source.sin6_addr;
        ip6_hdr->ip6_dst = ((struct sockaddr_in6 *)&ping->target)->sin6_addr;
    }
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
    icmphdr->checksum           = in_cksum(packet, ping->packet_size - IPHDR_SIZE);
}

void        set_buff_ip6_hdr(char *buff_checksum, struct ip6_hdr *ip6_hdr)
{
    ft_memcpy(buff_checksum, &ip6_hdr->ip6_src, 16);
    ft_memcpy(buff_checksum + 16, &ip6_hdr->ip6_dst, 16);
    ft_memcpy(buff_checksum + 34, &ip6_hdr->ip6_ctlun.ip6_un1.ip6_un1_plen, 2);
    ft_memcpy(buff_checksum + 39, &ip6_hdr->ip6_ctlun.ip6_un1.ip6_un1_nxt, 1);
}

void        set_buff_icmp6_hdr(char *buff_checksum, struct icmp6_hdr *icmp6_hdr)
{
    ft_memcpy(buff_checksum, &icmp6_hdr->icmp6_type, 1);
    ft_memcpy(buff_checksum + 1, &icmp6_hdr->icmp6_code, 1);
    ft_memcpy(buff_checksum + 2, &icmp6_hdr->icmp6_id, 2);
    ft_memcpy(buff_checksum + 4, &icmp6_hdr->icmp6_seq, 2);
}

void        set_buff_icmp6_data(char *buff_checksum, uint8_t *data, size_t size)
{
    ft_memcpy(buff_checksum, data, size);
}

uint16_t    icmp6_checksum(t_ping *ping, void *packet)
{
    char    buff_checksum[104];


    ft_memset(buff_checksum, 0, 104);

    set_buff_ip6_hdr(buff_checksum, (struct ip6_hdr *)packet);
    set_buff_icmp6_hdr(buff_checksum + 40, (struct icmp6_hdr *)(packet + 40));
    set_buff_icmp6_data(buff_checksum + 40 + 8, (uint8_t *)(packet + 40 + ICMPHDR_SIZE),
                        ping->conf.payload_size);
    return (in_cksum(buff_checksum, 40 + 8 + ping->conf.payload_size));
}

static void     setup_icmp6hdr(t_ping *ping, void *packet)
{
    struct icmp6_hdr *icmp6_hdr     = (struct icmp6_hdr *)packet;

    icmp6_hdr->icmp6_type           = ICMP6_ECHO_REQUEST;
    icmp6_hdr->icmp6_seq            = htons(ping->sequence);
    icmp6_hdr->icmp6_id             = htons((uint16_t)ping->conf.pid);
    icmp6_hdr->icmp6_cksum          = icmp6_checksum(ping, ping->packet);
}

void    send_packet(t_ping *ping, struct timeval *current)
{
    ssize_t         bytes_sent  = 0;
    t_packet_data   packet_data;

    ping->sequence++;
    ft_memset(ping->packet, 0, ping->packet_size);
    ft_memset(&packet_data, 0, sizeof(packet_data));
    packet_data.sequence    =   ping->sequence;
    packet_data.status      |=  PACKET_PENDING;
    setup_iphdr(ping, ping->packet);
    if (ping->mode == IPV4_MODE)
    {
        if (ping->conf.payload_size >= TIMEVAL_SIZE)
            setup_timeval(ping->packet + IPHDR_SIZE + ICMPHDR_SIZE,
                &packet_data, current);
        setup_payload(ping, ping->packet + IPHDR_SIZE + ICMPHDR_SIZE);
        setup_icmphdr(ping, ping->packet + IPHDR_SIZE);
        bytes_sent = sendto(ping->sockfd, ping->packet,
                        ping->conf.payload_size + IPHDR_SIZE + ICMPHDR_SIZE,
                        MSG_DONTWAIT, (struct sockaddr_in *)&ping->target,
                        sizeof(struct sockaddr_in));
    }
    else
    {
        if (ping->conf.payload_size >= TIMEVAL_SIZE)
            setup_timeval(ping->packet + IPV6HDR_SIZE + ICMPHDR_SIZE,
                &packet_data, current);
        setup_payload(ping, ping->packet + IPV6HDR_SIZE + ICMPHDR_SIZE);
        setup_icmp6hdr(ping, ping->packet + IPV6HDR_SIZE);
        bytes_sent = sendto(ping->sockfd, ping->packet,
                ping->packet_size,
                MSG_DONTWAIT, (struct sockaddr_in6 *)&ping->target,
                sizeof(struct sockaddr_in6));
    }

    if (bytes_sent == -1)
    {
        printf("Ping error\n");
        ping->errors++;
    }
    if (!(ft_lstappend(&ping->packets,
            ft_lstnew(&packet_data, sizeof(t_packet_data)))))
        exit_routine(ping, FAILURE);
    if (ping->sequence > 0 && ping->received == 0)
        ft_memcpy(&ping->end, current, TIMEVAL_SIZE);
    if (ping->opts & C_OPT)
        ping->conf.count--;
}
