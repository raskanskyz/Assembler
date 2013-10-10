/********************************************//**
 * Students: Eran Raskansky, Michael Raskansky
 * Maman:    14
 * Course:   20465
 * Semester: 2013b
 * File:     transitions.c
 *
 * This file contains the functions which are associated
 * with the first and second transitions.
 *
 ***********************************************/
#include <stdio.h>
#include <stdlib.h>

#include "include/assembler.h"

/********************************************//**
 * Name:
 *  first_transition
 *
 * Params:
 *  *comp - A pointer to a compiler.
 *
 * Description:
 *  This function will open a file and iterate line by line
 *  and send each line to the parse line function for further
 *  processing. When the file has been completely processed
 *  we will update the symbol table and the data table with
 *  the correct addresses. if we cannot open the file for reading
 *  we will print an error and exit.
 *
 *
 * Return:
 *  0
 *
 ***********************************************/
int first_transition(Compiler *comp) {

	int lineNumber = 1; //line number index
	FILE *pFile = fopen(comp->fileName, "r");  //Open the file for reading

	//Check if we successfully opend the file
	if (pFile == NULL) {
		printf("Can not open file \"%s\"!\n", comp->fileName);
		exit(0);
	}

	//Iterate line by line.
	while (fgets(comp->line, MAX_LINE_LEN, pFile) != NULL) {

		//Add line number to the compiler, and increment the index
		comp->lineIdx = lineNumber++;

		//Send to parse_line()
		parse_line(comp);
	}

	//Update tables.
	update_symbol_table(comp);
	update_data_table(comp);
	update_entry_table(comp);

	//Set compiler for second transition.
	comp->transition = SECOND;

	fclose(pFile);
	return 0;
}

/********************************************//**
 * Name:
 *  second_transition
 *
 * Params:
 *  *comp - A pointer to a compiler.
 *
 * Description:
 *  This function will open a file and iterate line by line
 *  and send each line to the parse line function for further
 *  processing. When the file has been completely processed
 *  we will update the entry table with
 *  the correct addresses. if we cannot open the file for reading
 *  we will print an error and exit.
 *
 * Return:
 *  0
 *
 ***********************************************/
int second_transition(Compiler *comp) {
	int lineNumber = 1;
	comp->IC = 0;
	FILE *pFile = fopen(comp->fileName, "r");

	if (pFile == NULL) {
		printf("Can not open file \"%s\"!\n", comp->fileName);
		exit(0);
	}
	while (fgets(comp->line, MAX_LINE_LEN, pFile) != NULL) {

		//Add line number to the compiler.
		comp->lineIdx = lineNumber;

		//Parse the line
		parse_line(comp);

		//Increment line number.
		lineNumber++;
	}
	fclose(pFile);
	return 1;
}


/********************************************//**
 * Name:
 *  update_symbol_table
 *
 * Params:
 *  *comp - A pointer to a compiler.
 *
 * Description:
 *  This function will update the addresses of the DATA symbols.
 *  it will iterate over the symbol table check if the symbol is of type
 *  DATA and set the address to be the next avaliable address after the code.
 *
 * Return:
 *  0
 *
 ***********************************************/
int update_symbol_table(Compiler *comp) {

	for (int i = 0; i < comp->DC; i++) //Iterate over the symbol table
		if (comp->symbolTable[i].type == DATA) //If the symbol is of type DATA
			(comp->symbolTable[i].address) += comp->IC; //Set the address
	return 0;
}

/********************************************//**
 * Name:
 *  update_dat_table
 *
 * Params:
 *  *comp - A pointer to a compiler.
 *
 * Description:
 *  This function will update the addresses of the DATA entries.
 *  The function will iterate over the data table and set the addresses
 *  of each word.
 *
 * Return:
 *  0
 *
 ***********************************************/
int update_data_table(Compiler *comp) {

	for (int i = 0; i < comp->DC; i++) //Start iteration.
		(comp->heap[i].address) = comp->codeLen + comp->offset + i; //Set the address
	return 0;
}


/********************************************//**
 * Name:
 *  update_entry_table
 *
 * Params:
 *  *comp - A pointer to a compiler.
 *
 * Description:
 *  This function will update the addresses of the entries.
 *  it will iterate over the entry table and set the correct addresses.
 *
 * Return:
 *  0
 *
 ***********************************************/
int update_entry_table(Compiler *comp) {

	for (int i = 0; i < comp->entryIndex; i++)
		comp->entryTable[i].address = get_symbol(comp,
				comp->entryTable[i].name);

	return 0;
}

