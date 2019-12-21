#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <zconf.h>
#include <errno.h>

#define N_DEFAULT 10
#define bool bool

#define ERROR(msg) write(2, msg, strlen(msg))

int EOL = '\n';

typedef unsigned short bool;

int print_error(char* msg, bool with_errno) {
    if (with_errno) {
        char* msg_in = msg;
        char* msg_new = malloc(strlen(msg_in) + 100);
        strcat(msg_new, msg_in);
        strcat(msg_new, ": ");
        strcat(msg_new, strerror(errno));
        strcat(msg_new, "\n");
        ERROR(msg_new);
        free(msg_new);
    } else {
        ERROR(msg);
    }
    return errno;
}

bool head(char* file, int n_items, bool print_headers, bool lines) {
    int fd;
    char buffer[BUFSIZ];
    if (strcmp(file, "-") == 0) {
        fd = STDIN_FILENO;
        file = "stdin";
    } else {
        fd = open(file, O_RDONLY);
        if (fd < 0) {
            print_error("Error opening file", 1);
            return 0;
        }
    }

    while (n_items != 0) {
        ssize_t n_selected_bytes = 0;
        char read_byte = 0;
        ssize_t len = read(fd, buffer, BUFSIZ);
        bool fullbuf = (bool) (len == BUFSIZ ? 1 : 0);
        if (len == -1) {
            print_error("Error reading file", 1);
            return 0;
        }

        int step = 1;

        if (n_items < 0) {
            step = -1;
            n_selected_bytes = len;
            if (lines) {
                n_items--;
            }
        }

        while (n_items != 0 && len != 0) {
            read_byte = buffer[n_selected_bytes];
            len--;
            n_selected_bytes += step;
            if (!lines || read_byte == EOL) {
                n_items -= step;
            }
        }

        if (step == -1 && lines) {
            n_selected_bytes += 1;
        }

        if (print_headers) {
            char header[100] = "---";
            strcat(header, file);
            strcat(header, "---\n");
            write(STDOUT_FILENO, header, strlen(header));
        }

        buffer[n_selected_bytes] = '\n';
        n_selected_bytes++;
        write(STDOUT_FILENO, buffer, (size_t) n_selected_bytes);
        if (fd != STDIN_FILENO)
            close(fd);

        if (!fullbuf && len == 0) {
            n_items = 0;
        }
    }
    return 1;
}

int main(int argc, char** argv) {
    int opt;
    bool success = 1;
    bool print_headers = 1;
    bool lines = 1;
    char* endptr;
    char msg[500];
    char** filelist;
    int nval = N_DEFAULT;
    while ((opt = getopt(argc, argv, "zqn:c:")) != -1) {
        switch (opt) {
            case 'z':
                EOL = '\0';
                break;
            case 'n':
                nval = strtol(optarg, &endptr, 10);
                lines = 1;
                break;
            case 'c':
                nval = strtol(optarg, &endptr, 10);
                lines = 0;
                break;
            case 'q':
                print_headers = 0;
                break;
            case '?':
                if (optopt == 'n') {
                    sprintf(msg, "Option -%c requires an argument.\n", optopt);
                    print_error(msg, 0);
                } else if (isprint(optopt)) {
                    sprintf(msg, "Unknown option `-%c'.\n", optopt);
                    print_error(msg, 0);
                } else {
                    sprintf(msg,
                            "Unknown option character `\\x%x'.\n",
                            optopt);
                    print_error(msg, 0);
                }
                return 1;
            default:
                abort();
        }
    }

    char* stdin_filelist[] = {"-"};
    filelist = optind < argc ? &argv[optind] : stdin_filelist;
    int i = 0;

    while (filelist[i]) {
        success = success & head(filelist[i], nval, print_headers, lines);
        i++;
    }

    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
