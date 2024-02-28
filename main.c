#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/joystick.h>
#include <dirent.h>
#include <ncurses.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>

#include "config.h"

#define MAX_EVENT_ID 25
#define BUFFER_SIZE 128

unsigned short int highlight = 1; // The cursor
unsigned short int start_index = 0; // This is were menu start to appear
bool running = true;
char game_names[MAX_ROMS][256];

char command[512];

bool gamepad;

// To keep track of wich console is on the screen 
unsigned short int idx = 0;

int num_extensions = sizeof(ext) / sizeof(ext[0]);

int TestDir(const char *path) {

    DIR *dir = opendir(path);
    if (dir) {
        closedir(dir);
        return EXIT_SUCCESS; // Directory exists
    } else {
        return EXIT_FAILURE; // Directory does not exist
    }
}

int num_RomsDir = sizeof(RomsDir) / sizeof(RomsDir[0]);

int CreateDirs() {
    const char roms_dir[] = "roms";

    if (mkdir(roms_dir, 0777) == -1) {
        fprintf(stderr, "Failed to create directory: %s\n", roms_dir);
        return EXIT_SUCCESS;
    }

    for (int j = 0; j < num_RomsDir; j++) {
        char current_path[100];
        snprintf(current_path, sizeof(current_path), "%s", RomsDir[j]);

        if (mkdir(current_path, 0777) == -1) {
            printf("Failed to create directory: %s\n", current_path);
            return 1;
        }
    }
    return EXIT_SUCCESS;
}

int FirstRun() {
    cbreak();
    curs_set(0);
    // Calculate the dimensions and position of the subwindow
    int subwinHeight = LINES / 2;
    int subwinWidth = COLS / 2;
    int subwinY = (LINES - subwinHeight) / 2;
    int subwinX = (COLS - subwinWidth) / 2;

    // Create the subwindow
    WINDOW *subwin = newwin(subwinHeight, subwinWidth, subwinY, subwinX);

    // Set a border around the subwindow
    box(subwin, 0, 0);

    // Print text in the subwindow
    static const char MessageInfo[] = {"There's no Roms folder in here"};
    static const char MessageCreation[] = {"Would you like to create it? y/n"};
    static const char MessageSuccess[] = {"Roms dir created successfully!"};
    static const char MessageReload[] = {"   Press any key to reload    "};
    int textLength = strlen(MessageInfo);
    int textX = (subwinWidth - textLength) / 2; // Centered horizontally
    int textY = (subwinHeight - 4) / 2; // Centered vertically

    mvwprintw (subwin, textY, textX, "%s", MessageInfo);
    mvwprintw (subwin, (textY + 2), textX, "%s", MessageCreation);
    wmove(subwin, 0, 0);

    char yn;

    yn = wgetch(subwin);
    if(yn == 'y') {
        if(CreateDirs() == EXIT_SUCCESS){
            wclear(subwin);
            box(subwin, 0, 0);
            mvwprintw (subwin, textY, textX, "%s", MessageSuccess);
            mvwprintw (subwin, (textY + 2), textX, "%s", MessageReload);
            wgetch(subwin);
            return EXIT_SUCCESS;
        }else{
            fprintf(stderr, "[ERROR] Could not create Roms dir...");
            return EXIT_FAILURE;
        }
    }
    if(yn == 'n') {
        fprintf(stderr, "[INFO] User cancel creation of Roms dir...");
        return EXIT_FAILURE;
        delwin(subwin);
    }
    delwin(subwin);
    return EXIT_SUCCESS;
}



