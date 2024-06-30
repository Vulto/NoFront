#include <stdio.h>
#include <stdlib.h>
#include <linux/joystick.h>
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAX_ITEMS 100
#define MAX_SYSTEMS 100

// Always check this values. They are specific for each gamepad.
#define X_AXIS 6
#define Y_AXIS 7

bool Running = true;
static char *items[MAX_ITEMS] = { NULL };
static char *systems[MAX_SYSTEMS] = { NULL };

int numItems = 0;
int highlight = 0;
int firstLine = 0;
int sdx = 0;
int numConsoles = 0;

const char *Folders[] = {
    "Nintendo",
    "Super Nintendo",
    "Nintendo 64",
    "Neo Geo",
};

int Num_Folders = sizeof(Folders) / sizeof(Folders[0]);

void ReadFolder(const char *dirname, char *items[], int *numItems) {
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(dirname)) == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG) {
            items[*numItems] = strdup(ent->d_name);
            (*numItems)++;
            if (*numItems >= MAX_ITEMS) break;
        }
    }
    closedir(dir);
}

void DrawText(const char *consoleName) {
    clear();
    initscr();
    start_color();
    cbreak();
    curs_set(0);

    init_pair(1, COLOR_RED, COLOR_BLACK);
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(0, COLS / 2 - strlen(consoleName) / 2, "%s", consoleName);
    attroff(COLOR_PAIR(1) | A_BOLD);

    int lines = LINES - 2; // Number of lines available for items
    for (int i = firstLine; i < numItems && i < firstLine + lines; i++) {
        int imiddle = (COLS - strlen(items[i])) / 2;

        if (i == highlight) {
            attron(A_BOLD);
        }
        mvprintw(i - firstLine + 2, imiddle, "%s", items[i]);
        attroff(A_BOLD);
    }

    refresh();
}

void OpenFile(const char *dirname, const char *filename) {
    char path[strlen(dirname) + strlen(filename) + 2]; // +2 for '/' and null terminator
    sprintf(path, "%s/%s", dirname, filename);
    char *args[] = {"nopen", path, NULL};
    pid_t pid = fork();
    if (pid == 0) {
        // Redirect standard output and standard error to /dev/null
        int dev_null = open("/dev/null", O_WRONLY);
        dup2(dev_null, STDOUT_FILENO);
     //   dup2(dev_null, STDERR_FILENO);
        close(dev_null);

        execvp(args[0], args);
        exit(EXIT_FAILURE); // If execvp fails
    } else if (pid < 0) {
        perror("Error forking process");
    } else {
        waitpid(pid, NULL, 0);
    }
}

void UseKeyboard() {
    keypad(stdscr, true);
    int c = wgetch(stdscr);
    int lines = LINES - 2; // Number of lines available for items
    switch (c) {
        case KEY_DOWN:
            if (highlight < numItems - 1) {
                highlight++;
                if (highlight >= firstLine + lines) {
                    firstLine++;
                }
            }
            break;
        case KEY_UP:
            if (highlight > 0) {
                highlight--;
                if (highlight < firstLine) {
                    firstLine--;
                }
            }
            break;
        case KEY_LEFT:
            sdx--;
            if (sdx < 0) {
                sdx = numConsoles - 1;
            }
            numItems = 0;
            ReadFolder(systems[sdx], items, &numItems);
            highlight = 0;
            firstLine = 0;
            break;
        case KEY_RIGHT:
            sdx++;
            if (sdx >= numConsoles) {
                sdx = 0;
            }
            numItems = 0;
            ReadFolder(systems[sdx], items, &numItems);
            highlight = 0;
            firstLine = 0;
            break;
        case KEY_END:
            Running = false;
            break;
        case '\n':
            if (highlight >= 0 && highlight < numItems) {
                OpenFile(systems[sdx], items[highlight]);
            }
            break;
    }
}

int joystick_fd;

void UseJoystick() {
    joystick_fd = open("/dev/input/js0", O_RDONLY);

    struct js_event event;
    while (read(joystick_fd, &event, sizeof(struct js_event)) > 0) {
        if (event.type == JS_EVENT_AXIS) {
            if (event.number == Y_AXIS) {
                if (event.value > 0) {
                    if (highlight < numItems - 1) {
                        highlight++;
                        if (highlight >= firstLine + (LINES - 2)) {
                            firstLine++;
                        }
                    }
                } else if (event.value < 0) {
                    if (highlight > 0) {
                        highlight--;
                        if (highlight < firstLine) {
                            firstLine--;
                        }
                    }
                }
            } else if (event.number == X_AXIS) {
                if (event.value > 0) {
                    sdx++;
                    if (sdx >= numConsoles) {
                        sdx = 0;
                    }
                    numItems = 0;
                    ReadFolder(systems[sdx], items, &numItems);
                    highlight = 0;
                    firstLine = 0;
                } else if (event.value < 0) {
                    sdx--;
                    if (sdx < 0) {
                        sdx = numConsoles - 1;
                    }
                    numItems = 0;
                    ReadFolder(systems[sdx], items, &numItems);
                    highlight = 0;
                    firstLine = 0;
                }
            }
        } else if (event.type == JS_EVENT_BUTTON && event.value == 1) {
            if (event.number == 0) { // Assuming button 0 is for selection
                    OpenFile(systems[sdx], items[highlight]);
            } 
        }
        DrawText(systems[sdx]);
    }
    close(joystick_fd);
}

int main() {
    for (int i = 0; i < Num_Folders; i++) {
        systems[numConsoles] = strdup(Folders[i]);
        numConsoles++;
    }

    if (numConsoles > 0) {
        ReadFolder(systems[sdx], items, &numItems);
    }

    while (Running) {
        DrawText(systems[sdx]);

        UseJoystick();

        if (joystick_fd == -1) {
            UseKeyboard();
        }

    }

    for (int i = 0; i < numConsoles; i++) {
        free(systems[i]);
    }

    for (int i = 0; i < numItems; i++) {
        free(items[i]);
    }

    endwin();
    return EXIT_SUCCESS;
}
