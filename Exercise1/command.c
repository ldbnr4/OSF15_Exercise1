#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "command.h"

#define MAX_CMD_COUNT 50
#define MAX_CMD_LEN 25


 	/* 
	 * PURPOSE: parses the user's input into a list of commands
	 * INPUT: 
	 *	input - the user's inputs
	 *	cmd - the list to be created from the user's entries
	 * RETURN:
	 *  True - if the list of commands are created successfully
	 *  Fasle - if there are errors with allocation
	 */
bool parse_user_input (const char* input, Commands_t** cmd) {
	
	if( !input ){
		printf("Null input from the user.\n");
		return false;
	}//TODO ERROR CHECK INCOMING PARAMETERS

	char *string = strdup(input);
	
	*cmd = calloc (1,sizeof(Commands_t));
	(*cmd)->cmds = calloc(MAX_CMD_COUNT,sizeof(char*));

	unsigned int i = 0;
	char *token;
	token = strtok(string, " \n");
	for (; token != NULL && i < MAX_CMD_COUNT; ++i) {
		(*cmd)->cmds[i] = calloc(MAX_CMD_LEN,sizeof(char));
		if (!(*cmd)->cmds[i]) {
			perror("Allocation Error\n");
			return false;
		}	
		strncpy((*cmd)->cmds[i],token, strlen(token) + 1);
		(*cmd)->num_cmds++;
		token = strtok(NULL, " \n");
	}
	free(string);
	return true;
}

 	/* 
	 * PURPOSE: frees the memory allocated for the commands
	 * INPUTS:
	 * cmd - list of commands
	 * RETURN:
	 *
	 */
void destroy_commands(Commands_t** cmd) {

	if( !(*cmd) ){
		printf("Null list of commands.\n");
		return;
	}//TODO ERROR CHECK INCOMING PARAMETERS
	
	for (int i = 0; i < (*cmd)->num_cmds; ++i) {
		free((*cmd)->cmds[i]);
	}
	free((*cmd)->cmds);
	free((*cmd));
	*cmd = NULL;
}

