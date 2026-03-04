/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   picoshell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pedde-so <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 09:30:11 by pedde-so          #+#    #+#             */
/*   Updated: 2026/03/04 09:30:18 by pedde-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

//prototypes
int	picoshell(char *cmds[]);
int	ft_strcmp(char *str1, char *str2);

int	main(int argc, char **argv)
{
	int	i;

	i = 1;
	if (argc == 1)
		return (0);
	return (picoshell(argv + 1));
}

int	ft_strcmp(char *str1, char *str2)
{
	int	i;

	if (!str1 || !str2)
		return (1);
	i = 0;
	while (*(str1 + i) && *(str2 + i))
	{
		if (*(str1 + i) != *(str2 + i))
			return (*(str1 + i) - *(str2 + i));
		i++;
	}
	return (*(str1 + i) - *(str2 + i));
}

int	picoshell(char *cmds[])
{
	int	i;
	int	fds[2];
	int	in_fd;
	int	res;
	pid_t	pid;
	int	status;
	int	start;

	i = 0;
	in_fd = 0;
	res = 0;

	while (cmds[i])
	{
		while (cmds[i] && !ft_strcmp(cmds[i], "|"))
			i++;
		start = i;
		if (cmds[i + 1]) //nao é o último
		{
			if (pipe(fds) == -1)
				return (1);
		}
		else
		{
			fds[0] = -1;
			fds[1] = -1;
		}
		pid = fork();
		if (pid < 0)
		{
			if (fds[0] != -1)
				close(fds[0]);
			if (fds[1] != -1)
				close(fds[1]);
			if (in_fd != 0)
				close(in_fd);
			return (1);
		}
		if (pid == 0) //filho
		{
			i = start;
			while (cmds[i] && ft_strcmp("|", cmds[i]))
				i++;
			if (cmds[i])
				cmds[i] = NULL;
			if (in_fd != 0)
			{
				if (dup2(in_fd, STDIN_FILENO) == -1)
					exit(1);
				close(in_fd);
			}
			if (fds[1] != -1) // nao é o útimo
			{
				if (dup2(fds[1], STDOUT_FILENO) == -1)
					exit(1);
				close(fds[0]);
				close(fds[1]);
			}
			execvp(cmds[start], cmds + start);
			exit(1);
		}
		else //pai
		{
			if (in_fd != 0)
				close(in_fd);
			if (fds[1] != -1)
				close(fds[1]);
			in_fd = fds[0];
			i++;
		}
	}
	while (wait(&status) > 0)
	{
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			res = 1;
		else if (!WIFEXITED(status))
			res = 1;
	}

	return (res);

}

