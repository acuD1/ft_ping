/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate_packet.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/12 12:18:38 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/12 13:12:22 by arsciand         ###   ########.fr       */
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

    for (size_t i = 0; i < (uint64_t)payload_size - sizeof(struct timeval); i++)
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

    if (check_payload(payload, payload_size) != SUCCESS)
        return (NULL);
    if (!(packet = ft_lstfind(ping->packets,
                        &sequence, (int (*)(void*, void*))find_sequence)))
        return (NULL);
    if (check_timeval(
            (char *)payload + ((uint64_t)payload_size - sizeof(struct timeval)),
            packet) != SUCCESS)
        return (NULL);

    return (packet);
}

t_packet_data   *validate_packet(
                    t_ping *ping, ssize_t icmp_area_size,
                    struct timeval *time_recv, void *icmp_area)
{
    t_lst           *packet       = NULL;
    t_packet_data   *packet_data  = NULL;
    struct icmphdr  *response     = (struct icmphdr *)(icmp_area);

    if (response->type == ICMP_ECHOREPLY
        && htons(response->un.echo.id) == ping->conf.pid
        && htons(response->un.echo.sequence) <= ping->sequence)
    {
        if (!(packet = check_packet(ping, (char *)icmp_area + ICMPHDR_SIZE,
                            icmp_area_size - ICMPHDR_SIZE,
                            htons(response->un.echo.sequence))))
            return (NULL);
        else
        {
            packet_data = (t_packet_data *)packet->content;
            ft_memcpy(&packet_data->time_recv, time_recv,
                sizeof(struct timeval));
            packet_data->status |= PACKET_RECEIVED;
            ping->received++;
            return (packet_data);
        }
    }

    return (NULL);
}
