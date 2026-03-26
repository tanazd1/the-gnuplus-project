#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_LEN 1024

// A very minimal parser for C+++
// Currently supports:
//   - #function_name(params) { ... }
//   - r(value);  -> maps to return value
//   - printf(...);
// All other lines are passed through as-is (for demonstration)

typedef struct ASTNode {
    char line[LINE_LEN];
    struct ASTNode* next;
} ASTNode;

// Create a new AST node
ASTNode* new_node(const char* line) {
    ASTNode* node = malloc(sizeof(ASTNode));
    strncpy(node->line, line, LINE_LEN-1);
    node->line[LINE_LEN-1] = 0;
    node->next = NULL;
    return node;
}

// Parse the preprocessed file into a linked list of AST nodes
ASTNode* parse_file(const char* input_file) {
    FILE* fin = fopen(input_file, "r");
    if(!fin) {
        fprintf(stderr, "Error: Cannot open preprocessed file %s\n", input_file);
        return NULL;
    }

    char line[LINE_LEN];
    ASTNode* head = NULL;
    ASTNode* tail = NULL;

    while(fgets(line, LINE_LEN, fin)) {
        ASTNode* node = new_node(line);
        if(!head) head = node;
        else tail->next = node;
        tail = node;
    }

    fclose(fin);
    return head;
}

// Generate output C code (for now, just converts C+++ -> C)
int codegen(ASTNode* ast, const char* output_file) {
    FILE* fout = fopen(output_file, "w");
    if(!fout) {
        fprintf(stderr, "Error: Cannot create output file %s\n", output_file);
        return 1;
    }

    ASTNode* curr = ast;
    while(curr) {
        char* line = curr->line;

        // Convert #function_name(params) { -> normal C
        if(strncmp(line, "#", 1) == 0) {
            // Remove leading #
            fputs(line+1, fout);
        } 
        // Convert r(value); -> return value;
        else if(strncmp(line, "r(", 2) == 0) {
            char* p = line + 2; // after r(
            fprintf(fout, "return %s\n", p);
        } 
        // All other lines remain unchanged
        else {
            fputs(line, fout);
        }

        curr = curr->next;
    }

    fclose(fout);
    return 0;
}

// Free AST memory
void free_ast(ASTNode* ast) {
    while(ast) {
        ASTNode* tmp = ast;
        ast = ast->next;
        free(tmp);
    }
}

// CLI for parser+codegen
int main(int argc, char** argv) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <preprocessed.i+++ file> <output.c file>\n", argv[0]);
        return 1;
    }

    ASTNode* ast = parse_file(argv[1]);
    if(!ast) return 1;

    int res = codegen(ast, argv[2]);

    free_ast(ast);
    return res;
}
