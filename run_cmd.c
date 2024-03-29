#include "shell.h"

/**
 * run_cmd - a function that runs a command within a file
 * @file_path: the path to the file
 * @exec_ret: the return value of the last executed command
 * Return: the value of the last command ran, 127 if the file could not open,
 * -1 if memory allocation fails
 */
int run_cmd(char *file_path, int *exec_ret)
{
	char buffer[120], *line, **args, **front;
	ssize_t file, bytes_read, j;
	unsigned int line_size = 0, old_size = 120;
	int ret_val;

	file = open(file_path, O_RDONLY);
	if (file == -1)
	{
		*exec_ret = run_cmd_error(file_path);
		return (*exec_ret);
	}
	line = malloc(sizeof(char) * old_size);
	if (!line)
		return (-1);
	do {
		bytes_read = read(file, buffer, 119);
		if (bytes_read == 0 && line_size == 0)
			return (*exec_ret);
		buffer[bytes_read] = '\0', line_size += bytes_read;
		line = _realloc(line, old_size, line_size);
		_strcat(line, buffer), old_size = line_size;
	} while (bytes_read);
	for (j = 0; line[j] == '\n'; j++)
		line[j] = ' ';
	for (; j < line_size; j++)
	{
		if (line[j] == '\n')
		{
			line[j] = ';';
			for (j += 1; j < line_size && line[j] == '\n'; j++)
				line[j] = ' ';
		}
	}
	change_var(&line, exec_ret), handle_line(&line, line_size);
	args = str_tok(line, " "), free(line);
	if (!args)
		return (0);
	if (check_args(args) != 0)
	{
		*exec_ret = 2, args_free(args, args);
		return (*exec_ret);
	}
	front = args;
	for (j = 0; args[j]; j++)
	{
		if (_strncmp(args[j], ";", 1) == 0)
		{
			free(args[j]), args[j] = NULL;
			ret_val = call_args(args, front, exec_ret);
			args = &args[++j];
			j = 0;
		}
	}
	ret_val = call_args(args, front, exec_ret);
	free(front);

	return (ret_val);
}
