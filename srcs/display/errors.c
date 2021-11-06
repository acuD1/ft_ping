/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 16:38:12 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/29 15:58:27 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void     getaddrinfo_error_handler(char *target, int status)
{
    switch (status)
    {
        case EAI_ADDRFAMILY:
            fprintf(stderr,
                "ft_ping: %s: Address family for hostname not supported\n",
                target);
            break ;
        case EAI_AGAIN:
            fprintf(stderr,
                "ft_ping: %s: Temporary failure in name resolution\n",
                target);
            break ;
        case EAI_NODATA:
            fprintf(stderr,
                "ft_ping: %s: No address associated with hostname\n",
                target);
            break ;
        case EAI_NONAME:
            fprintf(stderr,
                "ft_ping: %s: Name or service not known\n",
                target);
            break ;
        case EAI_SYSTEM:
            fprintf(stderr,
                "ft_ping: %s: System error\n",
                target);
            break ;
    }
}

/* https://datatracker.ietf.org/doc/html/rfc792 */

void    icmp_error_handler(
            uint8_t type, uint8_t code, uint16_t sequence, char *source)
{
    char    *error          = NULL;
    uint8_t bad_code    = 0;

    switch (type)
    {
        case ICMP_DEST_UNREACH:
            switch (code)
            {
                case ICMP_NET_UNREACH:
                    error = "Destination Net Unreachable";
                    break ;
                case ICMP_HOST_UNREACH:
                    error = "Destination Host Unreachable";
                    break ;
                case ICMP_PROT_UNREACH:
                    error = "Destination Protocol Unreachable";
                    break ;
                case ICMP_PORT_UNREACH:
                    error = "Destination Port Unreachable";
                    break ;
                case ICMP_FRAG_NEEDED:
                    error = "Frag needed";
                    break ;
                case ICMP_SR_FAILED:
                    error = "Source Route Failed";
                    break ;
                default:
                    bad_code = code;
                    error = "Dest Unreachable, Bad Code: ";
                    break ;
            }
            break ;
        case ICMP_SOURCE_QUENCH:
            error = "Source quench";
            break ;
        case ICMP_REDIRECT:
            switch (code)
            {
                case ICMP_REDIR_NET:
                    error = "Redirect Network";
                    break ;
                case ICMP_REDIR_HOST:
                    error = "Redirect Host";
                    break ;
                case ICMP_REDIR_NETTOS:
                    error = "Redirect Type of Service and Network";
                    break ;
                case ICMP_REDIR_HOSTTOS:
                    error = "Redirect Type of Service and Host";
                    break ;
                default:
                    bad_code = code;
                    error = "Redirect, Bad Code: ";
                    break ;
            }
            break ;
        case ICMP_TIME_EXCEEDED:
            switch (code)
            {
                case ICMP_EXC_TTL:
                    error = "Time to live exceeded";
                    break ;
                case ICMP_EXC_FRAGTIME:
                    error = "Frag reassembly time exceeded";
                    break ;
                default:
                    bad_code = code;
                    error = "Time exceeded, Bad Code: ";
                    break ;
            }
            break ;
        case ICMP_PARAMETERPROB:
            error = "Parameter problem";
            break ;
        case ICMP_TIMESTAMP:
            error = "Timestamp";
            break ;
        case ICMP_TIMESTAMPREPLY:
            error = "Timestamp Reply";
            break ;
        case ICMP_INFO_REQUEST:
            error = "Information Request";
            break ;
        case ICMP_INFO_REPLY:
            error = "Information Reply";
            break ;
        default:
            bad_code = code;
            error = "Bad ICMP Type: ";
    }

    dprintf(STDERR_FILENO, "From %s icmp_seq=%hu %s", source, sequence, error);
    if (bad_code != 0)
        dprintf(STDERR_FILENO, "%hhu", bad_code);
    dprintf(STDERR_FILENO, "\n");
}

void    icmp6_error_handler(
            uint8_t type, uint8_t code, uint16_t sequence, char *source)
{
    char    *error      = NULL;
    uint8_t bad_code    = 0;


    dprintf(2, "ICMP6 Error: %s\n", source);
    switch (type)
    {
        case ICMP6_DST_UNREACH:
            switch (code)
            {
                case ICMP6_DST_UNREACH_NOROUTE:
                    error = "No route to host";
                    break;
                case ICMP6_DST_UNREACH_ADMIN:
                    error = "Administratively prohibited";
                    break;
                case ICMP6_DST_UNREACH_BEYONDSCOPE:
                    error = "Beyond scope of source address";
                    break;
                case ICMP6_DST_UNREACH_ADDR:
                    error = "Address unreachable";
                    break;
                case ICMP6_DST_UNREACH_NOPORT:
                    error = "Port unreachable";
                    break;
                default:
                    bad_code = code;
                    error = "Dest Unreachable, Bad Code: ";
                    break;
            }
            break ;
        case ICMP6_PACKET_TOO_BIG:
            error = "Packet too big";
            break ;
        case ICMP6_TIME_EXCEEDED:
            switch (code)
            {
                case ICMP6_TIME_EXCEED_TRANSIT:
                    error = "Time exceeded in transit";
                    break;
                case ICMP6_TIME_EXCEED_REASSEMBLY:
                    error = "Fragment reassembly time exceeded";
                    break;
                default:
                    bad_code = code;
                    error = "Time Exceeded, Bad Code: ";
                    break;
            }
            break ;
        case ICMP6_PARAM_PROB:
            switch (code)
            {
                case ICMP6_PARAMPROB_HEADER:
                    error = "Bad header";
                    break;
                case ICMP6_PARAMPROB_NEXTHEADER:
                    error = "Bad next header";
                    break;
                case ICMP6_PARAMPROB_OPTION:
                    error = "Bad option";
                    break;
                default:
                    bad_code = code;
                    error = "Parameter Problem, Bad Code: ";
                    break;
            }
            break ;
        default:
            bad_code = type;
            error = "Bad ICMP6 Type: ";
            break ;
    }
    dprintf(STDERR_FILENO, "From %s icmp_seq=%hu %s", source, sequence, error);
    if (bad_code != 0)
        dprintf(STDERR_FILENO, "%hhu", bad_code);
    dprintf(STDERR_FILENO, "\n");
}

void    getnameinfo_error_handler(t_ping *ping, int status)
{
    char    *error  = NULL;
    dprintf(STDERR_FILENO, "STATUS |%d|\n", status);

    switch (status)
    {
        case EAI_AGAIN:
            error = "The name could not be resolved at this time. Try again later";
            break ;
        case EAI_FAIL:
            error = "A nonrecoverable error occurred.";
            break ;
        case EAI_MEMORY:
            error = "Out of memory.";
            break ;
        case EAI_SYSTEM:
            error = "System error";
            break ;
        default:
            error = "Unknown error";
            break;
    }
    dprintf(STDERR_FILENO, "ft_ping: getnameinfo(): %s\n", error);
    exit_routine(ping, FAILURE);
}
