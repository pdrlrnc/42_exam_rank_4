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
char	*get_next_line(int fd);
char	*ft_strchr(const char *s, int c);
size_t	ft_strlen(const char *str);


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
			{
				close(fd[0]);
				close(fd[1]);
				exit(1);
			}
		}
		if (type == 'w')
		{
			if (dup2(fd[0], STDIN_FILENO) == -1)
			{
				close(fd[0]);
				close(fd[1]);
				exit(1);
			}
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
			close(fd[0]);
			return (fd[1]);
		}

	}
	
}

int	main() {
	char *line;
	int	fd = ft_popen("ls", (char *const []){"ls", NULL}, 'r');
	while ((line = get_next_line(fd)))
		printf("%s", line);
	return (fd);
}

size_t	ft_strlen(const char *str)
{
	size_t	len;

	if (!str)
		return (0);
	len = 0;
	while (*(str + len))
		len++;
	return (len);
}

char	*ft_strchr(const char *s, int c)
{
	int	i;

	c = c % 256;
	i = 0;
	while (*(s + i))
	{
		if (*(s + i) == c)
			return ((char *)(s + i));
		i++;
	}
	if (c == 0)
		return ((char *)(s + i));
	return (NULL);
}

static char	*gnl_realloc(char *result, int i, int *r)
{
	char	*new_result;

	*r = 0;
	if (result)
		while (*(result + *r))
			(*r)++;
	new_result = malloc(i + *r + 2);
	if (!new_result)
	{
		if (result)
			free(result);
		return (NULL);
	}
	if (!result)
		return (new_result);
	*r = 0;
	while (*(result + *r))
	{
		*(new_result + *r) = *(result + *r);
		(*r)++;
	}
	free(result);
	return (new_result);
}

static char	*ft_handle_new_line(char *buff, char *result, int i)
{
	int	buffer_index;
	int	result_index;

	result = gnl_realloc(result, i, &result_index);
	if (!result)
		return (NULL);
	buffer_index = 0;
	while (buffer_index <= i)
	{
		*(result + result_index) = *(buff + buffer_index);
		*(buff + buffer_index) = '\0';
		buffer_index++;
		result_index++;
	}
	*(result + result_index) = '\0';
	return (result);
}

static char	*ft_process_buffer(char *buff, int i)
{
	int	start;

	start = 0;
	while (*(buff + i) && i < 20)
	{
		*(buff + start) = *(buff + i);
		*(buff + i) = '\0';
		i++;
		start++;
	}
	return (buff);
}

static char	*get_next_line_cont(int fd, char *buffer,
				char *result, int bytes_read)
{
	int	i;

	result = NULL;
	while (bytes_read)
	{
		if (bytes_read != -2)
		{
			if (ft_strchr(buffer, '\n'))
				i = ft_strchr(buffer, '\n') - buffer;
			else
				i = ft_strlen(buffer) - 1;
			result = ft_handle_new_line(buffer, result, i);
			if (!result)
				return (NULL);
			buffer = ft_process_buffer(buffer, i + 1);
			if (ft_strchr(result, '\n'))
				return (result);
		}
		bytes_read = read(fd, buffer, 20);
		if (bytes_read == -1)
			return (free(result), NULL);
		buffer[bytes_read] = '\0';
	}
	return (result);
}

char	*get_next_line(int fd)
{
	static char	buffer[20];
	char		*result;
	int			bytes_read;

	result = NULL;
	bytes_read = 1;
	if (!buffer[0])
		bytes_read = -2;
	if (fd < 0 || fd >= FOPEN_MAX)
		return (NULL);
	return (get_next_line_cont(fd, buffer, result, bytes_read));
}
