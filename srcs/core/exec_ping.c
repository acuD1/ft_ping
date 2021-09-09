/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_ping.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/29 14:30:22 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/09 15:59:46 by arsciand         ###   ########.fr       */
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

    #ifdef DEBUG
        dprintf(STDERR_FILENO, "[DEBUG] check_payload() OK\n");
    #endif

    return (SUCCESS);
}

static uint8_t  check_timeval(void *timeval, t_lst *packet)
{
    t_packet_data   *tmp_packet = (t_packet_data *)packet->content;
    struct timeval  tmp_timeval = *(struct timeval *)timeval;

    #ifdef DEBUG
        print_bytes(16, timeval);
        dprintf(STDERR_FILENO, "[DEBUG] tmp_packet->time_sent.tv_sec |%ld|\n", tmp_packet->time_sent.tv_sec);
        dprintf(STDERR_FILENO, "[DEBUG] tmp_packet->time_sent.tv_usec |%ld|\n", tmp_packet->time_sent.tv_usec);
        dprintf(STDERR_FILENO, "[DEBUG] tmp_timeval->tv_sec |%ld|\n", tmp_timeval.tv_sec);
        dprintf(STDERR_FILENO, "[DEBUG] tmp_timeval->tv_usec |%ld|\n", tmp_timeval.tv_usec);
    #endif

    if ((tmp_timeval.tv_sec != tmp_packet->time_sent.tv_sec) || (tmp_timeval.tv_usec != tmp_packet->time_sent.tv_usec))
        return (FAILURE);

    #ifdef DEBUG
        dprintf(STDERR_FILENO, "[DEBUG] check_timeval() OK\n");
    #endif

    return (SUCCESS);
}

static t_lst    *validate_packet(t_ping *ping, void *payload, ssize_t payload_size, uint16_t sequence)
{
    t_lst           *packet = NULL;

    if (check_payload(payload, payload_size) != SUCCESS)
        return (NULL);
    if (!(packet = ft_lstfind(ping->packets, &sequence, (int (*)(void*, void*))find_sequence)))
        return (NULL);
    if (check_timeval((char *)payload + ((uint64_t)payload_size - sizeof(struct timeval)), packet) != SUCCESS)
        return (NULL);

    return (packet);
}

static void     analyse_packet(t_ping *ping, ssize_t icmp_area_size, struct timeval *time_received, void *icmp_area)
{
    t_lst           *validated_packet   = NULL;
    struct icmphdr  *response           = (struct icmphdr *)(icmp_area);

    #ifdef DEBUG
        dprintf(STDERR_FILENO, "[DEBUG] icmphdr->type |%s|\n", response->type == ICMP_ECHOREPLY ? "ICMP_REPLY" : ft_itoa(response->type));
    #endif

    if (response->type == ICMP_ECHOREPLY
        && htons(response->un.echo.id) == ping->conf.pid
        && htons(response->un.echo.sequence) <= ping->sequence)

        #ifdef DEBUG
            dprintf(STDERR_FILENO, "[DEBUG] icmphdr->un.ech.sequence |%hu|\n", htons(response->un.echo.sequence));
            dprintf(STDERR_FILENO, "[DEBUG] icmphdr->un.ech.id |%hu|\n", htons(response->un.echo.id));
            print_bytes((int)icmp_area_size, icmp_area);
            // print_time((char *)icmp_area + ((uint64_t)icmp_area_size - sizeof(struct timeval) - IPHDR_SIZE));
            dprintf(STDERR_FILENO, "[DEBUG] TIME SEC |%lu| USEC |%lu| (RECEIVED) \n", time_received->tv_sec, time_received->tv_usec);
        #endif
    {
        // ssize_t delta_timeval_buffer =  bytes_received - (ssize_t)(ICMPHDR_SIZE + sizeof(struct timeval));
        if (!(validated_packet = validate_packet(ping, (char *)icmp_area + ICMPHDR_SIZE, icmp_area_size - ICMPHDR_SIZE, htons(response->un.echo.sequence))))
        {
            // process_packet()
            dprintf(STDERR_FILENO, "NOT FOUND !\n");
        }
        else
        {
            dprintf(STDERR_FILENO, "Validaded !\n");
        }
    }

    #ifdef DEBUG
        if (response->type != ICMP_ECHOREPLY || htons(response->un.echo.id) != ping->conf.pid || htons(response->un.echo.sequence) > ping->sequence)
            dprintf(STDERR_FILENO, "\n/!\\\n[DEBUG] Wrong packet !!!\n/!\\\n\n");
    #endif
}

static void     retrieve_response(t_ping *ping)
{
    ssize_t                 bytes_received  = 0;
    char                    buffer[MAX_MTU];
    struct sockaddr_storage tmp;
    struct msghdr           msghdr;
    struct iovec            msg_iov[1];
    struct timeval          time_received;

    ft_memset(&buffer, 0, sizeof(buffer));
    ft_memset(&msghdr, 0, sizeof(msghdr));
    ft_memset(&tmp, 0, sizeof(struct sockaddr_storage));
    ft_memset(msg_iov, 0, sizeof(msg_iov));

    msg_iov->iov_base   = buffer;
    msg_iov->iov_len    = sizeof(buffer);

    ft_memcpy(&tmp, &ping->target, sizeof(struct sockaddr_storage));

    msghdr.msg_name     = (struct sockaddr_in *)&tmp;
    msghdr.msg_namelen  = sizeof((struct sockaddr_in *)&tmp);
    msghdr.msg_iov      = msg_iov;
    msghdr.msg_iovlen   = 1;
    msghdr.msg_flags    = 0;

    bytes_received = recvmsg(ping->sockfd, &msghdr, MSG_DONTWAIT);
    if (bytes_received != -1)
    {
        gettimeofday_handler(ping, &time_received);

        #ifdef DEBUG
            dprintf(STDERR_FILENO, "---\n[DEBUG] bytes_received |%zu|\n", bytes_received);
            print_bytes((int)bytes_received, &buffer);
        #endif

        analyse_packet(ping, bytes_received - IPHDR_SIZE, &time_received, buffer + IPHDR_SIZE);
    }
}

uint8_t         exec_ping(t_ping *ping)
{
    char    *packet = NULL;

    if (!(packet = ft_memalloc(ping->conf.packet_size)))
        exit_routine(ping, FAILURE);

    print_init(ping);

    setup_socket(ping);

    send_packet(ping, packet);
    while (1)
    {
        if (g_ping & SEND_PACKET)
        {
            send_packet(ping, packet);
            g_ping = 0;
        }
        if (g_ping & EXIT_PING)
        {

            #ifdef DEBUG
                dprintf(STDERR_FILENO, "[DEBUG] EXIT_PING\n");
            #endif

            break ;
        }
        retrieve_response(ping);

    }
    ft_strdel(&packet);

    #ifdef DEBUG
        ft_lstiter(ping->packets, debug_packets);
    #endif

    return (SUCCESS);
}
