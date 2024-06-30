#include "commom.h"

#define ROM_PATH "/home/vulto/menu/systems/snes/roms"

void add_files_to_snes_array(Game **snes, int *snes_game_count) {
    DIR *d;
    struct dirent *dir;
    d = opendir(ROM_PATH);

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG) { // if it's a regular file
                Game new_game;
                new_game.dir = ROM_PATH;
                new_game.real_name = strdup(dir->d_name); // dynamically allocate memory for the file name
                new_game.pretty_name = strdup(dir->d_name); // you can modify this to set a proper title
                new_game.command = "mednafen";
                new_game.arg1 = "";
                new_game.arg2 = "";

                // Check if the file already exists in the array
                int exists = 0;
                for (int i = 0; i < *snes_game_count; i++) {
                    if (strcmp((*snes)[i].real_name, dir->d_name) == 0) {
                        exists = 1;
                        break;
                    }
                }

                if (!exists) {
                    // Resize the array to fit the new game
                    *snes = realloc(*snes, (*snes_game_count + 1) * sizeof(Game));
                    (*snes)[*snes_game_count] = new_game;
                    (*snes_game_count)++;
                }
            }
        }
        closedir(d);
    }
}

void write_snes_h(const Game *snes, int snes_game_count) {
    FILE *file = fopen("snes.h", "w");
    if (file) {
        fprintf(file, "#ifndef SNES_GAMES_H\n");
        fprintf(file, "#define SNES_GAMES_H\n\n");
        fprintf(file, "#include \"../nes/nes.h\"  // Include Game struct definition if not already included\n\n");
        fprintf(file, "const Game snes[] = {\n");

        for (int i = 0; i < snes_game_count; i++) {
            fprintf(file, "    {\"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\"}%s\n",
                    snes[i].dir, snes[i].real_name, snes[i].pretty_name, snes[i].command, snes[i].arg1, snes[i].arg2, (i < snes_game_count - 1) ? "," : "");
        }

        fprintf(file, "};\n");
        fprintf(file, "const int snes_game_count = sizeof(snes) / sizeof(snes[0]);\n\n");
        fprintf(file, "#endif // SNES_GAMES_H\n");
        fclose(file);
    }
}

int main() {
    // Load initial games from snes.h
    extern const Game snes[];
    extern const int snes_game_count;
    
    int current_snes_game_count = snes_game_count;
    Game *current_snes = malloc(current_snes_game_count * sizeof(Game));
    memcpy(current_snes, snes, current_snes_game_count * sizeof(Game));

    add_files_to_snes_array(&current_snes, &current_snes_game_count);

    write_snes_h(current_snes, current_snes_game_count);

    // Free dynamically allocated memory for real_name and pretty_name
    for (int i = 0; i < current_snes_game_count; i++) {
        free((void*)current_snes[i].real_name);
        free((void*)current_snes[i].pretty_name);
    }

    free(current_snes);
    return 0;
}
