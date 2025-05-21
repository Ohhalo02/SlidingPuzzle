#ifndef _DEFS__H
#define _DEFS__H

#include <SDL.h>

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;
const char* WINDOW_TITLE = "Sliding Puzzle";

#define BOARD_SIZE 3
#define CELL_SIZE 150
#define BOARD_X 75
#define BOARD_Y 75
#define EMPTY_CELL 0

const char* FONT_PATH = "assets/Purisa-BoldOblique.ttf";
const char* BACKGROUND_IMG = "assets/background.jpg";
#define MENU_BACKGROUND_IMG "assets/menu_background.png"

const int MENU_OPTION_COUNT = 3;
const char* MENU_OPTIONS[MENU_OPTION_COUNT] = {"Play", "Quit", "Sound"};
const int MENU_Y_START = 250;
const int MENU_SPACING = 80;

const char* MUSIC_PATH = "assets/background_music.mp3";

// Sound Setting Page
const int SOUND_SETTING_WIDTH = 400;
const int SOUND_SETTING_HEIGHT = 300;
const int SOUND_SETTING_X = (SCREEN_WIDTH - SOUND_SETTING_WIDTH) / 2;
const int SOUND_SETTING_Y = (SCREEN_HEIGHT - SOUND_SETTING_HEIGHT) / 2;

// Căn chỉnh thanh trượt
const int SLIDER_WIDTH = 250; // Giảm chiều rộng để cân đối hơn
const int SLIDER_HEIGHT = 20;
const int SLIDER_X = SOUND_SETTING_X + (SOUND_SETTING_WIDTH - SLIDER_WIDTH) / 2; // Căn giữa theo chiều ngang
const int SLIDER_Y = SOUND_SETTING_Y + 120; // Điều chỉnh để nằm giữa Sound On/Off và Back
const int SLIDER_MAX = 128; // Âm lượng tối đa
const int BUTTON_WIDTH = 150;
const int BUTTON_HEIGHT = 50;

#endif
