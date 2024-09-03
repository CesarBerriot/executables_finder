#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include "utils/utils.h"

char ** files = NULL;
int files_len = 0;

void parse_directory(char * path);

void copy_files(char * input_directory, char * output_directory);

int main(int args_len, char ** args)
{	if(args_len != 3)
		hard_fail("expected 2 arguments, got %i", args_len - 1);
	system("cls");
	puts("parsing...");
	parse_directory(args[1]);
	printf("found %i files\n", files_len);
	puts("copying...");
	copy_files(args[1], args[2]);
	system("pause");
	return 0;
}

void parse_directory(char * path)
{	DIR * directory = opendir(path);
	if(!directory)
		hard_fail("failed to open directory for parsing '%s'", path);
	struct dirent * entry = NULL;
	// skip . and ..
	readdir(directory);
	readdir(directory);
	while(entry = readdir(directory))
	{	char file_path[PATH_MAX];
		sprintf(file_path, "%s/%s", path, entry->d_name);
		struct stat file_info;
		if(stat(file_path, &file_info))
			hard_fail("stat() failed on file '%s'", file_path);
		else
		{	if(S_ISDIR(file_info.st_mode))
				parse_directory(file_path);
			else
				if(entry->d_namlen >= 4)
				{	char * extension = entry->d_name + (entry->d_namlen - 4);
					if(!strcmp(extension, ".exe") || !strcmp(extension, ".dll"))
					{	++files_len;
						files = realloc(files, files_len * sizeof(char*));
						files[files_len - 1] = strdup(file_path);
					}
				}
		}
	}
	closedir(directory);
}

void copy_files(char * input_directory, char * output_directory)
{	if(access(output_directory, F_OK))
		if(errno != ENOENT)
			hard_fail("failed to access output directory '%s'", output_directory);
		else
			if(mkdir(output_directory))
				hard_fail("failed to create output directory '%s'", output_directory);
	if(access(output_directory, W_OK))
		if(chmod(output_directory, S_IRUSR | S_IWUSR))
			hard_fail("failed to modify permissions of output directory '%s'", output_directory);

	char base_path[PATH_MAX];
	int base_bath_len = 0;
	{	int min = fmin(strlen(input_directory), strlen(output_directory));
		for(int i = 0; i < min; ++i)
			if(input_directory[i] == output_directory[i])
			{	base_path[i] = input_directory[i];
				++base_bath_len;
			}
			else
				break;
		base_path[base_bath_len] = '\0';
	}

	int success_count = 0, fail_count = 0;
	double bytes_copied_count = 0;
	#define log_progress() do { printf("\033[4;0H%i/%i files copied - %i failed\n", success_count, files_len, fail_count); } while(false)
	#define iteration_fail(...) { soft_fail(true, __VA_ARGS__); ++fail_count; log_progress(); continue; } 0
	#define iteration_success(...) do { ++success_count; log_progress(); } while(false)
	for(int i = 0; i < files_len; ++i)
	{	char to_base_name[PATH_MAX];
		int to_base_name_len;
		{	char * reduced_path = files[i] + base_bath_len;
			int reduced_path_len = strlen(reduced_path);
			memcpy(to_base_name, reduced_path, reduced_path_len + 1);
			to_base_name_len = reduced_path_len;
			for(int i2 = 0; i2 < to_base_name_len; i2++)
				if(to_base_name[i2] == '/' || to_base_name[i2] == '\\')
					to_base_name[i2] = '.';
		}

		if(to_base_name_len + strlen(output_directory) + 2 > PATH_MAX)
			iteration_fail("path length exceeds PATH_MAX (%i characters). file skipped. name : '%s'", PATH_MAX - 1, to_base_name);

		char to_path[PATH_MAX];
		sprintf(to_path, "%s/%s", output_directory, to_base_name);

		int from = open(files[i], O_RDONLY | O_BINARY);
		int to = creat(to_path, S_IRUSR | S_IWUSR);
		if(from == -1)
			iteration_fail("failed to open file with read permissions '%s'. file skipped.", files[i]);
		if(to == -1)
			iteration_fail("failed to create output file '%s'. file skipped.", to_base_name);

		lseek(from, 0, SEEK_END);
		int from_size = tell(from);
		lseek(from, 0, SEEK_SET);

		if(!from_size)
			iteration_fail("failed to acquire physical size of file '%s'. file skipped.", files[i]);

		void * from_bytes = malloc(from_size);
		if(!from_bytes)
			iteration_fail("file bytes buffer allocation (%i bytes) failed for file '%s'. file skipped.", from_size, files[i]);

		{	int read_result = read(from, from_bytes, from_size);
			if(read_result == -1)
			{	free(from_bytes);
				iteration_fail("failed to read data (%i bytes) from file '%s'. file skipped.", from_size, files[i]);
			}
			else if (read_result < from_size)
				iteration_fail("insufficient data read from file '%s' (expected %i bytes, got %i - %i missing). file skipped.", files[i], from_size, read_result, from_size - read_result);
		}

		{	int write_result = write(to, from_bytes, from_size);
			if(write_result == -1)
			{	free(from_bytes);
				iteration_fail("failed to write data to file '%s'. file corrupted.", to_base_name);
			}
			else if (write_result < from_size)
			iteration_fail("insufficient data written to file '%s' (expected %i bytes, got %i - %i missing). file skipped.", to_base_name, from_size, write_result, from_size - write_result);
		}


		free(from_bytes);

		bytes_copied_count += from_size / 1024. / 1024.;

		close(from);
		close(to);

		iteration_success();
	}
	release_delayed_log();
	printf("copied %.2fMB\n", (int)(bytes_copied_count * 100) / 100.);
	#undef log_progress
	#undef iteration_fail
	#undef iteration_success
}
