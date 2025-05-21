#include <iostream>
#include <SDL.h>
#include "defs.h"
#include "graphics.h"
#include "logic.h"

using namespace std;

enum GameState { MENU, PLAYING, SOUND_SETTING };
void processClick(int x, int y, SlidingPuzzle& game, Graphics& graphics, GameState& state);
void handleMenuInput(SDL_Event& event, int& selectedOption, GameState& state, SlidingPuzzle& game, Graphics& graphics, bool& quit);

int main(int argc, char* argv[]) {
    Graphics graphics;
    graphics.init();

    SlidingPuzzle game;

    GameState state = MENU;
    int selectedOption = 0;
    bool quit = false;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (state) {
                case MENU:
                    handleMenuInput(event, selectedOption, state, game, graphics, quit);
                    break;
                case PLAYING:
                    if (event.type == SDL_QUIT) {
                        quit = true;
                    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                        int x, y;
                        SDL_GetMouseState(&x, &y);
                        processClick(x, y, game, graphics, state);
                    } else if (event.type == SDL_KEYDOWN) {
                        if (event.key.keysym.sym == SDLK_r) {
                            game.init();
                        } else if (event.key.keysym.sym == SDLK_m) {
                            state = MENU;
                        }
                    }
                    if (game.isSolved() && !game.gaveUp) { // Chỉ cập nhật highScore nếu không Give Up
                        game.updateHighScore();
                    }
                    break;
                case SOUND_SETTING:
                    if (event.type == SDL_QUIT) {
                        quit = true;
                    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                        int x, y;
                        SDL_GetMouseState(&x, &y);
                        // Kiểm tra nút Sound On/Off
                        SDL_Rect soundButton = {SOUND_SETTING_X + 125, SOUND_SETTING_Y + 50, BUTTON_WIDTH, BUTTON_HEIGHT};
                        if (x >= soundButton.x && x <= soundButton.x + soundButton.w &&
                            y >= soundButton.y && y <= soundButton.y + soundButton.h) {
                            graphics.isMusicPlaying = !graphics.isMusicPlaying;
                            graphics.playMusic();
                            graphics.renderSoundSetting();
                        }
                        // Kiểm tra thanh trượt
                        SDL_Rect sliderBg = {SLIDER_X, SLIDER_Y, SLIDER_WIDTH, SLIDER_HEIGHT};
                        if (x >= sliderBg.x && x <= sliderBg.x + sliderBg.w &&
                            y >= sliderBg.y && y <= sliderBg.y + sliderBg.h) {
                            int newValue = ((x - SLIDER_X) * SLIDER_MAX) / SLIDER_WIDTH;
                            graphics.sliderValue = newValue;
                            if (graphics.sliderValue < 0) graphics.sliderValue = 0;
                            if (graphics.sliderValue > SLIDER_MAX) graphics.sliderValue = SLIDER_MAX;
                            graphics.musicVolume = graphics.sliderValue;
                            Mix_VolumeMusic(graphics.musicVolume);
                            graphics.renderSoundSetting();
                        }
                        // Kiểm tra nút Back
                        SDL_Rect backButton = {SOUND_SETTING_X + 125, SOUND_SETTING_Y + 200, BUTTON_WIDTH, BUTTON_HEIGHT};
                        if (x >= backButton.x && x <= backButton.x + backButton.w &&
                            y >= backButton.y && y <= backButton.y + backButton.h) {
                            graphics.showSoundSetting = false;
                            state = MENU;
                            graphics.renderMenu(selectedOption, game.moveCount, game.highScore);
                        }
                    }
                    break;
            }
        }

        if (state == MENU) {
            graphics.renderMenu(selectedOption, game.moveCount, game.highScore);
        } else if (state == PLAYING) {
            graphics.render(game);
        } else if (state == SOUND_SETTING) {
            graphics.renderSoundSetting();
        }
    }

    graphics.quit();
    return 0;
}

void processClick(int x, int y, SlidingPuzzle& game, Graphics& graphics, GameState& state) {
    if (!game.isSolved()) {
        // Kiểm tra nhấn nút Give Up
        SDL_Rect giveUpButton = {SCREEN_WIDTH - 210, SCREEN_HEIGHT - 60, 200, 50};
        if (x >= giveUpButton.x && x <= giveUpButton.x + giveUpButton.w &&
            y >= giveUpButton.y && y <= giveUpButton.y + giveUpButton.h) {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Give Up clicked at (%d, %d)", x, y);
            game.giveUp();
            graphics.render(game);
            return;
        }

        // Kiểm tra nhấn ô trên bàn cờ
        int clickedCol = (x - BOARD_X) / CELL_SIZE;
        int clickedRow = (y - BOARD_Y) / CELL_SIZE;
        if (clickedRow >= 0 && clickedRow < BOARD_SIZE && clickedCol >= 0 && clickedCol < BOARD_SIZE) {
            game.move(clickedRow, clickedCol);
            graphics.render(game);
        }
    } else {
        // Kiểm tra nhấn nút Back
        SDL_Rect backButton = {(SCREEN_WIDTH - 200) / 2, SCREEN_HEIGHT / 2 + 80, 200, 50};
        if (x >= backButton.x && x <= backButton.x + backButton.w &&
            y >= backButton.y && y <= backButton.y + backButton.h) {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Back clicked at (%d, %d)", x, y);
            state = MENU;
            game.init();
            game.resetMoves();
            graphics.renderMenu(0, game.moveCount, game.highScore);
        }
    }
}

void handleMenuInput(SDL_Event& event, int& selectedOption, GameState& state, SlidingPuzzle& game, Graphics& graphics, bool& quit) {
    switch (event.type) {
        case SDL_QUIT:
            quit = true;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_UP) {
                selectedOption = (selectedOption - 1 + MENU_OPTION_COUNT) % MENU_OPTION_COUNT;
            } else if (event.key.keysym.sym == SDLK_DOWN) {
                selectedOption = (selectedOption + 1) % MENU_OPTION_COUNT;
            } else if (event.key.keysym.sym == SDLK_RETURN) {
                if (selectedOption == 0) { // Play
                    graphics.showSoundSetting = false;
                    state = PLAYING;
                    game.init();
                    game.resetMoves();
                } else if (selectedOption == 1) { // Quit
                    quit = true;
                } else if (selectedOption == 2) { // Sound
                    graphics.showSoundSetting = true;
                    state = SOUND_SETTING;
                    graphics.renderSoundSetting();
                }
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            for (int i = 0; i < MENU_OPTION_COUNT; i++) {
                SDL_Rect rect = {(SCREEN_WIDTH - 200) / 2, MENU_Y_START + i * MENU_SPACING, 200, 50};
                if (mouseX >= rect.x && mouseX <= rect.x + rect.w &&
                    mouseY >= rect.y && mouseY <= rect.y + rect.h) {
                    selectedOption = i;
                    if (selectedOption == 0) { // Play
                        graphics.showSoundSetting = false;
                        state = PLAYING;
                        game.init();
                        game.resetMoves();
                    } else if (selectedOption == 1) { // Quit
                        quit = true;
                    } else if (selectedOption == 2) { // Sound
                        graphics.showSoundSetting = true;
                        state = SOUND_SETTING;
                        graphics.renderSoundSetting();
                    }
                    break;
                }
            }
            break;
    }
}
