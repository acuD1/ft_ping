/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/26 15:33:32 by arsciand          #+#    #+#             */
/*   Updated: 2021/04/30 20:23:23 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include <stdio.h>

/* Build informations */
static void     print_version()
{
    fprintf(stdout, "ft_ping: v.%s-%s-%s-%s\n",
        BUILD_VERSION_STRING,
        BUILD_RELEASE_STRING,
        BUILD_PATCH_STRING,
        BUILD_DATE_STRING);
}

static void     print_usage(void)
{
    fprintf(stderr, "Usage: ft_ping [-vh] <destination>\n");
}

static void     print_unallowed_opt(t_opts_args *opts_args) {
    if (opts_args->invalid)
        fprintf(stderr, "ft_ping: unrecognized option '--%s'\n", opts_args->invalid);
    else
        fprintf(stderr, "ft_ping: invalid option -- '%c'\n", (char)(opts_args->all % 128));
}

/* Options parser */
static uint8_t  get_opts_args_handler(int argc, char **argv, t_core *core)
{
    t_opts_conf opts_conf;

    ft_memset(&opts_conf, 0, sizeof(t_opts_conf));
    opts_conf.allowed_opt = ALLOWED_OPT;
    if (argc < 2)
    {
        print_usage();
        return (FAILURE);
    }
    if (!(core->opts_args = ft_get_opts_and_args(argc, argv, &opts_conf)))
        return (FAILURE);
    if (core->opts_args->all & UNALLOWED_OPT)
    {
        print_unallowed_opt(core->opts_args);
        print_usage();
        return (FAILURE);
    }
    if (core->opts_args->all & H_OPT)
    {
        print_version();
        print_usage();
        return (FAILURE);
    }
    if (core->opts_args->all & V_OPT)
    {
        fprintf(stderr, "NOT SUPPORTED YET\n");
        return (FAILURE);
    }
    if (ft_tablen((const char **)core->opts_args->args) > 1)
    {
        fprintf(stderr, "ft_ping: too many arguments: hops not implemented\n");
        print_usage();
        return (FAILURE);
    }
    return (SUCCESS);
}

int8_t  exit_routine(t_core *core, int8_t status, uint8_t do_exit)
{
    for (struct addrinfo *tmp = NULL; core->res; core->res = tmp)
    {
        tmp = core->res->ai_next;
        free(core->res);
    }
    free_opts_args(core->opts_args);
    if (do_exit == TRUE)
        exit(status);
    return (status);
}

