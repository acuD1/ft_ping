/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   recv_packet.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/12 11:48:32 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/27 18:05:21 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static void icmp6_error_handler(uint8_t type, uint8_t code, uint16_t sequence, char *source)
{
    (void)type;
    (void)code;
    (void)sequence;
    (void)source;
    dprintf(2, "ICMP6 Error: %s\n", source);
}

static void             display_icmp_error(
                            t_ping *ping, void *buffer, void *icmp_area,
                            uint16_t sequence)
{
    static uint16_t last_seq   = 0;

    switch (ping->mode)
    {
        case IPV4_MODE:
        {
            struct iphdr    *iphdr     = (struct iphdr *)buffer;
            struct icmphdr  *icmphdr   = (struct icmphdr *)icmp_area;

            if (icmphdr->type != ICMP_ECHO && last_seq != sequence)
            {
                inet_ntop_handler(ping, &iphdr->saddr);
                last_seq = sequence;
                ping->errors++;
                if (ping->opts & F_OPT)
                    dprintf(STDERR_FILENO, "E");
                else
                    icmp_error_handler(icmphdr->type, icmphdr->code, sequence,
                        ping->buff_ip);
            }
            break;
        }

        case IPV6_MODE:
        {
            struct ip6_hdr  *ip6_hdr    = (struct ip6_hdr *)buffer;
            struct icmp6_hdr *icmp6_hdr  = (struct icmp6_hdr *)icmp_area;

            if (icmp6_hdr->icmp6_type != ICMP6_ECHO_REQUEST &&
                last_seq != sequence)
            {
                dprintf(STDERR_FILENO, "???\n");
                inet_ntop_handler(ping, (uint32_t *)&ip6_hdr->ip6_src);
                last_seq = sequence;
                ping->errors++;
                if (ping->opts & F_OPT)
                    dprintf(STDERR_FILENO, "E");
                else
                    icmp6_error_handler(icmp6_hdr->icmp6_type, icmp6_hdr->icmp6_code, sequence,
                        ping->buff_ip);
            }
            break;
        }

        default:
            break;
    }
}



static uint8_t owned_packet_v4(t_ping *ping, void *icmp_area)
{
    struct icmphdr  *response     = (struct icmphdr *)(icmp_area);
    uint16_t        id            = htons(response->un.echo.id);

    return (id == 0 || id == ping->conf.pid ? TRUE : FALSE);
}

static uint8_t owned_packet_v6(t_ping *ping, void *icmp_area)
{
    struct icmp6_hdr *response     = (struct icmp6_hdr *)(icmp_area);
    uint16_t          id            = htons(response->icmp6_id);

    return (id == 0 || id == ping->conf.pid ? TRUE : FALSE);
}

static uint8_t owned_packet_handler(t_ping *ping, void *buffer)
{
    if (ping->mode == IPV4_MODE)
        return (owned_packet_v4(ping, (char *)buffer + IPHDR_SIZE));
    else
        return (owned_packet_v6(ping, buffer));
}

static t_packet_data    *process_packet(
                            t_ping *ping, char *buffer, ssize_t *bytes_recv,
                            struct timeval *time_recv)
{
    uint16_t        discard_time_recv   = 0;
    ssize_t         icmp_area_size      = *bytes_recv;
    char            *icmp_area          = (char *)buffer;
    t_packet_data   *packet_data        = NULL;

    if (*bytes_recv != -1)
    {
        ft_memcpy(&ping->end, time_recv, TIMEVAL_SIZE);
        if (owned_packet_handler(ping, buffer) == TRUE)
        {
            if (ping->mode == IPV4_MODE)
            {
                icmp_area_size -= IPHDR_SIZE;
                icmp_area += IPHDR_SIZE;
            }
            if (!(packet_data = validate_packet(ping, icmp_area_size, time_recv, icmp_area)))
            {
                display_icmp_error(ping, buffer, icmp_area, ping->sequence);
                return (NULL);
            }
            return (packet_data);
        }
        if (ping->opts & V_OPT)
            display_unowned(ping, buffer, bytes_recv);
    }
    if (discard_time_recv != ping->errors)
    {
        ft_memcpy(&ping->end, time_recv, sizeof(struct timeval));
        discard_time_recv = ping->errors;
    }
    return (NULL);
}



t_packet_data           *recv_packet(
                            t_ping *ping, char *buffer,
                            ssize_t *bytes_recv, struct timeval *time_recv)
{
    t_packet_data           *packet_data = NULL;
    struct sockaddr_storage tmp;
    struct msghdr           msghdr;
    struct iovec            msg_iov[1];
    uint8_t                 ancillary[64];

    ft_memset(&msghdr, 0, sizeof(msghdr));
    ft_memset(&tmp, 0, sizeof(struct sockaddr_storage));
    ft_memset(msg_iov, 0, sizeof(msg_iov));
    ft_memset(ancillary, 0, sizeof(ancillary));

    msg_iov->iov_base   = buffer;
    msg_iov->iov_len    = sizeof(char) * MAX_MTU;

    ft_memcpy(&tmp, &ping->target, sizeof(struct sockaddr_storage));

    if (ping->mode == IPV4_MODE)
    {
        msghdr.msg_name     = (struct sockaddr_in *)&tmp;
        msghdr.msg_namelen  = sizeof((struct sockaddr_in *)&tmp);
    }
    else
    {
        msghdr.msg_name     = (struct sockaddr_in6 *)&tmp;
        msghdr.msg_namelen  = sizeof((struct sockaddr_in6 *)&tmp);
    }
    msghdr.msg_iov      = msg_iov;
    msghdr.msg_iovlen   = 1;
    msghdr.msg_flags    = 0;

    if (ping->mode == IPV6_MODE)
    {
        msghdr.msg_control = ancillary;
        msghdr.msg_controllen = sizeof(ancillary);
    }

    *bytes_recv = recvmsg(ping->sockfd, &msghdr, MSG_DONTWAIT);

    packet_data = process_packet(ping, buffer, bytes_recv, time_recv);

    if (ping->mode == IPV6_MODE && packet_data)
    {
        packet_data->ancillary_data.ttl = *(uint8_t *)find_ancillary_data(&msghdr, IPV6_HOPLIMIT);
        if (ping->conf.local == FALSE)
            inet_ntop(AF_INET6, &((struct sockaddr_in6 *)&tmp)->sin6_addr, ping->buff_ip, INET6_ADDRSTRLEN);
    }


    return (packet_data);

}
