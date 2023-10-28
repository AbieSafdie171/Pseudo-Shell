#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>  // mkdir, chdir
#include <sys/types.h> // mkdir, chdir
#include <sys/syscall.h> 
#include <dirent.h>	   // dir struct
#include <fcntl.h>     // open()
#include "command.h"
#include "string_parser.h"

int main(int argc, char* argv[]){

	// checks how many input args for file mode use
	if (argc > 1){

		if (argc == 2){
			write(STDOUT_FILENO, "Error. No file specified\n\0", strlen("Error. No file specified\n\0"));
			return 0;
		}

		if (strcmp(argv[1], "-f") == 0){

				DIR *dir;

				// get our current dir path
				char cwd[1024];
				getcwd(cwd, sizeof(cwd));

				// open the directory
				dir = opendir(cwd);

				// struct for our read directory
				struct dirent *read_dir;

				// set error (no file found) to true
				int file_error = 1;

				// read through directory to see if file exists
				while ((read_dir = readdir(dir)) != NULL){

					// if we find the file
					if (strcmp(read_dir->d_name, argv[2]) == 0){
						file_error = 0;	// set file found
						// get our file path and output file path, and open file for reading
						char *file_path = (char *)malloc(2 + strlen(cwd) + strlen(argv[2]));
						char *output_path = (char *)malloc(2 + strlen(cwd) + strlen("output.txt"));
						strcpy(file_path, cwd);
						strcpy(output_path, cwd);
						strcat(output_path, "/");
						strcat(file_path, "/");
						strcat(file_path, argv[2]);
						strcat(output_path, "output.txt");
						file_path[1 + strlen(cwd) + strlen(argv[2])] = '\0';
						output_path[1 + strlen(cwd) + strlen("output.txt")] = '\0';

						// redirect the output stream
						FILE *out_f = freopen(output_path, "w", stdout);

						// Checks if file is a directory
						struct stat file_stat;
						stat(file_path, &file_stat);
						if (!(S_ISREG(file_stat.st_mode))){
							write(STDOUT_FILENO, "Error. Is directory\n\0", strlen("Error. Is directory\n\0"));
							break;
						}

						// open the input file for reading
						char *input = NULL;
						size_t len_input = 0;
						ssize_t read_bytes;
						FILE *f;
						f = fopen(file_path, "r");
						free(file_path);

						// loop through the input file
						while ((read_bytes = getline(&input, &len_input, f)) != -1){

							command_line token_buffer;			// change this to malloc
							token_buffer = str_filler(input, ";");

							// run through tokens
							for (int i = 0; i < token_buffer.num_token; ++i)
							{
								int error = 0;
								command_line small_token_buffer;		// change this to malloc
								small_token_buffer = str_filler(token_buffer.command_list[i], " ");

								for (int j = 0; j < small_token_buffer.num_token; ++j)
								{

									freopen(output_path, "a", stdout); // change redirect back to output

									if (strcmp(small_token_buffer.command_list[j], "ls") == 0){
										if (small_token_buffer.num_token != 1){
											write(STDOUT_FILENO, "Error! Unsupported parameters for ls command\n", strlen("Error! Unsupported parameters for ls command\n"));
											break;
										}
										// call ls function
										listDir();
									} else if (strcmp(small_token_buffer.command_list[j], "pwd") == 0){
										if (small_token_buffer.num_token != 1){
											write(STDOUT_FILENO, "Error! Unsupported parameters for pwd command\n", strlen("Error! Unsupported parameters for pwd command\n"));
											break;
										}
										// call pwd function
										showCurrentDir();
									} else if (strcmp(small_token_buffer.command_list[j], "mkdir") == 0){
										if (small_token_buffer.num_token != 2){
											write(STDOUT_FILENO, "Error! Unsupported parameters for mkdir command\n", strlen("Error! Unsupported parameters for mkdir command\n"));
											break;
										}
										// call mkdir function
										makeDir(small_token_buffer.command_list[1]);
									} else if (strcmp(small_token_buffer.command_list[j], "cd") == 0){
										if (small_token_buffer.num_token != 2){
											write(STDOUT_FILENO, "Error! Unsupported parameters for cd command\n", strlen("Error! Unsupported parameters for cd command\n"));
											break;
										}
										// call cd function
										changeDir(small_token_buffer.command_list[1]);
									} else if (strcmp(small_token_buffer.command_list[j], "cp") == 0){
										if (small_token_buffer.num_token != 3){
											write(STDOUT_FILENO, "Error! Unsupported parameters for cp command\n", strlen("Error! Unsupported parameters for cp command\n"));
											break;
										}
										// call cp function
										copyFile(small_token_buffer.command_list[1], small_token_buffer.command_list[2]);
									} else if (strcmp(small_token_buffer.command_list[j], "mv") == 0){
										if (small_token_buffer.num_token != 3){
											write(STDOUT_FILENO, "Error! Unsupported parameters for mv command\n", strlen("Error! Unsupported parameters for mv command\n"));
											break;
										}
										// call mv function
										moveFile(small_token_buffer.command_list[1], small_token_buffer.command_list[2]);
									} else if (strcmp(small_token_buffer.command_list[j], "rm") == 0){
										if (small_token_buffer.num_token != 2){
											write(STDOUT_FILENO, "Error! Unsupported parameters for rm command\n", strlen("Error! Unsupported parameters for rm command\n"));
											break;
										}
										// call rm function
										deleteFile(small_token_buffer.command_list[1]);
									} else if (strcmp(small_token_buffer.command_list[j], "cat") == 0){
										if (small_token_buffer.num_token != 2){
											write(STDOUT_FILENO, "Error! Unsupported parameters for cat command\n", strlen("Error! Unsupported parameters for cat command\n"));
											break;
										}
										// call cat function
										displayFile(small_token_buffer.command_list[1]);
									} else {
										if (strcmp(small_token_buffer.command_list[j], "exit") != 0){
											write(STDOUT_FILENO, "Error unrecognized command: ", strlen("Error unrecognized command: "));
											write(STDOUT_FILENO, small_token_buffer.command_list[j], strlen(small_token_buffer.command_list[j]));
											write(STDOUT_FILENO, "\n", strlen("\n"));
											error = 1;
											break;
										}
									}

									
									break;


								}	// inner for

								free_command_line(&small_token_buffer);

								if (error){
										break;
									}

								
							}	// outer for
							free_command_line(&token_buffer);

						}	// second while

						// freeing memory
						fclose(f);
						free(input);
						freopen(output_path, "a", stdout);
						free(output_path);
						write(STDOUT_FILENO, "End of File\nHave an amazing day:)\n", strlen("End of File\nHave an amazing day:)\n"));

				}	// first while
			}

			// if there are errors
			if (file_error){
				write(STDOUT_FILENO, "Error. No existing file\n\0", strlen("Error. No existing file\n\0"));
			}

			closedir(dir);

		} else {
			write(STDOUT_FILENO, "Error. No -f specified\n\0", strlen("Error. No -f specified\n\0"));
		}

		

	} else {
		// interactive mode
		while (1){

			// input text
			char *input = NULL;		
			size_t len_input = 0;	

			// allocate and write data for arrows for pseudoshell
			char *arrows = (char *)malloc(sizeof(char) * 5);	
			strcpy(arrows, ">>> ");
			arrows[strlen(arrows)] = '\0';
			write(STDOUT_FILENO, arrows, strlen(arrows));
			free(arrows);

			
			// get input using getline
			getline(&input, &len_input, stdin);

			// store our tokens
			command_line token_buffer;	
			token_buffer = str_filler(input, ";");

			// run through our tokens
			for (int i = 0; i < token_buffer.num_token; ++i)
			{
				int error = 0;
				command_line small_token_buffer;
				small_token_buffer = str_filler(token_buffer.command_list[i], " ");

				for (int j = 0; j < small_token_buffer.num_token; ++j)
				{

					if (strcmp(small_token_buffer.command_list[j], "ls") == 0){
						if (small_token_buffer.num_token != 1){
							write(STDOUT_FILENO, "Error! Unsupported parameters for ls command\n", strlen("Error! Unsupported parameters for ls command\n"));
							break;
						}
						// call ls function
						listDir();
					} else if (strcmp(small_token_buffer.command_list[j], "pwd") == 0){
						if (small_token_buffer.num_token != 1){
							write(STDOUT_FILENO, "Error! Unsupported parameters for pwd command\n", strlen("Error! Unsupported parameters for pwd command\n"));
							break;
						}
						// call pwd function
						showCurrentDir();
					} else if (strcmp(small_token_buffer.command_list[j], "mkdir") == 0){
						if (small_token_buffer.num_token != 2){
							write(STDOUT_FILENO, "Error! Unsupported parameters for mkdir command\n", strlen("Error! Unsupported parameters for mkdir command\n"));
							break;
						}
						// call mkdir function
						makeDir(small_token_buffer.command_list[1]);
					} else if (strcmp(small_token_buffer.command_list[j], "cd") == 0){
						if (small_token_buffer.num_token != 2){
							write(STDOUT_FILENO, "Error! Unsupported parameters for cd command\n", strlen("Error! Unsupported parameters for cd command\n"));
							break;
						}
						// call cd function
						changeDir(small_token_buffer.command_list[1]);
					} else if (strcmp(small_token_buffer.command_list[j], "cp") == 0){
						if (small_token_buffer.num_token != 3){
							write(STDOUT_FILENO, "Error! Unsupported parameters for cp command\n", strlen("Error! Unsupported parameters for cp command\n"));
							break;
						}
						// call cp function
						copyFile(small_token_buffer.command_list[1], small_token_buffer.command_list[2]);
					} else if (strcmp(small_token_buffer.command_list[j], "mv") == 0){
						if (small_token_buffer.num_token != 3){
							write(STDOUT_FILENO, "Error! Unsupported parameters for mv command\n", strlen("Error! Unsupported parameters for mv command\n"));
							break;
						}
						// call mv function
						moveFile(small_token_buffer.command_list[1], small_token_buffer.command_list[2]);
					} else if (strcmp(small_token_buffer.command_list[j], "rm") == 0){
						if (small_token_buffer.num_token != 2){
							write(STDOUT_FILENO, "Error! Unsupported parameters for rm command\n", strlen("Error! Unsupported parameters for rm command\n"));
							break;
						}
						// call rm function
						deleteFile(small_token_buffer.command_list[1]);
					} else if (strcmp(small_token_buffer.command_list[j], "cat") == 0){
						if (small_token_buffer.num_token != 2){
							write(STDOUT_FILENO, "Error! Unsupported parameters for cat command\n", strlen("Error! Unsupported parameters for cat command\n"));
							break;
						}
						// call cat function
						displayFile(small_token_buffer.command_list[1]);
					} else {
						if (strcmp(small_token_buffer.command_list[j], "exit") != 0){
							write(STDOUT_FILENO, "Error unrecognized command: ", strlen("Error unrecognized command: "));
							write(STDOUT_FILENO, small_token_buffer.command_list[j], strlen(small_token_buffer.command_list[j]));
							write(STDOUT_FILENO, "\n", strlen("\n"));
							error = 1;
							break;
						}
					}

					break;


				}

				free_command_line(&small_token_buffer);

				if (error){
						break;
					}

				
			}

			free_command_line(&token_buffer);


			if (strcmp(input, "exit") == 0){
				free(input);
				break;
			}

			free(input);

		}	// while

	}		// else
	}		// main