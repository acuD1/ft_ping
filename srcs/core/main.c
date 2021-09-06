/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/26 15:33:32 by arsciand          #+#    #+#             */
/*   Updated: 2021/09/06 14:32:45 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

int             main(int argc, char *argv[])
{
    t_ping  ping;

    init_ping(&ping);

    if (set_opts_args(&ping, argc, argv) != SUCCESS)
        exit_routine(&ping, FAILURE);

    /* Check if ft_ping executed as root */
    if (getuid())
    {
        fprintf(stderr, "ft_ping: socket: Operation not permitted\n");
        exit_routine(&ping, FAILURE);
    }
    // else
    //     exec_ft_ping();

    free_ping(&ping);
    return (EXIT_SUCCESS);
}
