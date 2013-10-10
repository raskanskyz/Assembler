/********************************************//**
 * Students: Eran Raskansky, Michael Raskansky
 * Maman:    14
 * Course:   20465
 * Semester: 2013b
 * File:     compiler.c
 *
 * This file contains functions which will deal with , managing the,
 * data structures, add values to the relevant tables memory management
 * etc.
 *
 ***********************************************/

//#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/assembler.h"

/********************************************//**
 * Name:
 *  init_compiler
 *
 * Params:
 *  comp 		- a pointer to a pointer to a compiler structure.
 *  offset 		- the memory address which will be the start of the code
 *  filename 	- the file that is going to be compiled.
 *
 * Description:
 *  This function will.
 *  	1) Allocate memory for the compiler.
 *  	2) Initialize all the data fields.
 *
 * Return:
 * An initialized compiler
 *
 ***********************************************/
Compiler *init_compiler(Compiler **comp, int offset, char *filename) {

	//Allocate memory
	*comp = (Compiler *)malloc(sizeof(Compiler));

	//The compilers registers.
	char *registers[] = {"r0","r1","r2","r3","r4","r5","r6","r7"};

	//The "Language" this include all the valid commands an there valid operands
	InstructionInfo instructions[] = {
			//  Code  Name,	 Operands,				Valid Source, 									Valid Destination
			{0,  "mov",		2,		{INSTANT, DIRECT, VARINDEX, DIRECT_REGISTER}, 	{DIRECT, VARINDEX, DIRECT_REGISTER}},
			{1,  "cmp",		2,		{INSTANT, DIRECT, VARINDEX, DIRECT_REGISTER}, 	{INSTANT, DIRECT, VARINDEX, DIRECT_REGISTER}},
			{2,  "add",		2,		{INSTANT, DIRECT, VARINDEX, DIRECT_REGISTER}, 	{DIRECT, VARINDEX, DIRECT_REGISTER}},
			{3,  "sub",		2,		{INSTANT, DIRECT, VARINDEX, DIRECT_REGISTER}, 	{DIRECT, VARINDEX, DIRECT_REGISTER}},
			{6,  "lea",		2,		{DIRECT, VARINDEX, DIRECT_REGISTER}, 			{DIRECT, VARINDEX, DIRECT_REGISTER}},
			{4,  "not",		1,		{NONE}, 										{DIRECT, VARINDEX, DIRECT_REGISTER}},
			{5,  "clr",		1,		{NONE},											{DIRECT, VARINDEX, DIRECT_REGISTER}},
			{7,  "inc",		1,		{NONE},										 	{DIRECT, VARINDEX, DIRECT_REGISTER}},
			{8,  "dec",		1,		{NONE},			 								{DIRECT, VARINDEX, DIRECT_REGISTER}},
			{9,  "jmp",		1,		{NONE}, 										{DIRECT, VARINDEX, DIRECT_REGISTER}},
			{10, "bne",		1,		{NONE},										 	{DIRECT, VARINDEX, DIRECT_REGISTER}},
			{11, "red",		1,		{NONE}, 										{DIRECT, VARINDEX, DIRECT_REGISTER}},
			{12, "prn",		1,		{NONE}, 					    				{INSTANT, DIRECT, VARINDEX, DIRECT_REGISTER}},
			{13, "jsr",		1,		{NONE}, 										{DIRECT}},
			{14, "rts",		0,		{NONE}, 										{NONE}},
			{15, "stop", 	0,		{NONE}, 										{NONE}}
	};

	//Copy the registers to the compiler
	memcpy((*comp)->registers, registers, sizeof(registers));

	//Copy the valid instructions to the compiler.
	memcpy((*comp)->instructions, instructions, sizeof(instructions));


	//Initialize the data fields.
	(*comp)->codeLen = 0;
	(*comp)->dataLen = 0;
	(*comp)->DC = 0;
	(*comp)->entryIndex = 0;
	(*comp)->externAddressIndex = 0;
	(*comp)->externIndex = 0;
	(*comp)->fileName = filename;
	(*comp)->hasErrors = 0;
	(*comp)->errorIndex = 0;
	(*comp)->IC = 0;
	(*comp)->instructionCount = NUM_OF_INSTRUCT;
	(*comp)->lineIdx = 0;
	(*comp)->offset = offset;
	(*comp)->symIndex = 0;
	(*comp)->transition = FIRST;

	return *comp;
}

