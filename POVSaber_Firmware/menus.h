uint8_t _menu_item = 0;
uint8_t _menu = 0;

#define MENU_MAIN 0
#define MENU_MAIN_LEN 3
#define MENU_MAIN_TITLE "   Main Menu   "
String _menu_main[] = {
    "Brightness",
    "POV Files",
    "Sabers"
};

#define MENU_FILES 1
#define MENU_FILES_TITLE "   POV Files   "

#define MENU_SABER 2
#define MENU_SABER_LEN 3
#define MENU_SABER_TITLE "    Sabers     "
String _menu_sabers[] = {
    "Green",
    "Red",
    "Kylo"
};

#define MENU_POV 9
