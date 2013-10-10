/********************************************//**
 * Students: Eran Raskansky, Michael Raskansky
 * Maman:    14
 * Course:   20465
 * Semester: 2013b
 * File:     main.c
 *
 * This file contains the main function.
 *
 ***********************************************/

#include <stdio.h>
#include <stdlib.h>

#include "include/assembler.h"

/********************************************//**
 *
 * This is the main method which will start the compiler.
 * 1) Check if the file name contains .ps, if not it will exit.
 * 2) Do the first Transition.
 * 3) Do the second transition.
 * 4) Check for errors, if errors exist it will print them and exit the program
 *    before writing the files.
 * 5) Generate the required files.
 *
 ***********************************************/
int main(int argc, char *argv[]) {
	Compiler *compiler = NULL;
	int curFileIdx = 1;

	if (argc == 1) {
		printf("No input files to compile!\n");
		exit(0);
	} else {
		while (--argc) {

			if (is_valid_filename(argv[curFileIdx]) == FALSE)
				exit(0);

			init_compiler(&compiler, BASE_OFFSET, argv[curFileIdx]);
			first_transition(compiler);
			second_transition(compiler);

			if (check_errors(compiler)) {
				//printf("errors\n");
				print_errors(compiler);
				exit(0);
			} else
				generate_files(compiler);

			free(compiler);
			curFileIdx++;
		}
	}

	printf("Compilation finished successfully.\n");
	return 0;
}
