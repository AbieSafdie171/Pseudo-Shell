#include <stdio.h>
#include <stdlib.h>
#include <string.h>    // strcmp
#include <sys/stat.h>  // mkdir, chdir
#include <sys/types.h> // mkdir, chdir
#include <sys/syscall.h> 
#include <unistd.h>	   // getcwd
#include <dirent.h>	   // dir struct
#include <fcntl.h>     // open()
#include "string_parser.h"


// ls command
void listDir(){

	DIR *dir;

	char cwd[1024];
	getcwd(cwd, sizeof(cwd));

	dir = opendir(cwd);

	struct dirent *read_dir;

	// read thorugh our dir. printing contents
	while ((read_dir = readdir(dir)) != NULL){

		write(STDOUT_FILENO, read_dir->d_name, strlen(read_dir->d_name));
		write(STDOUT_FILENO, "\n", strlen("\n"));
	}

	closedir(dir);

}

// pwd command
void showCurrentDir(){
	//char *cwd;
	// char *cwd = (char *)malloc(sizeof(char) * 1024);

	char cwd[1024];
	getcwd(cwd, sizeof(cwd));		// gets our pwd
	write(STDOUT_FILENO, cwd, strlen(cwd));		
	write(STDOUT_FILENO, "\n", strlen("\n"));

}

// mkdir command
void makeDir(char *dirName){

	DIR *dir;
	int error = 0;
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));

	dir = opendir(cwd);

	struct dirent *read_dir;

	
	// loops through dir looking if directory already exists
	while ((read_dir = readdir(dir)) != NULL){

		if (strcmp(read_dir->d_name, dirName) == 0){
			write (STDOUT_FILENO, "Error. Directory already exists\n\0", strlen("Error. Directory already exists\n\0"));
			error = 1;
		}
		
	}

	closedir(dir);

	// else make the dir
	if (error == 0)
		mkdir(dirName, 0777);

}

// cd command
void changeDir(char *dirName){

	// check if dir exists
	if (chdir(dirName) != 0){
		write (STDOUT_FILENO, "Error. Directory does not exist\n\0", strlen("Error. Directory does not exist\n\0"));
	}

}

// cp command
void copyFile(char *sourcePath, char *destinationPath){
	DIR *dir;

	// get our current dir path
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));

	// struct for our read directory
	struct dirent *read_dir;

	// set error (no file found) to true
	int error = 1;

	// copy source path to new stirng that we will tokenize
	char *copy_source_path = (char *)malloc(1 + strlen(sourcePath));

	strcpy(copy_source_path, sourcePath);

	copy_source_path[strlen(sourcePath)] = '\0';

	command_line file_to_copy;	// change this to malloc
	file_to_copy = str_filler(copy_source_path, "/");

	// update our path to what we are copying to
	char *dir_to_read = (char *)malloc(sizeof(char) * (2 + strlen(cwd) + strlen(sourcePath)));
	strcpy(dir_to_read, cwd);
	for (int i = 0; i < file_to_copy.num_token - 1; ++i)
	{
		strcat(dir_to_read, "/");
		strcat(dir_to_read, file_to_copy.command_list[i]);

	}

	dir_to_read[1 + strlen(cwd) + strlen(sourcePath)] = '\0';


	// open the desired directory
	dir = opendir(dir_to_read);
	


	// read through directory to see if file exists
	while ((read_dir = readdir(dir)) != NULL){

		// if we find the file
		if (strcmp(read_dir->d_name, file_to_copy.command_list[file_to_copy.num_token - 1]) == 0){
			error = 0;	// set file found
			// get our file path, and open file for reading
			char *file_path = (char *)malloc(sizeof(char) * (2 + strlen(cwd) + strlen(sourcePath)));
			char *dest_path = (char *)malloc(sizeof(char) * (3 + strlen(cwd) + strlen(destinationPath) + strlen(sourcePath)));
			strcpy(file_path, cwd);
			strcpy(dest_path, cwd);
			strcat(file_path, "/");
			strcat(dest_path, "/");
			strcat(file_path, sourcePath);
			strcat(dest_path, destinationPath);
			strcat(dest_path, "/");

			strcat(dest_path, file_to_copy.command_list[file_to_copy.num_token - 1]);

			file_path[1 + strlen(cwd) + strlen(sourcePath)] = '\0';
			dest_path[2 + strlen(cwd) + strlen(destinationPath) + strlen(sourcePath)] = '\0';



			int file_id = open(file_path, O_RDONLY);

			// Checks if file is a directory
			
			struct stat file_stat;
			stat(file_path, &file_stat);
			if (!(S_ISREG(file_stat.st_mode))){
				write(STDOUT_FILENO, "Error. Is directory\n\0", strlen("Error. Is directory\n\0"));
				free(file_path);
				free(dest_path);
				close(file_id);
				break;
			}
			
			// change output stream
			FILE *out_f = freopen(dest_path, "w", stdout);

			// Reading from file and writing to file
			char buffer[1024];
			size_t bytes;
			do {
				bytes = read(file_id, buffer, sizeof(buffer));
				write(STDOUT_FILENO, buffer, bytes);
			} while (bytes > 0);
			// close the file

			close(file_id);

			// redirect back to stdout
			freopen("/dev/tty", "a", stdout);

			free(file_path);
			free(dest_path);

			break;
		}

	}
	// if no file exists, write error
	if (error)
		write(STDOUT_FILENO, "Error. No existing file\n\0", strlen("Error. No existing file\n\0"));

	// close the dir
	closedir(dir);
	free(dir_to_read);
	free(copy_source_path);
	free_command_line(&file_to_copy);
}

