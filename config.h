// I decided to make it statically defined to avoid memory management.
#define MAX_ROMS 100

static const char *RomsDir[] = {
	"roms/nes",
	"roms/snes",
	"roms/ps1",
	"roms/mega",
	"roms/sat"
};

static const char *Emulator[] = {
	"mednafen"
};

//Menu titles
static const char *Console[] = {
	"Nintendo",
	"Super Nintendo",
	"Playstation 1",
	"Megadrive",
	"Saturn"
};

const char *extensions[] = {
	".smc",
	".sfc",
	".fig", 
	".swc", 
	".nes", 
	".z64", 
	".n64", 
	".md",
	".cue"
};

// Just adding some "readability"
static unsigned short int MinConsoles = 0;
static unsigned short int MaxConsoles = 4; // Start counting from 0, so 2 == 3

// To keep track of wich console is on the screen 
unsigned short int idx = 0;
