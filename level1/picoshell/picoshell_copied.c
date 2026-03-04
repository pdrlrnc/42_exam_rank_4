/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   picoshell_copied.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pedde-so <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 13:30:59 by pedde-so          #+#    #+#             */
/*   Updated: 2026/03/04 13:32:11 by pedde-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
	So... I realized 30 minutes before the closing of the cluster that
	I had the wrong subject for this dude, so I have copied a solution
	from a friend and I will try to reproduce it during the exam. Do NOT
	tell my mum. Or bocal.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int picoshell(char **cmds[])
{
    int     fd[2] = {fd[0] = -1, fd[1] = -1};
    int     i = -1, in = 0, out = 0, nbr = 0;

    if (!cmds) return (1);
    if (!cmds[0] || !cmds[0][0]) return (0);
    while (cmds[nbr]) nbr++;
    while(++i < nbr)
    {
        if (i + 1 < nbr)
        {
            if (pipe(fd) == -1) return (1);
            out = fd[1];
        }
        if (fork() == 0)
        {
            (dup2(in, 0), close(in)), (dup2(out, 1), close(out), close(fd[0]));
            (execvp(cmds[i][0], cmds[i]), exit(1));
        }
        if (in > 2) close(in);
        if (out > 2) close(out);
        in = fd[0];
    }
    for (int j = 0; j++ < nbr; j++)
        wait(NULL);
    return (0);
}
