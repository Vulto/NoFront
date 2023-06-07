// I decided to make it statically defined to avoid memory management.
#define MAX_ROMS 100

static const char *RomsDir[] = {
	"roms/nes",
	"roms/snes",
	"roms/n64",
};

static const char *Emulators[] = {
	"mednafen",
	"mupen64",
	"fbneo",
};

//Menu titles
static const char *Console[] = {
	"Nintendo",
	"Super Nintendo",
	"Nintendo 64",
};

const char *extensions[] = {
	".smc",
	".sfc",
	".fig", 
	".swc", 
	".nes", 
	".z64", 
	".n64", 
	".zip"
};

// Just adding some "readability"
static unsigned short int MinConsoles = 0;
static unsigned short int MaxConsoles = 2; // Start counting from 0, so 2 == 3

// To keep track of wich console is on the screen 
unsigned short int idx = 0;
