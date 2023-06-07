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
