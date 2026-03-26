#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_LEN 1024

// Trim leading/trailing whitespace
char* trim(char* str) {
    char* end;
    while(*str == ' ' || *str == '\t') str++;
    if(*str == 0) return str;
    end = str + strlen(str) - 1;
    while(end > str && (*end == ' ' || *end == '\t' || *end == '\n')) end--;
    *(end+1) = 0;
    return str;
}

// Check if str starts with prefix
int starts_with(const char* str, const char* prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

// Extract header name from include "header.c+++h"
void extract_header(const char* line, char* out) {
    const char* start = strchr(line, '"');
    if(!start) { out[0] = 0; return; }
    start++;
    const char* end = strchr(start, '"');
    if(!end) { out[0] = 0; return; }
    size_t len = end - start;
    strncpy(out, start, len);
    out[len] = 0;
}

int preprocess(const char* input_file, const char* output_file) {
    FILE* fin = fopen(input_file, "r");
    if(!fin) {
        fprintf(stderr, "Error: Cannot open input file %s\n", input_file);
        return 1;
    }

    FILE* fout = fopen(output_file, "w");
    if(!fout) {
        fprintf(stderr, "Error: Cannot create output file %s\n", output_file);
        fclose(fin);
        return 1;
    }

    char line[LINE_LEN];
    while(fgets(line, LINE_LEN, fin)) {
        char* trimmed = trim(line);

        if(starts_with(trimmed, "include ")) {
            char header_name[256];
            extract_header(trimmed, header_name);
            if(strlen(header_name) == 0) {
                fprintf(stderr, "Warning: invalid include directive: %s\n", line);
                continue;
            }

            FILE* header = fopen(header_name, "r");
            if(!header) {
                fprintf(stderr, "Warning: cannot open header %s\n", header_name);
                continue;
            }

            char hline[LINE_LEN];
            while(fgets(hline, LINE_LEN, header)) {
                fputs(hline, fout);
            }
            fclose(header);
        } else {
            fputs(line, fout);
        }
    }

    fclose(fin);
    fclose(fout);

    printf("Preprocessing complete: %s -> %s\n", input_file, output_file);
    return 0;
}

// Command line interface
int main(int argc, char** argv) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <input.c+++ file> <output.i+++ file>\n", argv[0]);
        return 1;
    }

    return preprocess(argv[1], argv[2]);
}
