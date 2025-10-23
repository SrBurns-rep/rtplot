#!/usr/bin/env bash
gcc -Og -g -Wall -fsanitize=address,undefined main.c -o rtplot -I . -L . -lraylib -lm -lGL -lpthread -ldl -lrt -DUSE_WAYLAND_DISPLAY=TRUE -DPLATFORM_DESKTOP
