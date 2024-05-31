#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fields.h"
#include "dllist.h"

// Liste uzunluğunu hesaplayan fonksiyon
int get_list_size(Dllist list) {
    Dllist tmp;
    int size = 0;
    dll_traverse(tmp, list) {
        size++;
    }
    return size;
}

void execute_commands(const char *input_file, const char *output_file) {
    IS is;
    Dllist list = new_dllist();
    Dllist tmp, node;
    char *buffer;
    int buffer_size = 1024;
    int cursor = 0;

    // Giriş dosyasını ve çıkış dosyasını kontrol et
    if (strcmp(input_file, "giris.dat") != 0 || strcmp(output_file, "cikis.dat") != 0) {
        printf("Error: Invalid input or output file names. Please use 'giris.dat' for input and 'cikis.dat' for output.\n");
        return;
    }

    // Giriş dosyasını aç
    is = new_inputstruct(input_file);
    if (is == NULL) {
        fprintf(stderr, "Error opening input file: %s\n", input_file);
        return;
    }

    buffer = (char *)malloc(buffer_size * sizeof(char));
    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation error.\n");
        jettison_inputstruct(is);
        return;
    }
    buffer[0] = '\0';

    while (get_line(is) >= 0) {
        if (strcmp(is->fields[0], "yaz:") == 0) {
            for (int i = 1; i < is->NF; i += 2) {
                int count = atoi(is->fields[i]);
                char c;
                if (strcmp(is->fields[i+1], "\\b") == 0) {
                    c = ' ';
                } else if (strcmp(is->fields[i+1], "\\n") == 0) {
                    c = '\n';
                } else {
                    c = is->fields[i+1][0];
                }
                for (int j = 0; j < count; j++) {
                    Dllist node = dll_last(list);
                    for (int k = 0; k < cursor; k++) {
                        node = dll_prev(node);
                    }
                    dll_insert_b(node, new_jval_c(c));
                    cursor++;
                }
            }
        } else if (strcmp(is->fields[0], "sil:") == 0) {
            int count = atoi(is->fields[1]);
            char c = (strcmp(is->fields[2], "\\b") == 0) ? ' ' : is->fields[2][0];
            node = dll_last(list);
            for (int i = 0; i < get_list_size(list) - cursor; i++) {
                node = dll_prev(node);
            }
            while (count > 0 && node != list) {
                if (jval_c(node->val) == c) {
                    Dllist tmp = dll_prev(node);
                    dll_delete_node(node);
                    node = tmp;
                    cursor--;
                    count--;
                } else {
                    node = dll_prev(node);
                }
            }
        } else if (strcmp(is->fields[0], "sonagit:") == 0) {
            cursor = get_list_size(list);
        } else if (strcmp(is->fields[0], "dur:") == 0) {
            break;
        }
    }

    FILE *out = fopen(output_file, "w");
    if (out == NULL) {
        fprintf(stderr, "Error opening output file: %s\n", output_file);
        free(buffer);
        jettison_inputstruct(is);
        free_dllist(list);
        return;
    }

    dll_traverse(tmp, list) {
        fputc(jval_c(tmp->val), out);
    }
    fclose(out);

    free(buffer);
    jettison_inputstruct(is);
    free_dllist(list);
}

int main(int argc, char *argv[]) {
    const char *input_file;
    const char *output_file;

    // Check if input and output files are provided
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    input_file = argv[1];
    output_file = argv[2];

    execute_commands(input_file, output_file);
    return 0;
}