// mv command
void moveFile(char *sourcePath, char *destinationPath){
	DIR *dir;

	// get our current dir path
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	
	// struct for our read directory
	struct dirent *read_dir;

	// set error (no file found) to true
	int error = 1;

	// copy our source path to string we will tokenize
	char *copy_source_path = (char *)malloc(strlen(sourcePath) + 1);
	strcpy(copy_source_path, sourcePath);
	copy_source_path[strlen(sourcePath)] = '\0';
	

	command_line file_to_copy = str_filler(copy_source_path, "/");

	// setting the desired directory to use to dir_to_read
	char *dir_to_read = (char *)malloc(sizeof(char) * (2 + strlen(cwd) + strlen(sourcePath)));
	strcpy(dir_to_read, cwd);
	for (int i = 0; i < file_to_copy.num_token - 1; ++i)
	{
		strcat(dir_to_read, "/");
		strcat(dir_to_read, file_to_copy.command_list[i]);
	}

	dir_to_read[1 + strlen(cwd) + strlen(sourcePath)] = '\0';

	// open the desired directory
	dir = opendir(dir_to_read);
	
	// read through directory to see if file exists
	while ((read_dir = readdir(dir)) != NULL){

		// find the file we want to copy
		if (strcmp(read_dir->d_name, file_to_copy.command_list[file_to_copy.num_token - 1]) == 0){
			error = 0;	// set file found
			// get our file path, and open file for reading
			char *file_path = (char *)malloc(sizeof(char) * (2 + strlen(cwd) + strlen(sourcePath)));
			char *dest_path = (char *)malloc(sizeof(char) * (3 + strlen(cwd) + strlen(destinationPath) + strlen(file_to_copy.command_list[file_to_copy.num_token - 1])));
			strcpy(file_path, cwd);
			strcpy(dest_path, cwd);
			strcat(file_path, "/");
			strcat(dest_path, "/");
			strcat(file_path, sourcePath);
			strcat(dest_path, destinationPath);

			file_path[1 + strlen(cwd) + strlen(sourcePath)] = '\0';
			
			
		    struct stat path_stat;
		    if (stat(dest_path, &path_stat) != 0) {
		    	int rename = 1;	// means we found no existing file location, so we will rename
		    } 
		    // else if its a dir, append the name of the original file
		    else if (S_ISDIR(path_stat.st_mode)) {
		        strcat(dest_path, "/");
				strcat(dest_path, file_to_copy.command_list[file_to_copy.num_token - 1]);
		    }
			
			dest_path[2 + strlen(cwd) + strlen(destinationPath) + strlen(file_to_copy.command_list[file_to_copy.num_token - 1])] = '\0';



			int file_id = open(file_path, O_RDONLY);

			// Checks if file is a directory
			struct stat file_stat2;
			stat(file_path, &file_stat2);
			if (!(S_ISREG(file_stat2.st_mode))){
				write(STDOUT_FILENO, "Error. Is directory\n\0", strlen("Error. Is directory\n\0"));
				free(file_path);
				free(dest_path);
				close(file_id);
				break;
			}
			

			// write to new file
			FILE *out_f = freopen(dest_path, "w", stdout);

			

			// Reading from file and writing to file
			char buffer[1024];
			size_t bytes;
			do {
				bytes = read(file_id, buffer, sizeof(buffer));
				write(STDOUT_FILENO, buffer, bytes);
			} while (bytes > 0);
			// close the file

			// deleting the file via unlink
			unlink(file_path);
			close(file_id);

			free(file_path);
			free(dest_path);

			freopen("/dev/tty", "a", stdout);

			break;
		}

	}
	// if no file exists, write error
	if (error)
		write(STDOUT_FILENO, "Error. No existing file\n\0", strlen("Error. No existing file\n\0"));

	closedir(dir);
	free(dir_to_read);
	free(copy_source_path);
	free_command_line(&file_to_copy);
}

