/********************************************//**
 * Students: Eran Raskansky, Michael Raskansky
 * Maman:    14
 * Course:   20465
 * Semester: 2013b
 * File:     errors.c
 *
 * This file contains all functions which are associated
 * with error handling.
 *
 ***********************************************/

#include <stdio.h>
#include <string.h>

#include "include/assembler.h"

/********************************************//**
 * Name:
 *  check_errors
 *
 * Params:
 *  *comp - A pointer to a compiler.
 *
 * Description:
 *  This function will check if errors exsist in
 *  the source code.
 *
 * Return:
 *  1 if the code contains errors.
 *  0 if the code is clean.
 *
 ***********************************************/
int check_errors(Compiler *comp) {

	//Check if the Compiler hasErrors field is set.
	if (comp->hasErrors) {
		return TRUE;
	}
	return FALSE;
}

/********************************************//**
 * Name:
 *  add_error
 *
 * Params:
 *  comp        - A pointer to a compiler.
 *  filename    - the file the error was found in.
 *  desc        - a short description of the error.
 *
 * Description:
 *  This function will add an error to the error table.
 *
 * Return:
 *  0 after the error was added.
 *
 ***********************************************/
int add_error(Compiler *comp, char* filename, int line, char *desc) {
	//Set the line number the error was found in.
	comp->errorTable[comp->errorIndex].lineNumber = line;

	//Set the filename.
	strcpy(comp->errorTable[comp->errorIndex].file_name, filename);

	//Set the error description.
	strcpy(comp->errorTable[comp->errorIndex].error_desciption, desc);

	//Increment the error table index.
	comp->errorIndex++;

	//Toggle the compilers error flag.
	(comp->hasErrors) = TRUE;

	return 0;
}

/********************************************//**
 * Name:
 *  is_valid_format
 *
 * Params:
 *  comp    - A pointer to a compiler.
 *  cmd     - the command which include the command/type/comb/dbl
 *
 * Description:
 *  This function will check if the command format is correct.  
 *  If it is not valid add an error.
 *  it does this by matching charachters that must be included
 *
 *  if "/0" or "/1" for the Type, ",0" or ",1" for the DBL.
 *
 *
 * Return:
 *  0 if the format is not valid
 *  1 if the format is valid
 *
 ***********************************************/
int is_valid_format(Compiler *comp, char *cmd) {
	//Check for type and DBL strings.
	if ((strstr(cmd, "/0") == NULL && strstr(cmd, "/1") == NULL)
			|| (strstr(cmd, ",0") == NULL && strstr(cmd, ",1") == NULL)) {
		add_error(comp, comp->fileName, comp->lineIdx,
				"Illegal instruction definition, missing type or DBL.");
		return 0;
	}
	return 1;
}

/********************************************//**
 * Name:
 *  is_valid_string
 *
 * Params:
 *  str    - the string to evaluate.
 *
 * Description:
 *   A valid string data is a string startting with '\"' and ending with a '\"'
 *
 *
 *
 * Return:
 *  0 if the format is not valid
 *  1 if the format is valid
 *
 ***********************************************/
int is_valid_string(char *str) {
	return (*str == '\"' && *(str + strlen(str) - 1) == '\"');
}

/********************************************//**
 * Name:
 *  is_valid_access_type
 *
 * Params:
 *  info     - the command being checked.
 *  src      - Source memory access type.
 *  dst      - Destination memory access type.
 *
 * Description:
 *   This function will check if the source operand
 *   and the destination operand are valid for the
 *   specific command.
 *   It does this by iterating over the valid_src and
 *   valid_dst data fields under InstructionInfo
 *   and comparing then to the src and dst parameters.
 *   it will set a flag if the memory access type is valid
 *   for the src and dst parameters.
 *
 * Return:
 *  0 if the src and dst accesstypes are not valid for the command.
 *  1 if the memory access types are valid for the command.
 *
 ***********************************************/
int is_valid_access_type(InstructionInfo *info, int src, int dst) {

	int valid_src_flag = 0, valid_dst_flag = 0;

	//Check if valid_dst[] contains src.
	for (int i = 0; i < NUM_OF_ACC_TYPE; i++)
		if (info->valid_dst[i] == dst) {
			valid_dst_flag = 1;
			break;
		}

	//Check if valid_src[] contains src.
	for (int i = 0; i < NUM_OF_ACC_TYPE; i++)
		if (info->valid_src[i] == src) {
			valid_src_flag = 1;
			break;
		}

	return (valid_dst_flag && valid_src_flag);
}

