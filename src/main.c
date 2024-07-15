#include "common.h"

#define MAX_SYSTEMS 10
#define X_AXIS 6
#define Y_AXIS 7
#define JOYSTICK_DEVICE "/dev/input/js0"
#define BUTTON_SELECT 0
#define lines LINES -2

bool Running = true;

int hovered = 0;
int firstLine = 0;
int systemIndex = 0;
int joystick_fd;

const Game *games;
const Game *all_games[] = {
    nes,
    snes,
    n64,
};

const int game_counts[] = {
    nes_game_count,
    snes_game_count,
    n64_game_count,
};

const char *Systems[] = {
    "Nintendo",
    "Super Nintendo",
    "Nintendo 64",
};

int Num_Folders = (sizeof(Systems) / sizeof(Systems[0]) -1);

void DrawText(const char *consoleName) {
    if (consoleName == NULL) return;

    clear();

    int console_name_len = strlen(consoleName);

    init_pair(1, COLOR_RED, COLOR_BLACK);
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(0, (COLS - console_name_len) / 2, "%s", consoleName);
    attroff(COLOR_PAIR(1) | A_BOLD);

    const Game *games = all_games[systemIndex];

    for (int i = firstLine; i < game_counts[systemIndex] && i < firstLine + lines; i++) {
        int item_len = strlen(games[i].pretty_name);
        int imiddle = (COLS - item_len) / 2;

        if (i == hovered) {
            attron(A_BOLD);
        }
        mvprintw(i - firstLine + 2, 0, "%s", games[i].pretty_name);
        if (i == hovered) {
            attroff(A_BOLD);
        }
    }
    refresh();
}

void OpenFile(const char *command, const char *arg1, const char *arg2, const char *dirname, const char *filename) {
    if (command == NULL || dirname == NULL || filename == NULL) return;

    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/%s", dirname, filename);

    pid_t pid = fork();
    if (pid == 0) {
        int dev_null = open("/dev/null", O_WRONLY);
        if (dev_null != -1) {
            dup2(dev_null, STDOUT_FILENO);
            dup2(dev_null, STDERR_FILENO);
            close(dev_null);
        }

        if (arg1 && strlen(arg1) > 0) {
            execlp(command, command, arg1, arg2, path, (char *)NULL);
        } else {
            execlp(command, command, path, (char *)NULL);
        }

        perror("execlp"); // If execlp fails
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("Error forking process");
    } else {
        waitpid(pid, NULL, 0);
    }
}

void UseKeyboard() {
    DrawText(Systems[systemIndex]);
    games = all_games[systemIndex];

    keypad(stdscr, true);
    int c = wgetch(stdscr);

    switch (c) {
        case KEY_DOWN:
            if (hovered < game_counts[systemIndex] - 1) {
                hovered++;
                if (hovered >= firstLine + lines) {
                    firstLine++;
                } 
            } else {
                hovered = 0;
                firstLine = 0;
            }
            break;
        case KEY_UP:
            if (hovered > 0) {
                hovered--;
                if (hovered < firstLine) {
                    firstLine--;
                }
            }
            break;
        case KEY_RIGHT:
            if (systemIndex < Num_Folders) {
                systemIndex++;
            } else {
                systemIndex = 0;
            }
            hovered = 0;
            firstLine = 0;
            break;
        case KEY_LEFT:
            if (systemIndex > 0) {
                systemIndex--;
            } else {
                systemIndex = Num_Folders;
            }
            hovered = 0;
            firstLine = 0;
            break;
        case KEY_END:
            Running = false;
            break;
        case '\n':
            OpenFile(games[hovered].command, games[hovered].arg1, games[hovered].arg2, games[hovered].dir, games[hovered].real_name);
            break;
    }
}

void UseJoystick() {

    struct js_event event;

    while (read(joystick_fd, &event, sizeof(struct js_event)) > 0) {
        games = all_games[systemIndex];
        //STICK DOWN
        if (event.type == JS_EVENT_AXIS) {                      
            if (event.number == Y_AXIS) {
                if (event.value > 0 ) {
                    if (hovered < game_counts[systemIndex] - 1) {
                        hovered++;
                        if (hovered >= firstLine + lines) {     
                            firstLine++;
                        }
                    } else {
                        hovered = 0;
                        firstLine = 0;
                    }
                    // JOYSTICK_UP
                } 

                if (event.value < 0) {
                    if (hovered > 0) {
                        hovered--;
                        if (hovered < firstLine) {
                            firstLine--;
                        }
                    }
                }
            }

            if (event.number == X_AXIS) {
                if (event.value > 0) {
                    if (systemIndex < Num_Folders ) {
                        systemIndex++;
                    } else {
                        systemIndex = 0;
                    }
                }
                else if (event.value < 0) {
                    if (systemIndex > 0) {
                        systemIndex--;
                    } else {
                        systemIndex = Num_Folders ;
                    }
                }
                hovered = 0;
                firstLine = 0;
            }
        }
        if (event.type == JS_EVENT_BUTTON && event.number == BUTTON_SELECT && event.value == 1) {
            OpenFile(games[hovered].command, games[hovered].arg1, games[hovered].arg2, games[hovered].dir, games[hovered].real_name);
        }
        DrawText(Systems[systemIndex]);
    }
    close(joystick_fd);
}
/*
* TODO: 
*  stop read gamepad if playing
*  hotswap keyboard/gamepad
*/
int main(void) {
    initscr();
    noecho();
    cbreak();
    curs_set(FALSE);
    start_color();

    joystick_fd = open(JOYSTICK_DEVICE, O_RDONLY, O_NONBLOCK);

    while (Running) {
        if (joystick_fd == -1) {
            UseKeyboard();
        } else {
            UseJoystick();
        }
    }
    endwin();
    return EXIT_SUCCESS;
}
