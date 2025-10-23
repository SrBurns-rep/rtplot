#!/usr/bin/env bash
gcc -Og -g -Wall main.c -o rtplot -I . -L . -lraylib -lm -lGL -lpthread -ldl -lrt -DUSE_WAYLAND_DISPLAY=TRUE -DPLATFORM_DESKTOP
