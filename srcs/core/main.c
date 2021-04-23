/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsciand <arsciand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/26 15:33:32 by arsciand          #+#    #+#             */
/*   Updated: 2021/04/23 15:39:18 by arsciand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"
#include <stdio.h>

/* Build informations */
static void print_version()
{
    fprintf(stdout, "ft_ping: v.%s-%s-%s-%s\n",
        BUILD_VERSION_STRING,
        BUILD_RELEASE_STRING,
        BUILD_PATCH_STRING,
        BUILD_DATE_STRING);
}

static void print_usage(void)
{
    fprintf(stderr, "Usage: ft_ping [-vh] <destination>\n");
}

static void print_unallowed_opt(t_opt *opt) {
    if (opt->invalid)
        fprintf(stderr, "ft_ping: unrecognized option '--%s'\n", opt->invalid);
    else
        fprintf(stderr, "ft_ping: invalid option -- '%c'\n", (char)(opt->all % 128));
}

/* Options parser */
static uint8_t get_opt(int argc, char **argv, t_core *core)
{
    ft_memset(&core->opt, 0, sizeof(t_opt));
    if (argc < 2)
    {
        print_usage();
        return (FAILURE);
    }
    core->opt = ft_getopts(argc, argv, ALLOWED_OPT, ALLOWED_OPT_TAB));
    if (core->opt->all & UNALLOWED_OPT)
    {
        print_unallowed_opt(core->opt);
        print_usage();
        return (FAILURE);
    }
    if (core->opt->all & H_OPT)
    {
        print_version();
        print_usage();
        return (FAILURE);
    }
    if (core->opt->all & V_OPT)
    {
        fprintf(stderr, "NOT SUPPORTED YET\n");
        // return (FAILURE);
    }
    return (SUCCESS);
}

static uint8_t init_core(t_core *core)
{
    // char    ip_string[INET_ADDRSTRLEN];
    (void)core;

    return (SUCCESS);
}

int     main(int argc, char *argv[])
{
    t_core core;

    ft_memset(&core, 0, sizeof(t_core));

    const char **tab = ALLOWED_OPT_TAB);
    size_t i = 0;
    while (tab[i])
    {
        printf("TAB[%zu] | %s\n", i, tab[i]);
        i++;
    }
    if (get_opt(argc, argv, &core) != SUCCESS)
        exit(FAILURE);

    while (core.opt->opt_set) {
        printf("->\n%s\n%s\n", ((t_opt_set_db *)core.opt->opt_set->content)->current, ((t_opt_set_db *)core.opt->opt_set->content)->arg);
        core.opt->opt_set = core.opt->opt_set->next;
    }
    /* Check if ft_ping executed as root */
    // if (getuid())
    // {
    //     fprintf(stderr, "ft_ping: socket: Operation not permitted\n");
    //     exit(FAILURE);
    // }

    if (init_core(&core) != SUCCESS)
        return (EXIT_FAILURE);
    // else
    //     exec_ft_ping(&core);
    return (EXIT_SUCCESS);
}
