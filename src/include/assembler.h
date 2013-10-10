/********************************************//**
 * Students: Eran Raskansky, Michael Raskansky
 * Maman:    14
 * Course:   20465
 * Semester: 2013b
 * File:     assembler.h
 *
 * This file contains all the data structures, macros
 * and prototypes of the program.
 *
 ***********************************************/
#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

//Magic numbers
#define NUM_OF_INSTRUCT (16)
#define NUM_OF_REGISTER (8)
#define NUM_OF_ACC_TYPE (5)

#define STACK_SIZE      (2000)
#define HEAP_SIZE       (2000)
#define BASE_OFFSET     (100)

#define MAX_ERR_DESC    (100)
#define MAX_FILE_NAME   (100)
#define MAX_LABEL_NAME  (100)
#define MAX_LINE_LEN    (1000)

#define MAX_ERRORS      (100)
#define MAX_SYMBOLS     (100)
#define MAX_ENTRIES     (100)
#define MAX_EXTERNS     (100)
#define DEBUG_FLAG 		(1)

//Macro Definitions
#define INT_TO_STR(STR, INT) do {sprintf((STR), "%d", (INT));} while (0)

#define REPLACE_EXTENSION(FILE, EXT) do { \
		for(int i = 0; *((FILE)+i) != '\0' ; i++) {\
			if (*((FILE)+i) == '.') { \
				*((FILE)+i) = '\0'; \
				break; \
			} \
		} \
		strcat((FILE), #EXT); \
} while(0)

#define DEBUG(fmt, ...) do {  \
		if (DEBUG_FLAG) { \
			fprintf(stderr, "\033[1m\033[31m * * * DEBUG: \033[0m"); \
			fprintf(stderr, fmt, __VA_ARGS__); \
			fprintf(stderr, "\n"); \
		} \
} \
while (0)
#define CUT_LAST_CHAR(STR) do {*((STR) + strlen(STR) - 1) = '\0';} while (0)

#define REMOVE_SPACE(str) do { \
		char *p1 = (str), *p2 = (str); \
		do {\
			while ((*p2) == ' ' || (*p2) == '\r' || (*p2) == '\t'|| (*p2) == '\n') \
			p2++; \
		} while ((*p1++ = *p2++)); \
} while (0)

//Data structures
enum AddressingModes {
	INSTANT, DIRECT, VARINDEX, DIRECT_REGISTER, NONE
};
enum SymbolType {
	DATA, CODE
};
enum Transition {
	FIRST, SECOND
};

enum BOOLEAN {
	FALSE, TRUE
};

//Instruction data structures
typedef union DataCodeInstruction {
	struct Code {
		unsigned int CombRight :1;
		unsigned int CombLeft :1;
		unsigned int DestReg :3;
		unsigned int DestType :2;
		unsigned int SrcReg :3;
		unsigned int SrcType :2;
		unsigned int OpCode :4;
		unsigned int Type :1;
		unsigned int Dbl :1;
		unsigned int Unused :2;
	} Code;
	unsigned int Data :20;
} DataCodeInstruction;

typedef struct InstructionsInfo {
	int code;
	char *name;
	short number_of_opernads;
	short valid_src[4];
	short valid_dst[4];
} InstructionInfo;

typedef struct Instruction {
	unsigned short address;
	DataCodeInstruction instruction;
	char linkInfo;
} Instruction;

typedef struct Error {
	int lineNumber;
	char error_desciption[MAX_ERR_DESC];
	char file_name[MAX_FILE_NAME];
} Error;

//Symbol data structures.
typedef struct Symbol {
	char name[MAX_LABEL_NAME];
	short address;
	short type;
} Symbol;

//Entry data structure.
typedef struct Entry {
	char name[MAX_LABEL_NAME];
	short address;
} Entry;

//External data structure.
typedef struct Extern {
	char name[MAX_LABEL_NAME];
	short address;
} Extern;

