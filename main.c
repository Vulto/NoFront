#include "main.h"
#include "firstrun.h"

int main() {
    initscr();
    keypad(stdscr, true);
    cbreak();
    curs_set(0);

    unsigned short int highlight = 1;
    bool running = true;

    // Index of first item displayed in the menu
    unsigned short int start_index = 0;

    int GamepadFd = open("/dev/input/js0", O_RDONLY);
	
	const char path[] = "roms";
	if (TestDir(path) == EXIT_FAILURE) {
		FirstRun();
	}	

    // Initiate GamePad input tracking
    struct js_event event;

    while (running) {
        clear();

        attron(A_BOLD);
        mvprintw(0, (COLS - strlen(Console[idx])) / 2, "%s", Console[idx]);
        attroff(A_BOLD);

        char game_names[MAX_ROMS][256];
        int num_roms = get_game_names(game_names, idx);

        // Adjust the max displayable items based on the updated ROM count
        unsigned short int max_displayable_items = num_roms > LINES - 2 ? LINES - 2 : num_roms;

        for (int i = 0; i < max_displayable_items; i++) {
            unsigned short int rom_index = start_index + i;

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

        if (read(GamepadFd, &event, sizeof(event)) > 0) {
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
									start_index = 0; // reset scroll to avoid blank menu
									highlight = 1; // highlight the first option
                                }
                                break;
                            }
                    }
                    break;
                case JS_EVENT_BUTTON:
                    if (event.number == 4 && event.value == 1) {
sprintf(command, "mednafen \"%s/%s\" >/dev/null 2>&1 &", RomsDir[idx], game_names[highlight - 1] + strlen(game_names[highlight - 1]) + 1);
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
                    sprintf(command, "mednafen \"%s/%s\" >/dev/null 2>&1 &", RomsDir[idx], game_names[highlight - 1] + strlen(game_names[highlight - 1]) + 1);
                    system(command);
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
            }
        }
    }
    endwin();
    return EXIT_SUCCESS;
}
