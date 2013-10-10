/********************************************//**
 * Students: Eran Raskansky, Michael Raskansky
 * Maman:    14
 * Course:   20465
 * Semester: 2013b
 * File:     file_generator.c
 *
 * This file contains all functions which are associated
 * with generating the output files.
 *
 ***********************************************/
#include <stdio.h>
#include <string.h>

#include "include/assembler.h"

/********************************************//**
 * Name:
 *  generate_files
 *
 * Params:
 *  *comp - A pointer to a compiler.
 *
 * Description:
 *  This function will call the functions that
 *  actually create the files.
 *
 *
 * Return:
 *  0
 *
 ***********************************************/
int generate_files(Compiler *comp) {

	generate_ent_file(comp); //Create entry file
	generate_ext_file(comp); //Create external file
	generate_obj_file(comp); //Create object file

	return 0;
}

/********************************************//**
 * Name:
 *  generate_ent_file
 *
 * Params:
 *  *comp - A pointer to a compiler.
 *
 * Description:
 *  Create the entry file.
 *  Go over the entryTable[] of the compiler
 *  and print the results to the *.ent table
 *  checking if the index is greater then 0.
 *
 * Return:
 *  0
 *
 ***********************************************/
int generate_ent_file(Compiler *comp) {
	char filename[MAX_FILE_NAME];
	strcpy(filename, comp->fileName);
	REPLACE_EXTENSION(filename, .ent);

	//Check if there are entries to write
	if (comp->entryIndex > 0) {
		FILE *pFile;
		//Open the .ent file
		if ((pFile = fopen(filename, "w")) != NULL) {
			//start the iteration of the entry table
			for (int i = 0; i < comp->entryIndex; i++)
				//write to the file
				fprintf(pFile, "%s\t%o\n", comp->entryTable[i].name,
						comp->entryTable[i].address);
		} else
			//If we failed to open add an error.
			add_error(comp, filename, 0, "Cannot open file.");

		fclose(pFile); //Done close the file
	}
	return 0;
}

/********************************************//**
 * Name:
 *  generate_ext_file
 *
 * Params:
 *  *comp - A pointer to a compiler.
 *
 * Description:
 *  Create the extern file.
 *  Go over the entryTable[] of the compiler
 *  and print the results to the *.ext table
 *  it will check if there is anything to print by
 *  checking if the index is greater then 0
 *
 * Return:
 *  0
 *
 ***********************************************/
int generate_ext_file(Compiler *comp) {
	char filename[MAX_FILE_NAME];
	strcpy(filename, comp->fileName);
	REPLACE_EXTENSION(filename, .ext);

	//Check if there are externals to write
	if (comp->externAddressIndex > 0) {
		FILE *pFile;
		//Open file and check if we succeed
		if ((pFile = fopen(filename, "w")) != NULL) {
			// Start iterating over the extern table
			for (int i = 0; i < comp->externAddressIndex; i++)
				//write to the file
				fprintf(pFile, "%s\t%o\n", comp->externAddressTable[i].name,
						comp->externAddressTable[i].address);
		} else
			add_error(comp, filename, 0, "Cannot open file.");
		fclose(pFile);
	}
	return 0;
}

/********************************************//**
 * Name:
 *  generate_obj_file
 *
 * Params:
 *  *comp - A pointer to a compiler.
 *
 * Description:
 *  Create the object file.
 *  Go over the codeTable[] of the compiler
 *  and print the results to the *.ob file
 *  Go over the dataTable[] of the compiler
 *  and print the results to the *.ob file
 *  it will check if there is anything to print by
 *  checking if the index is greater then 0
 *
 * Return:
 *  0
 *
 ***********************************************/
int generate_obj_file(Compiler *comp) {
	char filename[MAX_FILE_NAME];
	strcpy(filename, comp->fileName);
	REPLACE_EXTENSION(filename, .ob);

	//Check if code exists
	if (comp->codeLen > 0) {
		FILE *pFile;
		//Open and check file
		if ((pFile = fopen(filename, "w")) != NULL) {
			//Print the code length and data length
			fprintf(pFile, "\t  %o %o\n", comp->codeLen, comp->dataLen);

			//Iterate over the code table and print to file
			for (int i = 0; i < comp->codeLen; i++)
				fprintf(pFile, "%o\t %07o\t %c \n", comp->stack[i].address,
						comp->stack[i].instruction.Data,
						comp->stack[i].linkInfo);

			//Iterate over the data table and print to file
			for (int i = 0; i < comp->dataLen; i++)
				fprintf(pFile, "%o\t %07o\n", comp->heap[i].address,
						comp->heap[i].instruction.Data);

		} else
			add_error(comp, filename, 0, "Cannot open file.");
		fclose(pFile);
	}
	return 0;
}