int CheckExt(const char* name) {
    int len = strlen(name);
    const char* file_extension = name + len - 4;

    for (int i = 0; i < num_extensions; i++) {
        if (strcmp(file_extension, ext[i]) == 0) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

void FormatNames(char* name) {
    char* ptr = name;
    while (*ptr) {
        if (*ptr == '(' || *ptr == '[') {
            *ptr = '\0';
            break;
        }
        ptr++;
    }
}

int get_game_names() {
    DIR *dir = opendir(RomsDir[idx]);
    if (!dir) {
        perror("opendir");
        endwin();
        return EXIT_FAILURE;
    }

    // Store stripped and unstripped names of all valid ROMs in arrays
    struct dirent *ent;
    int num_roms = 0;
    char stripped_name[50];
    while ((ent = readdir(dir)) != NULL && num_roms < MAX_ROMS) {
        if (CheckExt(ent->d_name)) {
            // Remove strange characters from filename
            strncpy(stripped_name, ent->d_name, 50);
            FormatNames(stripped_name);

            // Copy stripped and unstripped names to game_names array
            strncpy(game_names[num_roms], stripped_name, 50);
            strcpy(game_names[num_roms] + strlen(stripped_name) + 1, ent->d_name);
            num_roms++;
        }
    }
    closedir(dir);
    return num_roms;
}

void run() {
    sprintf(command, "%s \"%s/%s\" >/dev/null 2>&1 &","nopen", RomsDir[idx], game_names[highlight - 1] + strlen(game_names[highlight - 1]) + 1);
    system(command);
}

void AddFav(){
    printf("Not implemented");
}


int main() {
    initscr();
    keypad(stdscr, true);
    cbreak();
    curs_set(0);

    int GamepadFd = open("/dev/input/js0", O_RDONLY);

    if(GamepadFd  != -1 ) {
        gamepad = true;
    } else {
        gamepad = false;
    }
    // Initiate GamePad input tracking
    struct js_event event;

    const char path[] = "roms";
    if (TestDir(path) == EXIT_FAILURE) {
        FirstRun();
    }	

    while (running) {
        clear();

        int num_roms = get_game_names();

        attron(A_DIM);
        mvprintw(0, (COLS - strlen(Console[idx])) / 2, "%s", Console[idx]);
        attroff(A_DIM);


        // Adjust the max displayable items based on the updated ROM count
        int max_displayable_items = num_roms > LINES - 2 ? LINES - 2 : num_roms;

        for (int i = 0; i < max_displayable_items; i++) {
            int rom_index = start_index + i;

            if(highlight > num_roms) {
                highlight = num_roms;
            }
            if (rom_index == highlight - 1) {
                attron(A_BOLD);
            }
            if (rom_index < num_roms) {
                mvprintw(i + 1, 0, " %s ", game_names[rom_index]);
            }
            attroff(A_BOLD);
        }

        refresh();

        if (gamepad == true) {
            read(GamepadFd, &event, sizeof(event));
            switch (event.type) {
                case JS_EVENT_AXIS:
                    switch (event.number) {
                        case 7: // Y AXIS
                            if (event.value < 0 && highlight > 1) {
                                highlight--;
                            } else if (highlight < start_index + 1) {
                                start_index--;
                            }
                            if (event.value > 0 && highlight < num_roms) {
                                highlight++;
                            } else if (highlight > start_index + max_displayable_items) {
                                start_index++;
                            }
                        break;
                        case 6: // X AXIS
                            if (event.value < 0) {
                                // Changing system with Left 
                                if (idx > MinConsoles) {
                                    idx--;
                                    start_index = 0; // reset scroll to avoid blank menu
                                    highlight = 1; // highlight the first option
                                }
                                break;
                            }
                            if (event.value > 0) {
                                if (idx < MaxConsoles) {
                                    idx++;
                                    start_index = 0; //return to the top
                                    highlight = 1; // highlight first option
                                }
                                break;
                        }
                    }
                    break;
                case JS_EVENT_BUTTON:
                    if (event.number == 4 && event.value == 1) {
                        run();
                        system(command);
                        break;
                }
                default:
                    break;
            }
        } else {
            int c;
            // Get user input
            c = wgetch(stdscr);

            switch (c) {
                case KEY_UP: // UP
                    if (highlight > 1) {
                        highlight--;
                        if (highlight < start_index + 1) {
                            start_index--;
                        }
                    }
                    break;

                case KEY_DOWN: // DOWN
                    if (highlight < num_roms) {
                        highlight++;
                        if (highlight > start_index + max_displayable_items) {
                            start_index++;
                        }
                    }
                    break;

                case '\n': // LAUNCH
                    run();
                    break;

                case KEY_INSERT: // LAUNCH
                    AddFav();
                    break;

                case KEY_LEFT:
                    if (idx > MinConsoles) {
                        idx--;
                        start_index = 0; // reset scroll to avoid blank menu
                        highlight = 1; // highlight the first option
                    }
                    break;

                case KEY_RIGHT:
                    if (idx < MaxConsoles) {
                        idx++;
                        start_index = 0; // reset scroll to avoid blank menu
                        highlight = 1; // highlight the first option
                    }
                    break;

                case 'q': // EXIT
                    running = false;
                    break;
                default:
                    running = false;
                    break;
            }
        }
    }
    endwin();
    return EXIT_SUCCESS;
}
