//Get rid of this macro. It sould be dynamic
#define MAX_ROMS 1000

static const char *RomsDir[] = {
	"roms/nes",
	"roms/snes",
	"roms/ps1",
	"roms/mega",
	"roms/sat"
};


//Menu titles
static const char *Console[] = {
	"Nintendo",
	"Super Nintendo",
	"Playstation 1",
	"Megadrive",
	"Saturn"
};

const char *ext[] = {
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

static int MinConsoles = 0;
static int MaxConsoles = 4;

// To keep track of wich console is on the screen 
unsigned short int idx = 0;
