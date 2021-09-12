/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   recv_packet.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/12 11:48:32 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/12 17:07:25 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static void             display_icmp_error(
                            t_ping *ping, void *buffer, void *icmp_area,
                            uint16_t sequence)
{
    struct iphdr    *iphdr     = (struct iphdr *)buffer;
    struct icmphdr  *icmphdr   = (struct icmphdr *)icmp_area;

    inet_ntop_handler(ping, &iphdr->saddr);

    if (icmphdr->type != ICMP_ECHO)
        icmp_error_handler(icmphdr->type, icmphdr->code, sequence,
            ping->buff_ipv4);
}

static t_packet_data    *process_packet(
                            t_ping *ping, char *buffer, ssize_t *bytes_recv,
                            struct timeval *time_recv)
{
    uint16_t        discard_time_recv   = 0;
    t_packet_data   *packet_data        = NULL;

    if (*bytes_recv != -1)
    {
        ft_memcpy(&ping->end, time_recv, sizeof(struct timeval));
        if (!(packet_data = validate_packet(ping, *bytes_recv - IPHDR_SIZE,
                                time_recv, buffer + IPHDR_SIZE)))
        {
            display_icmp_error(ping, buffer, buffer + IPHDR_SIZE,
                ping->sequence);
            ping->errors++;
            return (NULL);
        }
        return (packet_data);
    }
    else
    {
        if (discard_time_recv != ping->errors)
        {
            ft_memcpy(&ping->end, time_recv, sizeof(struct timeval));
            discard_time_recv = ping->errors;
        }
    }
    return (NULL);
}

t_packet_data           *recv_packet(
                            t_ping *ping, char *buffer,
                            ssize_t *bytes_recv, struct timeval *time_recv)
{
    struct sockaddr_storage tmp;
    struct msghdr           msghdr;
    struct iovec            msg_iov[1];

    ft_memset(&msghdr, 0, sizeof(msghdr));
    ft_memset(&tmp, 0, sizeof(struct sockaddr_storage));
    ft_memset(msg_iov, 0, sizeof(msg_iov));

    msg_iov->iov_base   = buffer;
    msg_iov->iov_len    = sizeof(buffer) * MAX_MTU;

    ft_memcpy(&tmp, &ping->target, sizeof(struct sockaddr_storage));

    msghdr.msg_name     = (struct sockaddr_in *)&tmp;
    msghdr.msg_namelen  = sizeof((struct sockaddr_in *)&tmp);
    msghdr.msg_iov      = msg_iov;
    msghdr.msg_iovlen   = 1;
    msghdr.msg_flags    = 0;

    *bytes_recv = recvmsg(ping->sockfd, &msghdr, MSG_DONTWAIT);

    return (process_packet(ping, buffer, bytes_recv, time_recv));

}