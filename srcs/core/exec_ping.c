/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_ping.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/29 14:30:22 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/10 17:42:22 by arsciand         ###   ########.fr       */
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

static t_packet_data     *analyse_packet(t_ping *ping, ssize_t icmp_area_size, struct timeval *time_received, void *icmp_area)
{
    t_lst           *validated_packet       = NULL;
    t_packet_data   *validated_packet_data  = NULL;
    struct icmphdr  *response               = (struct icmphdr *)(icmp_area);

    #ifdef DEBUG
        dprintf(STDERR_FILENO, "[DEBUG] icmphdr->type |%s|\n", response->type == ICMP_ECHOREPLY ? "ICMP_REPLY" : ft_itoa(response->type));
    #endif

    if (response->type == ICMP_ECHOREPLY
        && htons(response->un.echo.id) == ping->conf.pid
        && htons(response->un.echo.sequence) <= ping->sequence)
    {
        #ifdef DEBUG
            dprintf(STDERR_FILENO, "[DEBUG] icmphdr->un.ech.sequence |%hu|\n", htons(response->un.echo.sequence));
            dprintf(STDERR_FILENO, "[DEBUG] icmphdr->un.ech.id |%hu|\n", htons(response->un.echo.id));
            // print_bytes((int)icmp_area_size, icmp_area);
            // print_time((char *)icmp_area + ((uint64_t)icmp_area_size - sizeof(struct timeval) - IPHDR_SIZE));
            dprintf(STDERR_FILENO, "[DEBUG] TIME SEC |%lu| USEC |%lu| (RECEIVED) \n", time_received->tv_sec, time_received->tv_usec);
        #endif

        if (!(validated_packet = validate_packet(ping, (char *)icmp_area + ICMPHDR_SIZE, icmp_area_size - ICMPHDR_SIZE, htons(response->un.echo.sequence))))
            return (NULL);
        else
        {
            validated_packet_data = (t_packet_data *)validated_packet->content;
            ft_memcpy(&validated_packet_data->time_received, time_received, sizeof(struct timeval));
            validated_packet_data->status |= PACKET_RECEIVED;
            ping->received++;
            return (validated_packet_data);
        }
    }

    #ifdef DEBUG
        if (response->type != ICMP_ECHOREPLY || htons(response->un.echo.id) != ping->conf.pid || htons(response->un.echo.sequence) > ping->sequence)
            dprintf(STDERR_FILENO, "\n/!\\\n[DEBUG] Wrong packet !!!\n/!\\\n\n");
    #endif

    return (NULL);
}

static void             display_icmp_error(void *buffer, void *icmp_area, uint16_t sequence)
{
    char            buff_ipv4[INET_ADDRSTRLEN];
    struct iphdr    *iphdr      = (struct iphdr *)buffer;
    struct icmphdr  *response   = (struct icmphdr *)(icmp_area);

    inet_ntop(AF_INET, &iphdr->saddr, buff_ipv4, sizeof(buff_ipv4));

    if (response->type != ICMP_ECHO)
        icmp_error_handler(response->type, response->code, sequence, buff_ipv4);
}

static t_packet_data     *retrieve_response(t_ping *ping, char *buffer, ssize_t *bytes_received)
{
    t_packet_data           *packet_data    = NULL;
    struct sockaddr_storage tmp;
    struct msghdr           msghdr;
    struct iovec            msg_iov[1];
    struct timeval          time_received;

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

    *bytes_received = recvmsg(ping->sockfd, &msghdr, MSG_DONTWAIT);
    if (*bytes_received != -1)
    {
        gettimeofday_handler(ping, &time_received);
        ping->end = time_received;
        if (!(packet_data = analyse_packet(ping, *bytes_received - IPHDR_SIZE, &time_received, buffer + IPHDR_SIZE)))
        {
            display_icmp_error(buffer, buffer + IPHDR_SIZE, ping->sequence);
            ping->errors++;
            return (NULL);
        }
        return (packet_data);
    }
    return (NULL);
}

static void     display_response(void *buffer, t_packet_data *packet_data, ssize_t *bytes_received)
{
    #ifdef DEBUG
        dprintf(STDERR_FILENO, "/!\\ RECEIVED VALIDED PACKET |%hu|\n", packet_data->sequence);
        print_bytes(84, buffer);
    #endif

    struct iphdr *iphdr = (struct iphdr *)buffer;
    char          buff_ipv4[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &iphdr->saddr, buff_ipv4, sizeof(buff_ipv4));

    dprintf(STDOUT_FILENO, "%zd bytes from %s: icmp->seq=%hu ttl=%hhu time=%.2lf ms\n",
        *bytes_received - IPHDR_SIZE, buff_ipv4,
        packet_data->sequence, iphdr->ttl,
        calc_latency(&packet_data->time_sent, &packet_data->time_received));
}

static void     fetch_responses(t_ping *ping)
{
    struct sockaddr_in  *tmp = (struct sockaddr_in *)&ping->target;
    char                buff_ipv4[INET_ADDRSTRLEN];
    t_ping_rtt          ping_rtt;

    ft_memset(&buff_ipv4, 0, sizeof(buff_ipv4));
    ft_memset(&ping_rtt, 0, sizeof(t_ping_rtt));

    #pragma clang diagnostic ignored "-Wcast-align"

    if (!(inet_ntop(tmp->sin_family, &tmp->sin_addr, buff_ipv4, sizeof(buff_ipv4))))
    {
        dprintf(STDERR_FILENO, "ft_ping: inet_ntop(): %s\n", strerror(errno));
        exit_routine(ping, FAILURE);
    }

    ft_lstiter_ctx(ping->packets, &ping_rtt, fetch_ping_rtt);


    dprintf(STDOUT_FILENO, "--- %s ping statistics ---\n", buff_ipv4);
    dprintf(STDOUT_FILENO, "%hu packets transmitted, %hu received, ", ping->sequence, ping->received);
    if (ping->errors)
        dprintf(STDOUT_FILENO, "+%hu errors, ", ping->errors);
    dprintf(STDOUT_FILENO, "%d%% packet loss, time %.f ms\n",
        calc_packet_loss(ping), calc_latency(&ping->start, &ping->end));
    dprintf(STDOUT_FILENO, "rtt min/avg/max/mdev = %.3lf/%.3lf/%.3lf/%.3lf ms\n",
        ping_rtt.min, ping_rtt.avg, ping_rtt.max, calc_mdev(ping, &ping_rtt));
}

uint8_t         exec_ping(t_ping *ping)
{
    char            *packet         = NULL;
    t_packet_data   *packet_data    = NULL;
    ssize_t         bytes_received  = 0;
    char            buffer[MAX_MTU];

    if (!(packet = ft_memalloc(ping->conf.packet_size)))
        exit_routine(ping, FAILURE);

    print_init(ping);

    setup_socket(ping);

    // alarm(1);
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
            dprintf(STDERR_FILENO, "\n");

            break ;
        }
        ft_memset(&buffer, 0, sizeof(buffer));
        if ((packet_data = retrieve_response(ping, buffer, &bytes_received)) != NULL)
        {
            display_response(buffer, packet_data, &bytes_received);
        }
    }
    fetch_responses(ping);
    ft_strdel(&packet);
    #ifdef DEBUG
        ft_lstiter(ping->packets, debug_packets);
    #endif

    return (SUCCESS);
}
