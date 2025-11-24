#!/usr/bin/env bash
# Building for wayland
gcc -Og -g -Wall -fsanitize=address,undefined rtplot.c -o rtplot -I . -L . -lraylib -lm -lGL -lpthread -ldl -lrt -DUSE_WAYLAND_DISPLAY=TRUE -DPLATFORM_DESKTOP
