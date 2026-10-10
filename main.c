#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdarg.h>

int is_interactive = 0;

void print_error(const char* format, ...) {
    if (is_interactive) {
        printf("\033[1;31m× error:\033[0m ");
    } else {
        printf("\033[1;31m×\033[0m pack: \033[1;31merror:\033[0m ");
    }
    
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void print_warning(const char* format, ...) {
    if (is_interactive) {
        printf("\033[1;33m⚠ warn:\033[0m ");
    } else {
        printf("\033[1;33m⚠\033[0m pack: \033[1;33mwarn:\033[0m ");
    }
    
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void print_success(const char* format, ...) {
    if (is_interactive) {
        printf("\033[1;32m✓ success:\033[0m ");
    } else {
        printf("\033[1;32m✓\033[0m pack: \033[1;32msuccess:\033[0m ");
    }
    
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void get_dirname(const char* path, char* dirname) {
    char temp[1024];
    strncpy(temp, path, sizeof(temp)-1);
    temp[sizeof(temp)-1] = '\0';

    int len = strlen(temp);
    while (len > 1 && temp[len-1] == '/') {
        temp[len-1] = '\0';
        len--;
    }

    char *last_slash = strrchr(temp, '/');
    if (last_slash != NULL) {
        strcpy(dirname, last_slash + 1);
    } else {
        strcpy(dirname, temp);
    }
    
    if (strlen(dirname) == 0) {
        strcpy(dirname, "archive");
    }
}

void pack_dir(const char* dirpath) {
    char dirname[256];
    get_dirname(dirpath, dirname);

    char pack_filename[512];
    snprintf(pack_filename, sizeof(pack_filename), "%s.pack", dirname);

    FILE* pack_file = fopen(pack_filename, "wb");
    if (!pack_file) {
        print_error("failed to create PACK file\nBuilding terminated.\n");
        return;
    }

    fprintf(pack_file, "PACK 1.0\n");

    DIR* dir = opendir(dirpath);
    if (!dir) {
        print_error("failed to open directory\nBuilding terminated.\n");
        fclose(pack_file);
        return;
    }

    struct dirent* ent;
    char filepath[1024];
    struct stat st;

    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;

        snprintf(filepath, sizeof(filepath), "%s/%s", dirpath, ent->d_name);
        
        if (stat(filepath, &st) == 0) {
            if (S_ISREG(st.st_mode)) {
                fprintf(pack_file, "FILE %s SIZE %ld:\n", ent->d_name, (long)st.st_size);

                FILE* in_file = fopen(filepath, "rb");
                if (in_file) {
                    char buffer[4096];
                    size_t bytes;
                    while ((bytes = fread(buffer, 1, sizeof(buffer), in_file)) > 0) {
                        fwrite(buffer, 1, bytes, pack_file);
                    }
                    fclose(in_file);
                } else {
                    print_warning("failed to read %s\n", filepath);
                }
            }
        }
    }

    closedir(dir);
    fclose(pack_file);
    print_success("directory successfully packed to %s\n", pack_filename);
}

void unpack_file(const char* pack_filepath) {
    FILE* pack_file = fopen(pack_filepath, "rb");
    if (!pack_file) {
        print_error("failed to open PACK file\nUnpacking terminated.\n");
        return;
    }

    char header[16];
    if (!fgets(header, sizeof(header), pack_file) || strcmp(header, "PACK 1.0\n") != 0) {
        print_error("invalid format or version of PACK file\nUnpacking terminated.\n");
        fclose(pack_file);
        return;
    }

    char line[512];
    while (fgets(line, sizeof(line), pack_file)) {
        if (strncmp(line, "FILE ", 5) == 0) {
            char* size_pos = strstr(line, " SIZE ");
            if (!size_pos) {
                print_error("parse error at line: %s\nUnpacking terminated.\n", line);
                fclose(pack_file);
                return;
            }

            char filename[256] = {0};
            int name_len = size_pos - (line + 5);
            
            if (name_len > 0 && name_len < 256) {
                strncpy(filename, line + 5, name_len);
                filename[name_len] = '\0';
            } else {
                print_error("invalid filename length in archive\nUnpacking terminated.\n");
                fclose(pack_file);
                return;
            }

            long size = 0;
            if (sscanf(size_pos, " SIZE %ld:\n", &size) != 1) {
                print_error("failed to parse file size\nUnpacking terminated.\n");
                fclose(pack_file);
                return;
            }

            FILE* out_file = fopen(filename, "wb");
            if (!out_file) {
                print_error("failed to create file for unpacking: %s\n", filename);
                fseek(pack_file, size, SEEK_CUR);
                continue;
            }

            long remaining = size;
            char buffer[4096];
            while (remaining > 0) {
                size_t to_read = (size_t)remaining < sizeof(buffer) ? (size_t)remaining : sizeof(buffer);
                size_t bytes = fread(buffer, 1, to_read, pack_file);
                if (bytes == 0) break; 
                
                fwrite(buffer, 1, bytes, out_file);
                remaining -= bytes;
            }
            fclose(out_file);
            printf(" \033[1;32m✓\033[0m extracted file: %s (%ld bytes)\n", filename, size);
        }
    }

    fclose(pack_file);
    print_success("unpacking completed.\n");
}

void trim_spaces(char *str) {
    char *start = str;
    while (*start == ' ' || *start == '\t') start++;
    
    int len = strlen(start);
    while (len > 0 && (start[len-1] == ' ' || start[len-1] == '\t')) {
        start[len-1] = '\0';
        len--;
    }
    memmove(str, start, len + 1);
}

void run_interactive() {
    char input[1024];
    printf("Type 'help' for available commands, 'exit' to exit.\n");
    
    while (1) {
        printf("pack> ");
        if (!fgets(input, sizeof(input), stdin)) {
            printf("\n");
            break;
        }
        
        input[strcspn(input, "\n")] = '\0';
        if (strlen(input) == 0) continue;
        
        char *cmd = strtok(input, " \t");
        if (!cmd) continue;
        
        if (strcmp(cmd, "exit") == 0) {
            break;
        } else if (strcmp(cmd, "help") == 0) {
            printf("Commands:\n");
            printf("  build <dir_path>\n");
            printf("  unpack <pack_file_path>\n");
            printf("  exit\n");
        } else if (strcmp(cmd, "build") == 0) {
            char *arg = strtok(NULL, ""); 
            if (arg) {
                trim_spaces(arg);
                if (strlen(arg) > 0) pack_dir(arg);
                else print_error("missing path directory\nBuilding terminated.\n");
            } else {
                print_error("missing path directory\nBuilding terminated.\n");
            }
        } else if (strcmp(cmd, "unpack") == 0) {
            char *arg = strtok(NULL, "");
            if (arg) {
                trim_spaces(arg);
                if (strlen(arg) > 0) unpack_file(arg);
                else print_error("missing file path\nUnpacking terminated.\n");
            } else {
                print_error("missing file path\nUnpacking terminated.\n");
            }
        } else {
            print_error("%s: command not found...\n", cmd);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        is_interactive = 1;
        run_interactive();
        return 0;
    }

    if (argc < 3) {
        if (argc == 2) {
            if (strcmp(argv[1], "build") == 0) {
                print_error("missing path directory\nBuilding terminated.\n");
            } else if (strcmp(argv[1], "unpack") == 0) {
                print_error("missing file path\nUnpacking terminated.\n");
            } else if (strcmp(argv[1], "help") == 0) {
                printf("Usage:\n");
                printf("  %s build <dir_path>\n", argv[0]);
                printf("  %s unpack <pack_file_path>\n", argv[0]);
            } else {
                print_error("%s: command not found...\n", argv[1]);
            }
        } else {
            printf("Usage:\n");
            printf("  %s build <dir_path>\n", argv[0]);
            printf("  %s unpack <pack_file_path>\n", argv[0]);
        }
        return 1;
    }

    if (strcmp(argv[1], "build") == 0) {
        pack_dir(argv[2]);
    } else if (strcmp(argv[1], "unpack") == 0) {
        unpack_file(argv[2]);
    } else {
        print_error("%s: command not found...\n", argv[1]);
        return 1;
    }

    return 0;
}