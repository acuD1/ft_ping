/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   misc.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/12 16:46:20 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/30 19:17:02 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

char        *inet_ntop_handler(t_ping *ping, uint32_t *addr)
{
    ft_memset(&ping->buff_ip, 0, sizeof(ping->buff_ip));
    if (!(inet_ntop(ping->mode == IPV4_MODE ? AF_INET : AF_INET6, addr, ping->buff_ip, sizeof(ping->buff_ip))))
    {
        dprintf(STDERR_FILENO, "ft_ping: inet_ntop(): %s\n", strerror(errno));
        exit_routine(ping, FAILURE);
    }

    return (ping->buff_ip);
}

uint8_t     inet_pton_handler(t_ping *ping, char *target)
{
    u_char  buff[sizeof(struct in6_addr)];
    ft_memset(&buff, 0, sizeof(struct in6_addr));

    if (!inet_pton(ping->mode == IPV4_MODE ? AF_INET : AF_INET6, target, buff))
        return FALSE;
    return (TRUE);
}

void        getnameinfo_handler(t_ping *ping)
{
    int     status = 0;
    struct sockaddr_in6 sa6 = *(struct sockaddr_in6 *)&ping->target;
    struct sockaddr_in sa = *(struct sockaddr_in *)&ping->target;

    ft_memset(ping->buff_dns, 0, sizeof(ping->buff_dns));
    if ((status = getnameinfo(
                    ping->mode == IPV4_MODE ? (struct sockaddr *)&sa : (struct sockaddr *)&sa6,
                    ping->mode == IPV4_MODE ? sizeof(sa) : sizeof(sa6), ping->buff_dns,
                    sizeof(ping->buff_dns), NULL, 0, NI_NAMEREQD)) != 0)
    {
        if (status != EAI_NONAME)
            getnameinfo_error_handler(ping, status);
    }
    inet_ntop_handler(ping, ping->mode == IPV4_MODE ? (uint32_t *)&sa.sin_addr : (uint32_t * )&sa6.sin6_addr);
    ping->conf.dns = TRUE;
}

uint16_t    in_cksum(void *buffer, size_t len)
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

void        *find_ancillary_data(struct msghdr *msghdr, int cmsg_type)
{
    struct cmsghdr *cmsg = NULL;

    for (cmsg = CMSG_FIRSTHDR(msghdr); cmsg != NULL; cmsg = CMSG_NXTHDR(msghdr, cmsg))
    {
        if ((cmsg->cmsg_level == IPPROTO_IPV6) && (cmsg->cmsg_type == cmsg_type)) {
            return (CMSG_DATA(cmsg));
        }
    }
    return (NULL);
}