// rm command
void deleteFile(char *filename){
	DIR *dir;

	// get our current dir path
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));

	// struct for our read directory
	struct dirent *read_dir;

	// set error (no file found) to true
	int error = 1;

	// same logic as mv function
	char *copy_source_path = (char *)malloc(1 + strlen(filename));

	strcpy(copy_source_path, filename);

	copy_source_path[strlen(filename)] = '\0';

	command_line file_to_delete;	// change this to malloc
	file_to_delete = str_filler(filename, "/");

	// setting the desired directory to use to dir_to_read
	char *dir_to_read = (char *)malloc(sizeof(char) * (2 + strlen(cwd) + strlen(filename)));
	strcpy(dir_to_read, cwd);
	for (int i = 0; i < file_to_delete.num_token - 1; ++i)
	{
		strcat(dir_to_read, "/");
		strcat(dir_to_read, file_to_delete.command_list[i]);

	}

	dir_to_read[1 + strlen(cwd) + strlen(filename)] = '\0';

	// open the desired directory
	dir = opendir(dir_to_read);



	// read through directory to see if file exists
	while ((read_dir = readdir(dir)) != NULL){

		// if we find the file
		if (strcmp(read_dir->d_name, file_to_delete.command_list[file_to_delete.num_token - 1]) == 0){
			error = 0;	// set file found
			// get our file path, and open file for reading
			char *file_path = (char *)malloc(sizeof(char) * (2 + strlen(cwd) + strlen(filename)));
			strcpy(file_path, cwd);
			for (int i = 0; i < file_to_delete.num_token; ++i)
			{
				strcat(file_path, "/");
				strcat(file_path, file_to_delete.command_list[i]);

			}

			file_path[1 + strlen(cwd) + strlen(filename)] = '\0';

			// Checks if file is a directory
			struct stat file_stat;
			stat(file_path, &file_stat);
			if (!(S_ISREG(file_stat.st_mode))){
				write(STDOUT_FILENO, "Error. Is directory\n\0", strlen("Error. Is directory\n\0"));
				free(file_path);
				break;
			}

			// Deleting file via unlink
			unlink(file_path);
			free(file_path);

			break;
			
		}
			
	}
	// if no file exists, write error
	if (error)
		write(STDOUT_FILENO, "Error. No existing file\n\0", strlen("Error. No existing file\n\0"));

	free(copy_source_path);
	// close directory
	closedir(dir);
	free(dir_to_read);
	free_command_line(&file_to_delete);

}

// cat command
void displayFile(char *filename){
	// open, read, write
	DIR *dir;

	// get our current dir path
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));

	// same logic
	char *copy_source_path = (char *)malloc(1 + strlen(filename));

	strcpy(copy_source_path, filename);
	copy_source_path[strlen(filename)] = '\0';

	command_line file_to_read;	// change this to malloc
	file_to_read = str_filler(filename, "/");

	// setting the desired directory to use to dir_to_read
	char *dir_to_read = (char *)malloc(sizeof(char) * (2 + strlen(cwd) + strlen(filename)));
	strcpy(dir_to_read, cwd);
	for (int i = 0; i < file_to_read.num_token - 1; ++i)
	{
		strcat(dir_to_read, "/");
		strcat(dir_to_read, file_to_read.command_list[i]);

	}

	// open the directory
	dir = opendir(dir_to_read);

	dir_to_read[1 + strlen(cwd) + strlen(filename)] = '\0';

	// struct for our read directory
	struct dirent *read_dir;

	// set error (no file found) to true
	int error = 1;

	// read through directory to see if file exists
	while ((read_dir = readdir(dir)) != NULL){

		// if we find the file
		if (strcmp(read_dir->d_name, file_to_read.command_list[file_to_read.num_token - 1]) == 0){
			error = 0;	// set file found
			// get our file path, and open file for reading
			char *file_path = (char *)malloc(sizeof(char) * (2 + strlen(cwd) + strlen(filename)));
			strcpy(file_path, cwd);
			strcat(file_path, "/");
			strcat(file_path, filename);
			int file_id = open(file_path, O_RDONLY);

			file_path[1 + strlen(cwd) + strlen(filename)] = '\0';

			// Checks if file is a directory
			struct stat file_stat;
			stat(file_path, &file_stat);
			if (!(S_ISREG(file_stat.st_mode))){
				write(STDOUT_FILENO, "Error. Is directory\n\0", strlen("Error. Is directory\n\0"));
				free(file_path);
				close(file_id);
				break;
			}

			// Reading from file and writing to STDOUT
			char buffer[1024];
			size_t bytes;
			do {
				bytes = read(file_id, buffer, sizeof(buffer));
				write(STDOUT_FILENO, buffer, bytes);
			} while (bytes > 0);
				
			// close the file
			close(file_id);

			write(STDOUT_FILENO, "\n", strlen("\n"));

			free(file_path);
			break;
		}
			
	}
	// if no file exists, write error
	if (error)
		write(STDOUT_FILENO, "Error. No existing file\n\0", strlen("Error. No existing file\n\0"));

	free(copy_source_path);
	// close directory
	closedir(dir);
	free(dir_to_read);
	free_command_line(&file_to_read);
}


