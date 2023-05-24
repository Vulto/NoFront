#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/joystick.h>
#include <dirent.h>
#include <ncurses.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_EVENT_ID 25
#define BUFFER_SIZE 128

#define MAX_ROMS 100

static const char *RomsDir[] = {
"/home/vulto/gm/menu/roms/nes/", 
"/home/vulto/gm/menu/roms/snes/" 
};

static const char *Console[] = {
"Nintendo",
"Super Nintendo",
"Nintendo 64"
};
// To keep track of wich console is on the screen 
unsigned short int idx = 1;

char command[512];

int is_valid_rom_name(const char* name) {
	// Check if the filename ends with .smc
	int len = strlen(name);
	if (strcmp(name + len - 4, ".smc") == 0 ||
		strcmp(name + len - 4, ".sfc") == 0 ||
		strcmp(name + len - 4, ".fig") == 0 ||
		strcmp(name + len - 4, ".swc") == 0 ||
		strcmp(name + len - 4, ".nes") == 0) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

void remove_strange_characters(char* name) {
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
        fprintf(stderr, "Failed to open directory %s\n", RomsDir[idx]);
        return 0;
    }

    // Store stripped and unstripped names of all valid ROMs in arrays
    struct dirent *ent;
    int num_roms = 0;
    while ((ent = readdir(dir)) != NULL && num_roms < MAX_ROMS) {
        if (is_valid_rom_name(ent->d_name)) {
            // Remove strange characters from filename
            char stripped_name[256];
            strncpy(stripped_name, ent->d_name, 255);
            remove_strange_characters(stripped_name);

            // Copy stripped and unstripped names to game_names array
            strncpy(game_names[num_roms], stripped_name, 255);
            strncpy(game_names[num_roms] + strlen(stripped_name) + 1, ent->d_name, 255 - strlen(stripped_name) - 1);
            num_roms++;
        }
    }
    closedir(dir);
    return num_roms;
}