/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_popen.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pedde-so <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 12:18:26 by pedde-so          #+#    #+#             */
/*   Updated: 2026/01/21 12:18:58 by pedde-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

//prototypes
int	ft_popen(const char *file, char *const argv[], char type);


int	ft_popen(const char *file, char *const argv[], char type)
{
	pid_t	pid;
	int	fd[2];

	if (!file || !argv || (type != 'r' && type != 'w'))
		return (-1);
	if (pipe(fd) == -1)
		return (-1);
	pid = fork();
	if (pid == -1)
	{
		close(fd[0]);
		close(fd[1]);
		return (-1);
	}
	if (pid == 0)
	{
		if (type == 'r')
		{
			if (dup2(fd[1], STDOUT_FILENO) == -1)
				return (-1);
		}
		if (type == 'w')
		{
			if (dup2(fd[0], STDIN_FILENO) == -1)
				return (-1);
		}
		close(fd[0]);
		close(fd[1]);
		execvp(file, argv);
		return (-1);
	}
	else
	{
		if (type == 'r')
		{
			close(fd[1]);
			return (fd[0]);
		}
		else
		{
			close(fd[1]);
			return (fd[0]);
		}

	}
	
}

int	main() {
	int	fd = ft_popen("ls", (char *const []){"ls", NULL}, 'r');
	return (fd);
}