/********************************************//**
 * Name:
 *  add_symbol
 *
 * Params:
 *  comp 		- a pointer to a compiler structure.
 *  symbol 		- the symbol to be added
 *  filename 	- the symbol type DATA/CODE.
 *
 * Description:
 *  This function will, add the symbol to the symbol table
 *  and increment the symbol index
 *
 * Return:
 * 0
 *
 ***********************************************/
int add_symbol(Compiler *comp, char *symbol, int type) {

	//Check if this is the first transition.
	if (comp->transition == FIRST) {
		//Copy the symbol to the symbol array
		strcpy(comp->symbolTable[comp->symIndex].name, symbol);

		//Set the symbol address base on the offset plus DC or IC
		if (type == DATA)
			comp->symbolTable[comp->symIndex].address = (comp->offset)
			+ (comp->DC);
		else
			comp->symbolTable[comp->symIndex].address = (comp->offset)
			+ (comp->IC);

		//Set the symbol type.
		comp->symbolTable[comp->symIndex].type = type;

		//Increment the symbol index
		(comp->symIndex)++;
	}
	return 0;
}

/********************************************//**
 * Name:
 *  add_entry
 *
 * Params:
 *  comp 		- a pointer to a compiler structure.
 *  entry 		- the entry to be added
 *
 *
 * Description:
 *  This function will, add an entry to the entry table
 *  and increment the entry index
 *
 * Return:
 * 0
 *
 ***********************************************/
int add_entry(Compiler *comp, char *entry) {

	//Check if this is the first transition
	if (comp->transition == FIRST) {
		//Copy the entry to the entry table
		strcpy(comp->entryTable[comp->entryIndex].name, entry);
		//Increment entry index
		comp->entryIndex++;
	}
	return 0;
}

/********************************************//**
 * Name:
 *  add_extern
 *
 * Params:
 *  comp	- a pointer to a compiler structure.
 *  ext		- the external to be added
 *
 *
 * Description:
 *  This function will, add an external to the external table
 *  and increment the appropriate index.
 *
 * Return:
 * 0
 *
 ***********************************************/
int add_extern(Compiler *comp, char *ext) {

	//Check if this is the first transition.
	if (comp->transition == FIRST) {
		//Copy the extern
		strcpy(comp->externTable[comp->externIndex].name, ext);
		//Increment the index.
		comp->externIndex++;
	}
	return 0;
}

/********************************************//**
 * Name:
 *  add_data
 *
 * Params:
 *  comp	- a pointer to a compiler structure.
 *  data	- the data to be added
 *
 *
 * Description:
 *  This function will, add data to the "heap".
 *  and increment DC.
 *
 * Return:
 * 0
 *
 ***********************************************/
int add_data(Compiler *comp, int data) {
	//Check if this is the first transition.
	if (comp->transition == FIRST) {

		//Set the address to be the offset + the current DC
		comp->heap[comp->DC].address = (comp->offset) + (comp->DC);

		//add the data
		comp->heap[comp->DC].instruction.Data = data;

		//No linker info is required.
		comp->heap[comp->DC].linkInfo = ' ';

		//Increment DC.
		comp->DC++;
	}
	return 0;
}

/********************************************//**
 * Name:
 *  add_string
 *
 * Params:
 *  comp	- a pointer to a compiler structure.
 *  str		- the string to be added
 *
 * Description:
 *  This function will, add a string to the "heap".
 *  and increment DC.
 *
 * Return:
 * 0
 *
 ***********************************************/
int add_string(Compiler *comp, char *str) {
	//Check if this is the first transition.
	if (comp->transition == FIRST) {
		//Iterate over all chars in string, and call add_data for each one.
		for (int i = 1; i < strlen(str) - 1; i++)
			add_data(comp, *(str + i));
		//Increment DC.
		comp->DC++;
	}
	return 0;
}

