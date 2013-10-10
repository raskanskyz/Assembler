/********************************************//**
 * Students: Eran Raskansky, Michael Raskansky
 * Maman:    14
 * Course:   20465
 * Semester: 2013b
 * File:     parser.c
 *
 * This file contains functions which will deal with parsing the various parts of the code.
 *
 ***********************************************/

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "include/assembler.h"

/********************************************//**
 * Name:
 *  parse_line
 *
 * Params:
 *  *comp - A pointer to a compiler.
 *
 * Description:
 *  This function will parse the lines.
 *  it will is the standard libraries strtok() function
 *  to break the line in to tokens for further processing.
 *
 * Return:
 *  1 There was a problem parsing the line.
 *  0 The line was parsed successfully.
 *
 ***********************************************/
int parse_line(Compiler *comp) {

	//Create local copy of line so strtok() doesn't destroy the original.
	char *symbol = NULL;
	char *locLine = (char *) malloc(sizeof(char) * MAX_LINE_LEN);
	strcpy(locLine, comp->line);
	char *token = strtok(locLine, " \r\t\n");

	//Check if the line is empty, if it is stop the processing.
	if (token == NULL)
		return 0;

	//Check if the line is a comment, if it is stop the processing.
	if (strstr(token, ";") != NULL)
		return 0;

	//Check if optional label exists, if it does set symbol parameter, and get next token.
	if ((strstr(token, ":")) != NULL) {
		symbol = token;
		CUT_LAST_CHAR(symbol);
		token = strtok(NULL, " \r\t\n");
	}

	//Check if the line is .entry
	if (strstr(comp->line, ".entry") != NULL) {
		token = strtok(NULL, " \r\t\n");
		add_entry(comp, token);
		if (symbol != NULL)
			add_symbol(comp, symbol, DATA);
		return 0;
	}

	//Check if the line is .extern
	if (strstr(comp->line, ".extern") != NULL) {
		token = strtok(NULL, " \r\t\n");
		add_extern(comp, token);
		if (symbol != NULL)
			add_symbol(comp, symbol, DATA);
		return 0;
	}

	//Check if the line is .data
	if (strstr(comp->line, ".data") != NULL) {
		if (symbol != NULL)
			add_symbol(comp, symbol, DATA);
		token = strtok(NULL, " \r\t\n");
		add_data(comp, atoi(token));
		return 0;
	}

	//Check if the line is .string
	if (strstr(comp->line, ".string") != NULL) {
		if (symbol != NULL)
			add_symbol(comp, symbol, DATA);
		token = strtok(NULL, " \r\t\n");
		if (is_valid_string(token))
			add_string(comp, token);
		else
			add_error(comp, comp->fileName, comp->lineIdx, "Missing \"\n");
		return 0;
	}

	//if we got here the the line is code, if it has an optional symbol add it to symbol table.
	if (symbol != NULL)
		add_symbol(comp, symbol, CODE);

	//Add code to the compiler and check if the addition was successful.
	if (!add_code(comp, token, strtok(NULL, " \r\t\n"), strtok(NULL, ","))) {

		//Update the codeLen and dataLen fields
		comp->codeLen = comp->IC;
		comp->dataLen = comp->DC;
		free(locLine);
		return 0;
	}
	add_error(comp, comp->fileName, comp->lineIdx,
			"Incorrect line definition.");
	return 1;
}

/********************************************//**
 * Name:
 *  parse_command_fields
 *
 * Params:
 *  comp 		- A pointer to a compiler.
 *  cmd 			- The unparsed command line.
 *
 *  oper1		- This is where the function will put the parsed operand1 string
 *  oper2		- This is where the function will put the parsed operand2 string
 *  operation 	- This is where the function will put the parsed operation string
 *  type			- This is where the function will put the parsed type field
 *  comb1		- This is where the function will put the parsed combination field
 *  comb2		- This is where the function will put the parsed combination field
 *  dbl			- This is where the function will put the parsed dbl field
 *  oper1_type	- This is where the function will put the parsed operand1 type
 *  oper2_type	- This is where the function will put the parsed operand2 type
 *
 *
 * Description:
 *  This function will parse the lines.
 *  it will is the standard libraries strtok() function
 *  to break the line in to tokens for further processing.
 *
 * Return:
 *  0
 *
 ***********************************************/
int parse_command_fields(Compiler *comp, char *cmd, char *oper1, char *oper2,
		char *operation, int *type, int *comb1, int *comb2, int *dbl,
		int *oper1_type, int *oper2_type) {

	short combFlag = is_comb(cmd);//Check if the command has combinations included.
	strcpy(operation, strtok(cmd, "/"));//Get the operation it is before the first '/'
	char *type_comb = strtok(NULL, ",");//The type and combination field will be before the next ','
	*dbl = atoi(strtok(NULL, " \r\t\n"));	//The DBL field before the space.
	*type = atoi(strtok(type_comb, "/"));//Separate the type from the combinations

	*comb1 = -1;
	*comb2 = -1;

	//If the command has combinations get them.
	if (combFlag) {
		*comb1 = atoi(strtok(NULL, "/"));
		*comb2 = atoi(strtok(NULL, ","));
	}

	//If the first operand exists, clean up the spaces
	if (oper1 != NULL)
		REMOVE_SPACE(oper1);

	//If the second operand exists, clean up the spaces
	if (oper2 != NULL)
		REMOVE_SPACE(oper2);

	////If the first operation exists, clean up the spaces
	if (operation != NULL)
		REMOVE_SPACE(operation);

	//Get the operand types.
	*oper1_type = get_oper_type(comp, oper1);
	*oper2_type = get_oper_type(comp, oper2);

	return 0;
}

/********************************************//**
 * Name:
 *  get_between_braces
 *
 * Params:
 *  oper 	- The full operand.
 *  between	- The pointer to the location where the parsed string will be put.
 *
 * Description:
 *  This function will parse the lines.
 *  it will is the standard libraries strtok() function
 *  to break the line in to tokens for further processing.
 *  need to check if the string containd a *
 *
 * Return:
 *  1 if the code contains errors.
 *  0 if the code is clean.
 *
 ***********************************************/
char *get_between_braces(char *oper, char *between) {

	char *start = strstr(oper, "{");
	char *end = strstr(oper, "}");
	int j = 0, i = 1;

	if (start == NULL || end == NULL)
		return NULL;

//	if (*(start + i) == '*')
//		i = 2;

	while (*(start + i) != '}')
		*(between + j++) = *(start + i++);

	*(between + j) = '\0';

	return between;
}

int is_comb(char *cmd) {
	if (strstr(cmd, "0/0") != NULL || strstr(cmd, "1/0") != NULL
			|| strstr(cmd, "0/1") != NULL || strstr(cmd, "1/1") != NULL)
		return 1;
	return 0;
}

int is_numeric(const char * s) {

	if (s == NULL || *s == '\0' || isspace(*s))
		return 0;
	char * p;
	strtod(s, &p);
	return *p == '\0';
}