//Compiler data structure
typedef struct Compiler {

	//Base Address
	int offset;

	//Compiler transition state.
	int transition;

	/* Data Index */
	int DC;

	/* Instruction Index */
	int IC;

	//CPU registers
	char *registers[NUM_OF_REGISTER];

	//Errors Flag, 1 if errors exist 0 if no errors exist.
	unsigned int hasErrors :1;

	// Code length
	int codeLen;

	// Data length
	int dataLen;

	// Current handled line index.
	int lineIdx;

	//Symbol Table Index
	int symIndex;

	//Entry table index
	int entryIndex;

	//External symbol table index
	int externIndex;

	//External Address index
	int externAddressIndex;

	//Error table index
	int errorIndex;

	//Number of Instructions
	int instructionCount;

	//Instructions
	InstructionInfo instructions[NUM_OF_INSTRUCT];

	// File name being compiled
	char *fileName;

	//Current line being analyzed.
	char line[MAX_LINE_LEN];

	// Stack array.
	Instruction stack[STACK_SIZE];

	//Heap array.
	Instruction heap[HEAP_SIZE];

	//Error table
	Error errorTable[MAX_ERRORS];

	//Symbol Table
	Symbol symbolTable[MAX_SYMBOLS];

	//Entries table
	Entry entryTable[MAX_ENTRIES];

	//External table
	Extern externTable[MAX_EXTERNS];

	//External address
	Extern externAddressTable[MAX_EXTERNS];

} Compiler;

//Prototype Definitions
char *get_between_braces(char *oper, char *between);
Compiler *init_compiler(Compiler **, int, char*);
InstructionInfo *get_instruction_info(Compiler *comp, char *str);
int add_addresses_to_code(Compiler *, char *, char *);
int add_code(Compiler *, char *, char *, char *);
int add_data(Compiler *, int);
int add_entry(Compiler *, char *);
int add_error(Compiler *comp, char* filename, int line, char *desc);
int add_extern(Compiler *, char *);
int add_string(Compiler *, char *);
int add_symbol_address(Compiler *comp, char *oper);
int add_symbol(Compiler *, char *, int);
int allocate_memory(Compiler *comp, char *oper, int code_address);
int allocate_instant_memory(Compiler *comp, char *num);
int allocate_direct_memory(Compiler *comp, char *sym);
int allocate_varindex_memory(Compiler *comp, char *sym, int code_address);
int build_instruction_code(Compiler *comp, char *operation, int type, int comb1,
		int comb2, int dbl, char *oper1, int oper1_type, char *oper2,
		int oper2_type);
int check_errors(Compiler *comp);
int first_transition(Compiler *);
int generate_ent_file(Compiler *comp);
int generate_ext_file(Compiler *comp);
int generate_files(Compiler *);
int generate_obj_file(Compiler *comp);
int get_external(Compiler *comp, char *sym);
int get_oper_type(Compiler *comp, char *oper);
int get_register(Compiler *comp, char *oper);
int get_symbol(Compiler *comp, char *sym);
int is_comb(char *cmd);
int is_numeric(const char * s);
int is_register(Compiler *comp, char *oper);
int is_valid_access_type(InstructionInfo *info, int src, int dst);
int is_valid_filename(char *file);
int is_valid_format(Compiler *, char *);
int is_valid_instruction(Compiler *comp, char *operation, int type, int comb1,
		int comb2, int dbl, char *oper1, int oper1_type, char *oper2,
		int oper2_type);
int is_valid_string(char *);
int parse_command_fields(Compiler *comp, char *cmd, char *oper1, char *oper2,
		char *operation, int *type, int *comb1, int *comb2, int *dbl,
		int *oper1_type, int *oper2_type);
int parse_line(Compiler *comp);
int put_addresses_in_memory(Compiler *comp, char *oper);
int second_transition(Compiler *);
int update_data_table(Compiler *comp);
int update_entry_table(Compiler *comp);
int update_symbol_table(Compiler *comp);
void print_errors(Compiler *comp);

#endif /* ASSEMBLER_H_ */