/********************************************//**
 * Name:
 *  is_valid_access_instruction
 *
 * Params:
 *  comp        - The compiler pointer.
 *  operation   - The Command field, jsr, stop, lea...
 *  type        - The Type filed, 0 or 1.
 *  comb1       - The Combination filed, only relevent when the type is 1.
 *  comb2       - The Combination filed, only relevent when the type is 1.
 *  dbl         - The DBL field, 0 or 1.
 *  oper1       - The first operand.
 *  oper1_type  - The first operand memory access type.
 *  oper2       - The second operand.
 *  oper2_type  - The second operand memory access type.
 *
 * Description:
 *  This funciont will check the validity of the inscrution.
 *  the test that will take place:
 *      - If the command is known in the language.
 *      - if the number of operands are suited for the command.
 *      - If the type filed is either 0 or 1.
 *      - If the dbl filed is either 0 or 1.
 *      - If combinations exsist for type 0.
 *      - If combinations arent specified for type 1.
 *      - if the memory access types valid for the command.
 *
 * Return:
 *  0 if the instrucion is not valid
 *  1 if the instruction is valid.
 *
 ***********************************************/
int is_valid_instruction(Compiler *comp, char *operation, int type, int comb1,
		int comb2, int dbl, char *oper1, int oper1_type, char *oper2,
		int oper2_type) {

	if (comp->transition == SECOND)
		return TRUE;

	InstructionInfo *instructionInfo;
	int num_of_operands = 0;
	int valid_flag = TRUE;

	if (oper1 != NULL)
		num_of_operands++;

	if (oper2 != NULL)
		num_of_operands++;

	if ((instructionInfo = get_instruction_info(comp, operation)) == NULL) {
		add_error(comp, comp->fileName, comp->lineIdx, "Unrecognized command.");
		valid_flag = FALSE;
	}

	else if (num_of_operands != instructionInfo->number_of_opernads) {
		add_error(comp, comp->fileName, comp->lineIdx,
				"Invalid number of operands.");
		valid_flag = FALSE;
	}

	else if (type != 0 && type != 1) {
		add_error(comp, comp->fileName, comp->lineIdx,
				"Invalid operation type.");
		valid_flag = FALSE;
	}

	else if (dbl != 0 && dbl != 1) {
		add_error(comp, comp->fileName, comp->lineIdx,
				"Invalid operation dbl.");
		valid_flag = FALSE;
	}

	else if (type == 0 && comb1 != -1 && comb2 != -1) {
		add_error(comp, comp->fileName, comp->lineIdx,
				"Type 0 cannot be used with combinations.");
		valid_flag = FALSE;
	}

	else if (type == 1 && comb1 == -1 && comb2 == -1) {
		add_error(comp, comp->fileName, comp->lineIdx,
				"Type 1 used, must specify combinations.");
		valid_flag = FALSE;
	}
	else if (!is_valid_access_type(instructionInfo, oper1_type, oper2_type)) {
		add_error(comp, comp->fileName, comp->lineIdx, "Invalid access type.");
		valid_flag = FALSE;
	}

	return valid_flag;
}

/********************************************//**
 * Name:
 *  print_errors
 *
 * Params:
 *  comp     - The compiler.
 *
 * Description:
 *   This function will print the errors in the error table
 *    of the compiler. It will do the by iterating with a for
 *    loop over the error table and printing each error.
 *
 ***********************************************/
void print_errors(Compiler *comp) {

	for (int i = 0; i < comp->errorIndex; i++)
		printf("%s:%d: error: %s\n", comp->errorTable[i].file_name,
				comp->errorTable[i].lineNumber,
				comp->errorTable[i].error_desciption);
}

/********************************************//**
 * Name:
 *  check_filename
 *
 * Params:
 *  file     - The file to be checked.
 *
 * Description:
 *   This function will check if the file is of type *.as
 *
 * Return:
 *   0 not valid
 *   1 valid file name.
 *
 ***********************************************/
int is_valid_filename(char *file) {
	if (strstr(file, ".as") == NULL) {
		printf("Invalid Filename %s!\n", file);
		return FALSE;
	}
	return TRUE;
}
