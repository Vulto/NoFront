#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <dirent.h>
#include <linux/joystick.h>
#include <ncurses.h>

typedef struct {
    const char *dir;
    const char *real_name;
    const char *pretty_name;
    const char *command;
    const char *arg1;
    const char *arg2;
} Game;

#include "../systems/nes/nes.h"
#include "../systems/snes/snes.h"
#include "../systems/n64/n64.h"