/********************************************//**
 * Name:
 *  add_symbol_address
 *
 * Params:
 *  comp	- a pointer to a compiler structure.
 *  oper	- the operand.
 *
 * Description:
 *  This function will be used in the second transition.
 *  after the symbol table is complete and all address are know
 *  the function will add the relevant address to the allocated memory
 *  from the first transition.
 *
 * Return:
 * 0
 *
 ***********************************************/
int add_symbol_address(Compiler *comp, char *symbol) {

	//Check if the symbol is external
	if (get_external(comp, symbol) != -1) {

		//Set the linker info to 'e'
		comp->stack[comp->IC].linkInfo = 'e';

		//copy the symbol to the external address table.
		strcpy(comp->externAddressTable[comp->externAddressIndex].name, symbol);

		//Set the address to be the IC.
		comp->externAddressTable[comp->externAddressIndex].address =
				comp->stack[comp->IC].address;

		//Increment the relevant index
		(comp->externAddressIndex)++;

	// Check if the symbol is a local symbol.
	} else if (get_symbol(comp, symbol) != -1) {

		//set the link info to 'r'
		comp->stack[comp->IC].linkInfo = 'r';

		//get the address from the symbol table and put it in the allocated
		//memory slot from the first transition
		comp->stack[comp->IC].instruction.Data = get_symbol(comp, symbol);

	} else
		//If it isn't external or local then it is unknown.
		add_error(comp, comp->fileName, comp->lineIdx, "Unknown symbol.");

	return 0;
}

/********************************************//**
 * Name:
 *  add_code
 *
 * Params:
 *  comp	- a pointer to a compiler structure.
 *  cmd		- the command to be added.
 *  oper1	- the first operand.
 *  oper2	- the second operand.
 *
 * Description:
 *  This function will be used in the second transition.
 *  after the symbol table is complete and all address are know
 *  the function will add the relevant address to the allocated memory
 *  from the first transition.
 *
 * Return:
 * 0
 *
 ***********************************************/
int add_code(Compiler * comp, char *cmd, char *oper1, char *oper2) {

	//Local variables
	int type, dbl, comb1, comb2, oper1_type, oper2_type;
	char operation[10];


	//Basic check if the format is correct, if it is parse all command the fields
	if (is_valid_format(comp, cmd))
		parse_command_fields(comp, cmd, oper1, oper2, operation, &type, &comb1, &comb2, &dbl, &oper1_type, &oper2_type);
	else
		return 0;


	//This should be done only in the first transition
	//We do not need to build the code twice.
	if (comp->transition == FIRST) {

		//Do all error handling in the if.
		if (is_valid_instruction(comp, operation, type, comb1, comb2, dbl, oper1, oper1_type, oper2, oper2_type))

			//Build the instruction code
			build_instruction_code(comp, operation, type, comb1, comb2, dbl, oper1, oper1_type, oper2, oper2_type);
	}
	//Increment the IC, since we finished writing the code.
	(comp->IC)++;

	//Allocate memory for each operand.
	allocate_memory(comp, oper2, comp->IC + comp->offset);
	allocate_memory(comp, oper1, comp->IC + comp->offset);

	return 0;
}

int build_instruction_code(Compiler *comp, char *operation, int type, int comb1,
		int comb2, int dbl, char *oper1, int oper1_type, char *oper2,
		int oper2_type) {

	char oper1_index[MAX_LABEL_NAME], oper2_index[MAX_LABEL_NAME];
	InstructionInfo *info = get_instruction_info(comp, operation);

	if (oper1_type == VARINDEX)
		get_between_braces(oper1, oper1_index);

	if (oper2_type == VARINDEX)
		get_between_braces(oper2, oper2_index);

	comp->stack[comp->IC].address = comp->offset + comp->IC;
	comp->stack[comp->IC].instruction.Code.OpCode = info->code;
	comp->stack[comp->IC].instruction.Code.Type = type;
	comp->stack[comp->IC].instruction.Code.Dbl = dbl;

	if (info->number_of_opernads == 2) {
		comp->stack[comp->IC].instruction.Code.SrcType = oper2_type;
		comp->stack[comp->IC].instruction.Code.DestType = oper1_type;
	} else
		comp->stack[comp->IC].instruction.Code.DestType = oper2_type;

	if (comb1 != -1 && comb2 != -1) {
		comp->stack[comp->IC].instruction.Code.CombLeft = comb1;
		comp->stack[comp->IC].instruction.Code.CombRight = comb2;
	}

	if (is_register(comp, oper1))
		comp->stack[comp->IC].instruction.Code.DestReg = get_register(comp,
				oper1);

	else if (is_register(comp, oper1_index))
		comp->stack[comp->IC].instruction.Code.DestReg = get_register(comp,
				oper1_index);

	if (is_register(comp, oper2))
		comp->stack[comp->IC].instruction.Code.SrcReg = get_register(comp,
				oper2);

	else if (is_register(comp, oper2_index))
		comp->stack[comp->IC].instruction.Code.SrcReg = get_register(comp,
				oper2_index);

	comp->stack[comp->IC].linkInfo = 'a';

	return 0;
}


