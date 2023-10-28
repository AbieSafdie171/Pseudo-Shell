/*
 * string_parser.c
 *
 *  Created on: Nov 25, 2020
 *      Author: gguan, Monil
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_parser.h"

#define _GUN_SOURCE


int count_token (char* buf, const char* delim)
{
	//TODO：
	/*
	*	#1.	Check for NULL string
	*	#2.	iterate through string counting tokens
	*		Cases to watchout for
	*			a.	string start with delimeter
	*			b. 	string end with delimeter
	*			c.	account NULL for the last token
	*	#3. return the number of token (note not number of delimeter)
	*/

	// Checking for NULL string
	if (buf == NULL)
		return 0;

	int count = 0;

	char *ptr; char *token;
	
	token = strtok_r(buf, delim, &ptr);

	while (token != NULL){
		count++;
		// fprintf(stderr, "token %d:[%s]\n",count, token);
		token = strtok_r(NULL, delim, &ptr);
	}

	return count;


}

command_line str_filler (char* buf, const char* delim)
{
	//TODO：
	/*
	*	#1.	create command_line variable to be filled and returned
	*	#2.	count the number of tokens with count_token function, set num_token. 
    *           one can use strtok_r to remove the \n at the end of the line.
	*	#3. malloc memory for token array inside command_line variable
	*			based on the number of tokens.
	*	#4.	use function strtok_r to find out the tokens 
    *   #5. malloc each index of the array with the length of each token,
	*			fill command_list array with tokens, and fill last spot with NULL.
	*	#6. return the variable.
	*/

	// allocates mem for the command_line variable
	command_line cl;

	if (buf == NULL)
		return cl;

	// kill new line character
	int c = 0;
	while(buf[c] != '\0'){
		if (buf[c] == '\n')
			buf[c] = '\0';
		c++;
	}

	// copies buffer into copy_buffer
	size_t buf_length = strlen(buf);
	char *copy_buffer = (char *)malloc(sizeof(char) * (buf_length + 1));
	strcpy(copy_buffer, buf);
	copy_buffer[buf_length] = '\0';

	// copies buffer into second copy_buffer
	char *copy_buffer_2 = (char *)malloc(sizeof(char) * (buf_length + 1));
	strcpy(copy_buffer_2, buf);
	copy_buffer_2[buf_length] = '\0';

	// sets the num_token variable with the number of tokens in the buffer using count_token function
	cl.num_token = count_token(copy_buffer, delim);


	// Allocating memory for each token, +1 for null token
	cl.command_list = (char **)malloc((sizeof(char *)) * (cl.num_token + 1));

	// variables to run strok_r 
	char *cl_ptr; char *cl_token;

	// run it the first time, get token
	if (cl.num_token > 0)
		cl_token = strtok_r(copy_buffer_2, delim, &cl_ptr);
	
	
	int index = 0;

	// run through rest of tokens until NULL
	while (index < cl.num_token){
		// get length of token
		size_t length = strlen(cl_token);

		for (int i = 0; i < length; ++i)
		{
			if (cl_token[i] == '\n'){		
				cl_token[i] = '\0';}
		}

		// allocate mem based on length of token, +1 for null character
		cl.command_list[index] = (char *)malloc(sizeof(char) * (length + 1));

		// copy the token into the command list
		if (cl_token != NULL && cl.command_list[index] != NULL){
			// fprintf(stderr, "token: [%s]\n", cl_token);
			strcpy(cl.command_list[index], cl_token);
			cl.command_list[index][length] = '\0';
		}

		// calling strtok to run  through the tokens
		cl_token = strtok_r(NULL, delim, &cl_ptr);

		index++;

	}

	cl.command_list[cl.num_token] = NULL;

	free(copy_buffer);
	free(copy_buffer_2);


	return cl;

	

}


void free_command_line(command_line* command)
{
	//TODO：
	/*
	*	#1.	free the array base num_token
	*/
	
	for (int i = 0; i < command->num_token; i++){
		free(command->command_list[i]);
	}
	free(command->command_list);	
}
