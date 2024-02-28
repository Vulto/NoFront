#define NOBUILD_IMPLEMENTATION
#include "./nobuild.h"
#include <string.h>

#define CFLAGS  "-Wall", \
                "-Wextra", \
                "-Wswitch-enum", \
                "-std=c2x", "-pedantic", \
                "-pedantic-errors", \
                "-Wmissing-include-dirs", \
                "-lncurses", "-O3"

#define BIN "nf"
#define SOURCE "main.c"
#define DESTDIR "/usr/local/bin"
#define CC "clang"

const char *cc(void){
    const char *result = getenv("CC");
    return result ? result : "cc";
}

int posix_main(int argc, char **argv){
    CMD(CC, CFLAGS, "-o", BIN, SOURCE);

    if (argc > 1){
        if (strcmp(argv[1], "run") == 0){
            CMD(BIN);
        }else if (strcmp(argv[1], "install") == 0){
            CMD("doas", "cp", BIN, DESTDIR);
        }else{
            PANIC("%s is unknown subcommand", argv[1]);
        }
    }

    return 0;
}

int main(int argc, char **argv){
    GO_REBUILD_URSELF(argc, argv);

    return posix_main(argc, argv);
}
