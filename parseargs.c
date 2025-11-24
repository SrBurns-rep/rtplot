#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>

char *strncat_s(char *restrict dst, const char *src, size_t size) {
    size_t len = size - strlen(dst) - 1;
    return strncat(dst, src, len);
}

int print_err(const char * fmt, ...) {

    const int fmt_len = 256;
    char fmt_ap[fmt_len];

    strncpy(fmt_ap, "\x1b[0;1;31m[ERROR] ", fmt_len);
    strncat_s(fmt_ap, fmt, fmt_len);
    strncat_s(fmt_ap, "\x1b[0m", fmt_len);

    va_list ap;
    va_start(ap, fmt);
    va_end(ap);

    return vfprintf(stderr, fmt_ap, ap);
}

int file_is_not_char(char *path) {

    struct stat sb = {0};

    if(lstat(path, &sb) == -1) {
        print_err("Couldn't get file status: %s\n", path);
        return 1;
    }

    if ((sb.st_mode & S_IFMT) == S_IFCHR) return 0;

    return 1;
}

typedef struct {
    char *path;
    int baudrate;
    int framerate;
    int buffer_lenght;
    int error;
}RTargs;

RTargs parse_args(int argc, char **argv) {

    RTargs args = {0};

    if(argc < 5) {
        print_err("Not enough argumens, baudrate and device path are required.\n");
        args.error = 1;
        return args;
    }

    for(int i = 1; i < argc - 1; i++) {

        if(strcmp(argv[i], "-b") == 0) {
            args.baudrate = atoi(argv[i + 1]);
            if(args.baudrate <= 0) {
                print_err("Invalid baudrate: %s\n", argv[i + 1]);
                args.error = 1;
                return args;
            }
        }

        if(strcmp(argv[i], "-d") == 0) {
            // check if path exists
            args.path = argv[i + 1];

            if(access(args.path, F_OK)) {
                print_err("The path doesn't exist: %s\n", args.path);
                args.error = 1;
                return args;
            }

            if(access(args.path, R_OK)) {
                print_err("Permission denied to read: %s\n", args.path);
                args.error = 1;
                return args;
            }

            if(file_is_not_char(args.path)) {
                print_err("File is not a character device: %s\n", args.path);
                args.error = 1;
                return args;
            }
        }

        if(strcmp(argv[i], "-f") == 0) {
            args.framerate = atoi(argv[i + 1]);
            if(args.framerate <= 0) {
                print_err("Invalid framerate: %s\n", argv[i + 1]);
                args.error = 1;
                return args;
            }
        }

        if(strcmp(argv[i], "-l") == 0) {
            args.buffer_lenght = atoi(argv[i + 1]);
            if(args.buffer_lenght <= 0) {
                print_err("Invalid buffer lenght: %s\n", argv[i + 1]);
                args.error = 1;
                return args;
            }
        }
    }   

   return args;
}

int main(int argc, char **argv) {

    // format
    // -b baudrate          number      required
    // -d device            path        required
    // -f framerate         number      optional
    // -l buffer lenght     number      optional

    //rtplot -b 115200 -d /dev/ttyACM0 
    //  0     1  2      3   4

    if(argc < 5) {
        print_err("Not enough argumens, baudrate and device path are required.\n");
        return -1;
    }

    char stty_command_buffer[1024] = {0};

    RTargs args = parse_args(argc, argv);

    if(args.error) {
        print_err("Couldn't start rtplot.\n");
        return -1;
    }

    return 0;
}
