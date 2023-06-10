#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/joystick.h>
#include <dirent.h>
#include <ncurses.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "config.h"

#define MAX_EVENT_ID 25
#define BUFFER_SIZE 128

unsigned short int highlight = 1; // The cursor
unsigned short int start_index = 0; // This is were menu start to appear
bool running = true;
char game_names[MAX_ROMS][256];

char command[512];

bool gamepad;

int is_valid_rom_name(const char* name) {
	int num_extensions = sizeof(extensions) / sizeof(extensions[0]);
	int len = strlen(name);
	const char* file_extension = name + len - 4;

	for (int i = 0; i < num_extensions; i++) {
		if (strcmp(file_extension, extensions[i]) == 0) {
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

void FormatNames(char* name) {
	// Look for patterns that aren't part of the official name
	char* ptr = name;
	while (*ptr) {
		if (*ptr == '(' || *ptr == '[') {
			*ptr = '\0';
			break;
		}
		ptr++;
	}
}
int get_game_names(char game_names[MAX_ROMS][256], unsigned short int idx) {
    // Open the ROM directory
    DIR *dir = opendir(RomsDir[idx]);
    if (!dir) {
		perror("opendir");
		endwin();
        return EXIT_FAILURE;
    }

    // Store stripped and unstripped names of all valid ROMs in arrays
    struct dirent *ent;
    int num_roms = 0;
    while ((ent = readdir(dir)) != NULL && num_roms < MAX_ROMS) {
        if (is_valid_rom_name(ent->d_name)) {
            // Remove strange characters from filename
            char stripped_name[256];
            strncpy(stripped_name, ent->d_name, 255);
            FormatNames(stripped_name);

            // Copy stripped and unstripped names to game_names array
            strncpy(game_names[num_roms], stripped_name, 255);
            strncpy(game_names[num_roms] + strlen(stripped_name) + 1, ent->d_name, 255 - strlen(stripped_name) - 1);
            num_roms++;
        }
    }
    closedir(dir);
    return num_roms;
}

void run() {
	sprintf(command, "%s \"%s/%s\" >/dev/null 2>&1 &",Emulator[0], RomsDir[idx], game_names[highlight - 1] + strlen(game_names[highlight - 1]) + 1);
	system(command);
}
