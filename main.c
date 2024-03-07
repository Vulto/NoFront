#include <ncurses.h>
#include <stdio.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/joystick.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#define Y_AXIS 7
#define X_AXIS 6

#define HEADER 0
#define FOOTER LINES

#define MAX_ROMS 100
#define MAX_DISPLAY_NAME_LENGTH 256

typedef struct {
    const char *Dir;
    const char *Emulator;
} Screen;

Screen nes[] = {
    {"nes", "mednafen"},
};

Screen snes[] = {
    {"snes", "mednafen"},
    {"snes", "mednafen"},
    {"snes", "mednafen"},
};

Screen n64[] = {
    {"n64", "RMG"},
};

Screen *AllSystems[] = {
    nes,
    snes,
    n64,
};

static const char *Menu[] = {
    "Nintendo",
    "Super Nintendo",
    "Nintendo 64"
};

const char *StripPatterns[] = {
    ".smc",
    ".sfc",
    ".z64",
    ".fig",
    ".nes",
    "(U)",
    "snes/",
    "n64/",
    "nes/"
};

int Title = 0;
int FirstTitle = 0;
int LastTitle = (sizeof(Menu) / sizeof(Menu[0])) - 1;
int highlight = 0;
int FirstName = 2;

int RomsCount = 0;

int GetGameNames(const char *romsDir, char gameNames[MAX_ROMS][MAX_DISPLAY_NAME_LENGTH]) {
    DIR *dir = opendir(romsDir);
    if (!dir) {
        perror("opendir");
        endwin();
        return EXIT_FAILURE;
    }

    struct dirent *ent;
    int romsCount = 0;

    while ((ent = readdir(dir)) != NULL && romsCount < MAX_ROMS) {
        // Skip entries for "." and ".."
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }

        // Copy stripped and unstripped names to gameNames array
        snprintf(gameNames[romsCount], MAX_DISPLAY_NAME_LENGTH - strlen(romsDir) - 2, "%s/%s", romsDir, ent->d_name);
        gameNames[romsCount][MAX_DISPLAY_NAME_LENGTH - 1] = '\0';  // Ensure null-termination

        romsCount++;
    }
    closedir(dir);

    // Sort the names in alphabetical order
    qsort(gameNames, romsCount, MAX_DISPLAY_NAME_LENGTH, (int (*)(const void *, const void *))strcmp);

    return romsCount;
}

void StripPatternsFromName(char *name, const char *patterns[]) {
    
    for (int i = 0; patterns[i] != NULL; i++) {
        char *match = strstr(name, patterns[i]);
        while (match != NULL) {
            // Replace the pattern with an empty string
            memmove(match, match + strlen(patterns[i]), strlen(match + strlen(patterns[i])) + 1);
            match = strstr(name, patterns[i]);
        }
    }
}

void DrawText(char gameNames[MAX_ROMS][MAX_DISPLAY_NAME_LENGTH]) {
    int CENTER = (COLS / 2 - strlen(Menu[Title]) / 2);

    clear();
    attron(A_BOLD);
    mvprintw(HEADER, CENTER, "%s", Menu[Title]);
    attroff(A_BOLD);

    Screen *currentSystem = AllSystems[Title];
    RomsCount = GetGameNames(currentSystem->Dir, gameNames);

    // PRINT TITLES
    for (int i = 0; i < RomsCount; i++) {
        char formattedName[MAX_DISPLAY_NAME_LENGTH];
        strcpy(formattedName, gameNames[i]);
        StripPatternsFromName(formattedName, StripPatterns);

        // Calculate center position after stripping characters
        int CENTERNAMES = (COLS / 2 - (strlen(formattedName) / 2));

        if (i == highlight) {
            attron(A_BOLD);
            mvprintw(FirstName + i, CENTERNAMES, "%s", formattedName);
            attroff(A_BOLD);
        } else {
            mvprintw(FirstName + i, CENTERNAMES, "%s", formattedName);
        }
    }

    refresh();
}
void Joystick(struct js_event event, char GameNames[MAX_ROMS][MAX_DISPLAY_NAME_LENGTH]) {
    switch (event.type) {
        case JS_EVENT_AXIS:
            switch (event.number) {
                case X_AXIS:
                    if (event.value < 0) {
                        // Changing system with Left
                        if (Title > FirstTitle) {
                            Title--;
                            highlight = 0;  // Reset highlight when changing system
                        }
                    } else if (event.value > 0) {
                        // Changing system with Right
                        if (Title < LastTitle) {
                            Title++;
                            highlight = 0;  // Reset highlight when changing system
                        }
                    }
                    break;
                case Y_AXIS:
                    if (event.value < 0) {
                        // Move highlight up
                        if (highlight > 0) {
                            highlight--;
                        } else {
                            // Wrap to the bottom if at the top
                            highlight = RomsCount - 1;
                        }
                    } else if (event.value > 0) {
                        // Move highlight down
                        if (highlight < RomsCount - 1) {
                            highlight++;
                        } else {
                            // Wrap to the top if at the bottom
                            highlight = 0;
                        }
                    }
                    break;
            }
            break;
            case JS_EVENT_BUTTON:
            if (event.number == 4 && event.value == 1) {
                // Launch highlighted game with the associated emulator
                Screen *currentSystem = AllSystems[Title];
                char selectedGame[MAX_DISPLAY_NAME_LENGTH];
                strcpy(selectedGame, GameNames[highlight]);

                // Launch the emulator with the selected game
                char command[MAX_DISPLAY_NAME_LENGTH + strlen(currentSystem->Emulator) + 10];
                snprintf(command, sizeof(command), "%s \"%s\" >/dev/null 2>&1", currentSystem->Emulator, selectedGame);
                system(command);
            }
            break;
    }
}

int main() {
    initscr();
    cbreak();
    curs_set(0);
    clear();

    char GameNames[MAX_ROMS][MAX_DISPLAY_NAME_LENGTH];  // Game names for each system

    int GamepadFd = open("/dev/input/js0", O_RDONLY);
    struct js_event event;

    while (1) {
        read(GamepadFd, &event, sizeof(event));
        DrawText(GameNames);
        Joystick(event, GameNames);
    }

    endwin();
    return EXIT_SUCCESS;
}
