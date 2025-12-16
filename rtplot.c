#include "raylib.h"
#include "ring.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>

#define ARRLEN(X) (sizeof(X)/sizeof((X)[0]))

#define BUFF_LEN_RING 1024
#define BUFF_LEN_SYS 1024
#define BUFF_LEN_DEV 256

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

void print_help(void) {
    printf(
        "rtplot -b <baudrate> -d <device> ...\n"
        "\t-b   baudrate            required\n"
        "\t-d   device              required\n"
        "\t-f   framerate           optional\n"
        "\t-l   buffer lenght       optional\n"
    );
}

void draw_ring_plot_samples(SampleRing *ring, Rectangle rect, Color bg, Color fg) {

    int x0 = 0, x1 = 0, y0 = 0, y1 = 0;

    int xt = 0, yt = 0;

    DrawRectangle(rect.x - 1, rect.y, rect.width, rect.height + 1, bg);

    for(size_t i = 0; i < ring->head; i++) {

        if(i > 0) {
            x0 = ((ring->head - i + 1) * rect.width) / ring->size;
            x1 = ((ring->head - i) * rect.width) / ring->size;
            x0 = rect.width - x0;
            x1 = rect.width - x1;
            x0 += rect.x;
            x1 += rect.x;

            // Invert Y axis
            y0 = 1023 - ring->samples[i - 1].raw;
            y1 = 1023 - ring->samples[i].raw;
            y0 = (y0 * rect.height) / 1024;
            y1 = (y1 * rect.height) / 1024;
            y0 += rect.y;
            y1 += rect.y;

            DrawLine(x0, y0, x1, y1, fg);

            if(i == 1) {
                xt = x0;
                yt = y0;
            }
        }
    }
    
    for(size_t i = ring->head; i < ring->size; i++) {

        if(i > ring->head + 1) {
            x0 = ((ring->size - i + 1 + ring->head) * rect.width) / ring->size;
            x1 = ((ring->size - i + ring->head) * rect.width) / ring->size;
            x0 = rect.width - x0;
            x1 = rect.width - x1;
            x0 += rect.x;
            x1 += rect.x;

            // Invert Y axis
            y0 = 1023 - ring->samples[i - 1].raw;
            y1 = 1023 - ring->samples[i].raw;
            y0 = (y0 * rect.height) / 1024;
            y1 = (y1 * rect.height) / 1024;
            y0 += rect.y;
            y1 += rect.y;
                        
            DrawLine(x0, y0, x1, y1, fg);
        }
    }

    float dist = sqrt((xt - x1) * (xt - x1) + (yt - y1) * (yt - y1));

    if(dist <= rect.height) {
        DrawLine(xt, yt, x1, y1, fg);    
    }
    
}

int main(int argc, char **argv) {

    // format
    // -b baudrate          number      required
    // -d device            path        required
    // -f framerate         number      optional
    // -l buffer lenght     number      optional

    //rtplot -b 115200 -d /dev/ttyACM0 
    //  0     1  2      3   4

    if(argc < 2) {
        print_help();
        print_err("Not enough argumens, baudrate and device path are required.\n");
        return -1;
    }

    if(strcmp(argv[1], "-h") == 0) {
        print_help();
        return 0;
    }

    if(argc < 5) {
        print_help();
        print_err("Not enough argumens, baudrate and device path are required.\n");
        return -1;
    }

    RTargs args = parse_args(argc, argv);

    if(args.framerate == 0) args.framerate = 60;

    if(args.buffer_lenght == 0) args.buffer_lenght = BUFF_LEN_RING;

    if(args.error) {
        print_err("Couldn't start rtplot.\n");
        return -1;
    }

    char buf[BUFF_LEN_SYS] = {0};

    int device_fd = open(args.path, O_RDONLY | O_NONBLOCK);
    if(device_fd < 0) {
        print_err("Couldn't open device for reading: %s\n", args.path);
        return -1;
    }

    snprintf(buf, ARRLEN(buf),
        "stty -F %s %d cs8 -cstopb -parity -icanon min 1 time 1",
        args.path,
        args.baudrate
    );

    int ret = system(buf);
    if(ret < 0) {
        print_err("stty command failed.\n");
        close(device_fd);
        return -1;
    }

    // Init Ring
    SampleRing *ring = ring_init(args.buffer_lenght);

    // Init window
    const int screen_width = 800;
    const int screen_height = 450;

    Color bg_plot_color = (Color) {0x2C, 0x2C, 0x2C, 0xFF};
    Color bg_color =      (Color) {0x33, 0x33, 0x33, 0xFF};

    InitWindow(screen_width, screen_height, "rtplot");

    SetTargetFPS(args.framerate);

    while(!WindowShouldClose()) {

        char read_buf[BUFF_LEN_DEV] = {0};

        int n_read = read(device_fd, read_buf, BUFF_LEN_DEV - 1);

        size_t read_buf_len = strlen(read_buf);
        Sample s = {0};

        if(n_read > 0) {
            // Trim \r\n
            for(size_t i = read_buf_len - 1; i >= 0; --i) {
                if((read_buf[i] >= 0 && read_buf[i] <= 9)) break;

                if(read_buf[i] == '\r') read_buf[i] = '\0';
                if(read_buf[i] == '\n') read_buf[i] = '\0';
            }

            int cur_val = atoi(read_buf);

            if(cur_val >= 0) {
                s.raw = atoi(read_buf);
                s.is_valid = 1;
                ring_push_sample(ring, s);
            }
        }

        Rectangle rect = {
            .width = 600,
            .height = 300,
            .x = (screen_width - 600) / 2,
            .y = (screen_height - 300) / 2
            };

		BeginDrawing();
		{
			ClearBackground(bg_color);
		    draw_ring_plot_samples(ring, rect, bg_plot_color, RAYWHITE);
		}
		EndDrawing();        
    }

    free(ring);
    close(device_fd);
    CloseWindow();
    
    return 0;
}
