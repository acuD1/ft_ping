/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate_packet.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/12 12:18:38 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/26 15:31:41 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static int       find_sequence(void *current, void *to_find)
{
    t_packet_data   *tmp_current = current;
    uint16_t        tmp_to_find = *(uint16_t *)to_find;

    return (tmp_current->sequence == tmp_to_find ? TRUE : FALSE);
}

static uint8_t   check_payload(void *payload, ssize_t payload_size)
{
    char *payload_42 = (char *)payload;
    size_t  custom_payload_size = 0;

    if (payload_size >= TIMEVAL_SIZE)
        custom_payload_size = TIMEVAL_SIZE;

    for (size_t i = custom_payload_size; i < (size_t)payload_size; i++)
    {
        if (payload_42[i] != 0x42)
            return (FAILURE);
    }
    return (SUCCESS);
}

static uint8_t  check_timeval(void *timeval, t_lst *packet)
{
    t_packet_data   *tmp_packet = (t_packet_data *)packet->content;
    struct timeval  tmp_timeval = *(struct timeval *)timeval;

    if ((tmp_timeval.tv_sec != tmp_packet->time_sent.tv_sec)
        || (tmp_timeval.tv_usec != tmp_packet->time_sent.tv_usec))
        return (FAILURE);

    return (SUCCESS);
}

static t_lst    *check_packet(
                    t_ping *ping, void *payload, ssize_t payload_size,
                    uint16_t sequence)
{
    t_lst   *packet = NULL;

    if ((ping->opts & F_OPT) == 0
        && check_payload(payload, payload_size) != SUCCESS)
        return (NULL);
    if (!(packet = ft_lstfind(ping->packets, &sequence,
                    (int (*)(void*, void*))find_sequence)))
        return (NULL);
    if ((ping->opts & F_OPT) == 0 && payload_size >= TIMEVAL_SIZE)
    {
        if (check_timeval(payload, packet) != SUCCESS)
            return (NULL);
    }
    return (packet);
}

static uint8_t   check_icmp_sequence(
                    t_ping *ping, void *icmp_area, uint16_t *sequence)
{
    if (ping->mode == IPV4_MODE)
    {
        struct icmphdr  *icmphdr    = (struct icmphdr *)(icmp_area);

        *sequence = htons(icmphdr->un.echo.sequence);
        if (icmphdr->type != ICMP_ECHOREPLY || *sequence > ping->sequence)
            return (FAILURE);
    }
    else if (ping->mode == IPV6_MODE)
    {
        struct icmp6_hdr *icmp6_hdr  = (struct icmp6_hdr *)(icmp_area);

        *sequence = htons(icmp6_hdr->icmp6_seq);
        if (icmp6_hdr->icmp6_type != ICMP6_ECHO_REPLY
            || *sequence > ping->sequence)
            return (FAILURE);
    }
    return (SUCCESS);
}

t_packet_data   *validate_packet(
                    t_ping *ping, ssize_t icmp_area_size,
                    struct timeval *time_recv, void *icmp_area)
{
    t_lst               *packet         = NULL;
    t_packet_data       *packet_data    = NULL;
    uint16_t            sequence        = 0;

    if (check_icmp_sequence(ping, icmp_area, &sequence) != SUCCESS)
        return (NULL);
    if (!(packet = check_packet(ping, (char *)icmp_area + ICMPHDR_SIZE,
                        icmp_area_size - ICMPHDR_SIZE,
                        sequence)))
        return (NULL);
    else
    {
        if (ping->pipe < (ping->sequence - sequence))
            ping->pipe = ping->sequence - sequence;
        packet_data = (t_packet_data *)packet->content;
        ft_memcpy(&packet_data->time_recv, time_recv,
            sizeof(struct timeval));
        packet_data->sequence = sequence;
        packet_data->status |= PACKET_RECEIVED;
        ping->received++;
        return (packet_data);
    }

    return (NULL);
}