int allocate_memory(Compiler *comp, char *oper, int code_address) {
	int oper_type = get_oper_type(comp, oper);

	if (oper_type == INSTANT)
		allocate_instant_memory(comp, oper+1);

	else if (oper_type == DIRECT)
		allocate_direct_memory(comp, oper);

	else if (oper_type == VARINDEX)
		allocate_varindex_memory(comp, oper, code_address);

	return 0;
}

int allocate_varindex_memory(Compiler *comp, char *sym, int code_address) {
	char var_index[MAX_LABEL_NAME];
	get_between_braces(sym, var_index);

	if (comp->transition == FIRST)
		comp->stack[comp->IC].address = (comp->IC) + (comp->offset);

	else if (comp->transition == SECOND)
		add_symbol_address(comp, strtok(sym, "{"));

	(comp->IC)++;

	if (is_numeric(var_index))
		allocate_instant_memory(comp, var_index);

	else{ //The index is not numeric. need to check if contains *
		if (comp->transition == SECOND && get_symbol(comp, var_index+1) != -1) {

			INT_TO_STR(var_index, get_symbol(comp, var_index+1)-code_address+1);
			allocate_instant_memory(comp, var_index);
		}
		else
			allocate_memory(comp, var_index, code_address);
	}
	return 0;
}

int allocate_instant_memory(Compiler *comp, char *num) {
	comp->stack[comp->IC].address = (comp->IC) + (comp->offset);
	comp->stack[comp->IC].instruction.Data = atoi(num);
	comp->stack[comp->IC].linkInfo = 'a';
	(comp->IC)++;
	return 0;
}

int allocate_direct_memory(Compiler *comp, char *sym) {

	if (comp->transition == FIRST)
		comp->stack[comp->IC].address = (comp->IC) + (comp->offset);

	else if (comp->transition == SECOND)
		add_symbol_address(comp, sym);

	(comp->IC)++;

	return 0;
}

int get_symbol(Compiler *comp, char *sym) {

	for (int i = 0; i < comp->symIndex; i++) {
		if (strcmp(comp->symbolTable[i].name, sym) == 0) {
			return comp->symbolTable[i].address;
		}
	}
	return -1;
}

int get_external(Compiler *comp, char *sym) {

	for (int i = 0; i < comp->symIndex; i++) {
		if (strcmp(comp->externTable[i].name, sym) == 0) {
			return i;
		}
	}
	return -1;
}

InstructionInfo *get_instruction_info(Compiler *comp, char *str) {
	for (int i = 0; i < comp->instructionCount; i++) {
		if (strcmp(comp->instructions[i].name, str) == 0) {
			return &(comp->instructions[i]);
		}
	}
	return NULL;
}

int get_oper_type(Compiler *comp, char *oper) {

	char buffer[MAX_LABEL_NAME];

	if (oper == NULL)
		return NONE;

	if (strstr(oper, "#") != NULL)
		return INSTANT;

	if (get_between_braces(oper, buffer) != NULL)
		return VARINDEX;

	if (is_register(comp, oper))
		return DIRECT_REGISTER;

	return DIRECT;

}

int get_register(Compiler *comp, char *oper) {

	for (int i = 0; i < NUM_OF_REGISTER; i++)
		if (strcmp(oper, comp->registers[i]) == 0)
			return i;
	return -1;
}

int is_register(Compiler *comp, char *oper) {
	if (oper == NULL)
		return 0;
	if (get_register(comp, oper) != -1)
		return 1;
	return 0;
}