void            error_handler(t_core *core, int8_t status)
{
    /*
       EAI_ADDRFAMILY
              The specified network host does not have any network
              addresses in the requested address family.

       EAI_AGAIN
              The name server returned a temporary failure indication.
              Try again later.

       EAI_BADFLAGS
              hints.ai_flags contains invalid flags; or, hints.ai_flags
              included AI_CANONNAME and name was NULL.

       EAI_FAIL
              The name server returned a permanent failure indication.

       EAI_FAMILY
              The requested address family is not supported.

       EAI_MEMORY
              Out of memory.

       EAI_NODATA
              The specified network host exists, but does not have any
              network addresses defined.

       EAI_NONAME
              The node or service is not known; or both node and service
              are NULL; or AI_NUMERICSERV was specified in
              hints.ai_flags and service was not a numeric port-number
              string.

       EAI_SERVICE
              The requested service is not available for the requested
              socket type.  It may be available through another socket
              type.  For example, this error could occur if service was
              "shell" (a service available only on stream sockets), and
              either hints.ai_protocol was IPPROTO_UDP, or
              hints.ai_socktype was SOCK_DGRAM; or the error could occur
              if service was not NULL, and hints.ai_socktype was
              SOCK_RAW (a socket type that does not support the concept
              of services).

       EAI_SOCKTYPE
              The requested socket type is not supported.  This could
              occur, for example, if hints.ai_socktype and
              hints.ai_protocol are inconsistent (e.g., SOCK_DGRAM and
              IPPROTO_TCP, respectively).

       EAI_SYSTEM
              Other system error, check errno for details.
    */
    /*
        Address family for hostname not supported
        Temporary failure in name resolution
        Bad value for ai_flags
        Non-recoverable failure in name resolution
        ai_family not supported
        Memory allocation failure
        No address associated with hostname
        Name or service not known
        Servname not supported for ai_socktype
        ai_socktype not supported
        System error
    */
    // printf("%s\n", gai_strerror(EAI_ADDRFAMILY));
    // printf("%s\n", gai_strerror(EAI_AGAIN));
    // printf("%s\n", gai_strerror(EAI_BADFLAGS));
    // printf("%s\n", gai_strerror(EAI_FAIL));
    // printf("%s\n", gai_strerror(EAI_FAMILY));
    // printf("%s\n", gai_strerror(EAI_MEMORY));
    // printf("%s\n", gai_strerror(EAI_NODATA));
    // printf("%s\n", gai_strerror(EAI_NONAME));
    // printf("%s\n", gai_strerror(EAI_SERVICE));
    // printf("%s\n", gai_strerror(EAI_SOCKTYPE));
    // printf("%s\n", gai_strerror(EAI_SYSTEM));
    switch (status)
    {
        case EAI_ADDRFAMILY:
            fprintf(stderr, "ft_ping: %s: Address family for hostname not supported\n", core->opts_args->args[0]);
            break ;
        case EAI_AGAIN:
            fprintf(stderr, "ft_ping: %s: Temporary failure in name resolution\n", core->opts_args->args[0]);
            break ;
        case EAI_NODATA:
            fprintf(stderr, "ft_ping: %s: No address associated with hostname\n", core->opts_args->args[0]);
            break ;
        case EAI_NONAME:
            fprintf(stderr, "ft_ping: %s: Name or service not known\n", core->opts_args->args[0]);
            break ;
        case EAI_SYSTEM:
            fprintf(stderr, "ft_ping: %s: System error\n", core->opts_args->args[0]);
            break ;
    }
    exit_routine(core, FAILURE, TRUE);
}

static uint8_t init_core(t_core *core)
{
    struct  addrinfo    hints;
    int8_t              status  = 0;

    /* Preparing getaddrinfo struct */
    ft_memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags      = AI_V4MAPPED | AI_ADDRCONFIG;
    hints.ai_family     = AF_INET;
	hints.ai_socktype   = SOCK_STREAM | SOCK_DGRAM;

    if ((status = getaddrinfo(core->opts_args->args[0], NULL, &hints, &core->res))
        != SUCCESS)
        error_handler(core, status);

    /*  Converting internet address */
    if (!(inet_ntop(core->res->ai_family,
        &((struct sockaddr_in *)core->res->ai_addr)->sin_addr, core->target_ipv4,
        sizeof(core->target_ipv4))))
        exit_routine(core, FAILURE, TRUE);

    return (SUCCESS);
}


int             main(int argc, char *argv[])
{
    t_core  core;

    ft_memset(&core, 0, sizeof(t_core));
    if (get_opts_args_handler(argc, argv, &core) != SUCCESS)
        return (exit_routine(&core, FAILURE, TRUE));

    /* Check if ft_ping executed as root */
    // if (getuid())
    // {
    //     fprintf(stderr, "ft_ping: socket: Operation not permitted\n");
    //     return (exit_routine(&core, FAILURE, TRUE));
    // }

    if (init_core(&core) != SUCCESS)
        return (exit_routine(&core, FAILURE, FALSE));
    // else
    //     exec_ft_ping(&core);
    printf("-> |%s|\n", core.target_ipv4);
    debug_opts_args(core.opts_args);
    return (exit_routine(&core, EXIT_SUCCESS, FALSE));
}
