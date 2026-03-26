/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   picoshell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pedde-so <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 14:51:15 by pedde-so          #+#    #+#             */
/*   Updated: 2026/03/26 14:51:16 by pedde-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

//prototypes
int	picoshell(char **cmds[]);


int	picoshell(char **cmds[])
{
	int	fd[2];
	int	i = 0;
	int	prev = -1;
	pid_t	pid;

	while (cmds[i])
	{
		if (pipe(fd) == -1)
		{
			if (prev != -1)
				close(prev);
			return (1);
		}
		pid = fork();
		if (pid == -1)
		{
			if (prev != -1)
				close(prev);
			close(fd[0]);
			close(fd[1]);
			return (1);
		}
		if (pid == 0)
		{
			if (prev == -1 && cmds[i + 1]) //first of many
			{
				if (dup2(fd[1], STDOUT_FILENO) == -1)
				{
					close(fd[0]);
					close(fd[1]);
					return (1);
				}
			}
			else if (prev != -1 && !cmds[i + 1]) //last of many
			{
				if (dup2(prev, STDIN_FILENO) == -1)
				{
					close(prev);
					close(fd[0]);
					close(fd[1]);
					return (1);
				}
			}
			else if (cmds[i + 1]) //middle
			{
				if (dup2(prev, STDIN_FILENO) == -1)
				{
					close(prev);
					close(fd[0]);
					close(fd[1]);
					return (1);
				}
				if (dup2(fd[1], STDOUT_FILENO) == -1)
				{
					close(prev);
					close(fd[0]);
					close(fd[1]);
					return (1);
				}
			}
			close(fd[0]);
			close(fd[1]);
			if (prev != -1)
				close(prev);
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		close(fd[1]);
		if (prev != -1)
			close(prev);
		if (cmds[i + 1])
			prev = fd[0];
		else
			close(fd[0]);
		i++;
	}
	while (wait(NULL) > 0)
		;
	return (0);
}

int	main(void)
{
	printf("=== Test 1: ls | grep picoshell ===\n");
	char	*t1c1[] = {"/bin/ls", NULL};
	char	*t1c2[] = {"/usr/bin/grep", "picoshell", NULL};
	char	**t1[] = {t1c1, t1c2, NULL};
	picoshell(t1);

	printf("\n=== Test 2: echo | cat | sed ===\n");
	char	*t2c1[] = {"echo", "squalala", NULL};
	char	*t2c2[] = {"cat", NULL};
	char	*t2c3[] = {"sed", "s/a/b/g", NULL};
	char	**t2[] = {t2c1, t2c2, t2c3, NULL};
	picoshell(t2);

	printf("\n=== Test 3: long pipe (5 commands) ===\n");
	char	*t3c1[] = {"/bin/ls", NULL};
	char	*t3c2[] = {"cat", NULL};
	char	*t3c3[] = {"cat", NULL};
	char	*t3c4[] = {"cat", NULL};
	char	*t3c5[] = {"/usr/bin/grep", "picoshell", NULL};
	char	**t3[] = {t3c1, t3c2, t3c3, t3c4, t3c5, NULL};
	picoshell(t3);

	printf("\n=== Test 4: ls | grep .c | wc -l ===\n");
	char	*t4c1[] = {"/bin/ls", NULL};
	char	*t4c2[] = {"/usr/bin/grep", ".c", NULL};
	char	*t4c3[] = {"/usr/bin/wc", "-l", NULL};
	char	**t4[] = {t4c1, t4c2, t4c3, NULL};
	picoshell(t4);

	printf("\n=== Test 5: single command ===\n");
	char	*t5c1[] = {"echo", "single command works", NULL};
	char	**t5[] = {t5c1, NULL};
	picoshell(t5);

	printf("\n=== Test 6: empty cmds (should return 0, no output) ===\n");
	char	**t6[] = {NULL};
	int ret = picoshell(t6);
	printf("returned: %d\n", ret);

	printf("\n=== Test 7: invalid command (should return 1) ===\n");
	char	*t7c1[] = {"/bin/ls", NULL};
	char	*t7c2[] = {"notacommand_xyz", NULL};
	char	**t7[] = {t7c1, t7c2, NULL};
	ret = picoshell(t7);
	printf("returned: %d\n", ret);

	printf("\n=== Test 8: very long pipe (7 commands) ===\n");
	char	*t8c1[] = {"echo", "hello world foo bar baz", NULL};
	char	*t8c2[] = {"cat", NULL};
	char	*t8c3[] = {"cat", NULL};
	char	*t8c4[] = {"tr", " ", "\n", NULL};
	char	*t8c5[] = {"cat", NULL};
	char	*t8c6[] = {"cat", NULL};
	char	*t8c7[] = {"/usr/bin/wc", "-l", NULL};
	char	**t8[] = {t8c1, t8c2, t8c3, t8c4, t8c5, t8c6, t8c7, NULL};
	picoshell(t8);

	return (0);
}
