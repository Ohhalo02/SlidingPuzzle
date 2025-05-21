#ifndef _GRAPHICS__H
#define _GRAPHICS__H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "defs.h"
#include "logic.h"

struct Graphics {
    SDL_Renderer *renderer;
    SDL_Window *window;
    TTF_Font *font;
    SDL_Texture *cellTextures[BOARD_SIZE * BOARD_SIZE];
    Mix_Music *backgroundMusic;
    int musicVolume; // Âm lượng (0-128)
    bool isMusicPlaying;
    bool showSoundSetting; // Trạng thái hiển thị trang Sound Setting
    int sliderValue; // Giá trị thanh trượt

    void logErrorAndExit(const char* msg, const char* error) {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "%s: %s", msg, error);
        SDL_Quit();
        exit(1);
    }

    void init() {
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
            logErrorAndExit("SDL_Init", SDL_GetError());

        window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == nullptr)
            logErrorAndExit("CreateWindow", SDL_GetError());

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (renderer == nullptr)
            logErrorAndExit("CreateRenderer", SDL_GetError());

        if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))
            logErrorAndExit("SDL_image error:", IMG_GetError());

        if (TTF_Init() == -1)
            logErrorAndExit("SDL_ttf could not initialize!", TTF_GetError());

        font = TTF_OpenFont(FONT_PATH, 50);
        if (font == nullptr)
            logErrorAndExit("Load font", TTF_GetError());

        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
            logErrorAndExit("SDL_mixer could not initialize!", Mix_GetError());

        backgroundMusic = Mix_LoadMUS(MUSIC_PATH);
        if (backgroundMusic == nullptr)
            logErrorAndExit("Failed to load music!", Mix_GetError());

        musicVolume = MIX_MAX_VOLUME; // Âm lượng tối đa ban đầu
        isMusicPlaying = true;
        showSoundSetting = false; // Ban đầu không hiển thị
        sliderValue = musicVolume; // Khởi tạo giá trị thanh trượt
        playMusic();
        Mix_VolumeMusic(musicVolume); // Đặt âm lượng ban đầu

        for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) {
            cellTextures[i] = nullptr;
        }

        for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) {
            char path[50];
            sprintf(path, "assets/cell_%d.png", i);
            cellTextures[i] = loadTexture(path);
            if (cellTextures[i] == nullptr) {
                sprintf(path, "assets/cell_0.png");
                cellTextures[i] = loadTexture(path);
                if (cellTextures[i] == nullptr) {
                    logErrorAndExit("Failed to load default texture", IMG_GetError());
                }
            }
        }
    }

    void playMusic() {
        if (isMusicPlaying) {
            if (!Mix_PlayingMusic()) { // Chỉ phát nếu nhạc không đang chạy
                Mix_PlayMusic(backgroundMusic, -1); // Phát lặp lại
            } else {
                Mix_ResumeMusic(); // Tiếp tục nếu nhạc bị tạm dừng
            }
            Mix_VolumeMusic(musicVolume); // Cập nhật âm lượng
        } else {
            Mix_PauseMusic(); // Tạm dừng nhạc
        }
    }

    SDL_Texture* loadTexture(const char* filename) {
        SDL_Texture* texture = IMG_LoadTexture(renderer, filename);
        if (texture == NULL) {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Failed to load texture %s: %s", filename, IMG_GetError());
        }
        return texture;
    }

    SDL_Texture* renderText(const char* text, SDL_Color textColor, int fontSize = 50) {
        TTF_Font* tempFont = TTF_OpenFont(FONT_PATH, fontSize);
        if (tempFont == nullptr) {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Failed to load font size %d: %s", fontSize, TTF_GetError());
            return nullptr;
        }
        SDL_Surface* textSurface = TTF_RenderText_Solid(tempFont, text, textColor);
        TTF_CloseFont(tempFont);
        if (textSurface == nullptr) {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Render text surface %s", TTF_GetError());
            return nullptr;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (texture == nullptr) {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Create text texture %s", SDL_GetError());
        }
        SDL_FreeSurface(textSurface);
        return texture;
    }

    void renderTexture(SDL_Texture* texture, int x, int y) {
        if (texture == nullptr) return;
        SDL_Rect dest;
        dest.x = x;
        dest.y = y;
        SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
        SDL_RenderCopy(renderer, texture, NULL, &dest);
    }

    void renderMenu(int selectedOption, int moveCount, int highScore) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Texture* background = loadTexture(MENU_BACKGROUND_IMG);
        if (background != nullptr) {
            renderTexture(background, 0, 0);
            SDL_DestroyTexture(background);
        }

        SDL_Color white = {255, 255, 255, 255};
        for (int i = 0; i < MENU_OPTION_COUNT; i++) {
            SDL_Rect rect = {(SCREEN_WIDTH - 200) / 2, MENU_Y_START + i * MENU_SPACING, 200, 50};
            if (i == selectedOption) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            }
            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &rect);

            const char* text = MENU_OPTIONS[i];
            SDL_Texture* textTexture = renderText(text, white);
            if (textTexture != nullptr) {
                int textW, textH;
                SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
                int textX = rect.x + (rect.w - textW) / 2;
                int textY = rect.y + (rect.h - textH) / 2;
                renderTexture(textTexture, textX, textY);
                SDL_DestroyTexture(textTexture);
            }
        }

        SDL_RenderPresent(renderer);
    }

    void renderSoundSetting() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Vẽ nền Sound Setting
        SDL_Rect bgRect = {SOUND_SETTING_X, SOUND_SETTING_Y, SOUND_SETTING_WIDTH, SOUND_SETTING_HEIGHT};
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(renderer, &bgRect);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &bgRect);

        SDL_Color white = {255, 255, 255, 255};
        // Vẽ nút Sound On/Off
        SDL_Rect soundButton = {SOUND_SETTING_X + 125, SOUND_SETTING_Y + 50, BUTTON_WIDTH, BUTTON_HEIGHT};
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(renderer, &soundButton);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &soundButton);
        const char* soundText = isMusicPlaying ? "Sound: On" : "Sound: Off";
        SDL_Texture* soundTextTexture = renderText(soundText, white, 30);
        if (soundTextTexture != nullptr) {
            int textW, textH;
            SDL_QueryTexture(soundTextTexture, NULL, NULL, &textW, &textH);
            int textX = soundButton.x + (soundButton.w - textW) / 2;
            int textY = soundButton.y + (soundButton.h - textH) / 2;
            renderTexture(soundTextTexture, textX, textY);
            SDL_DestroyTexture(soundTextTexture);
        }

        // Vẽ thanh trượt
        SDL_Rect sliderBg = {SLIDER_X, SLIDER_Y, SLIDER_WIDTH, SLIDER_HEIGHT};
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
        SDL_RenderFillRect(renderer, &sliderBg);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &sliderBg);

        int sliderPos = SLIDER_X + (sliderValue * SLIDER_WIDTH) / SLIDER_MAX;
        SDL_Rect sliderHandle = {sliderPos - 5, SLIDER_Y - 5, 10, 30};
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &sliderHandle);

        // Vẽ nút Back
        SDL_Rect backButton = {SOUND_SETTING_X + 125, SOUND_SETTING_Y + 200, BUTTON_WIDTH, BUTTON_HEIGHT};
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(renderer, &backButton);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &backButton);
        SDL_Texture* backTextTexture = renderText("Back", white, 30);
        if (backTextTexture != nullptr) {
            int textW, textH;
            SDL_QueryTexture(backTextTexture, NULL, NULL, &textW, &textH);
            int textX = backButton.x + (backButton.w - textW) / 2;
            int textY = backButton.y + (backButton.h - textH) / 2;
            renderTexture(backTextTexture, textX, textY);
            SDL_DestroyTexture(backTextTexture);
        }

        SDL_RenderPresent(renderer);
    }

    void render(const SlidingPuzzle& game) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Texture* background = loadTexture(BACKGROUND_IMG);
        if (background != nullptr) {
            renderTexture(background, 0, 0);
            SDL_DestroyTexture(background);
        }

        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                int x = BOARD_X + j * CELL_SIZE;
                int y = BOARD_Y + i * CELL_SIZE;
                int value = game.board[i][j];
                if (value >= 0 && value < BOARD_SIZE * BOARD_SIZE && cellTextures[value] != nullptr) {
                    renderTexture(cellTextures[value], x, y);
                } else {
                    if (cellTextures[0] != nullptr) {
                        renderTexture(cellTextures[0], x, y);
                    }
                }
                // Vẽ số lên ô (trừ ô trống)
                if (value != EMPTY_CELL) {
                    char numberText[3];
                    sprintf(numberText, "%d", value);
                    SDL_Color white = {255, 255, 255, 255};
                    SDL_Texture* numberTexture = renderText(numberText, white, 40);
                    if (numberTexture != nullptr) {
                        int textW, textH;
                        SDL_QueryTexture(numberTexture, NULL, NULL, &textW, &textH);
                        int textX = x + (CELL_SIZE - textW) / 2;
                        int textY = y + (CELL_SIZE - textH) / 2;
                        renderTexture(numberTexture, textX, textY);
                        SDL_DestroyTexture(numberTexture);
                    }
                }
            }
        }

        SDL_Color white = {255, 255, 255, 255};
        if (!game.isSolved()) {
            // Hiển thị số bước di chuyển
            char moveText[50];
            sprintf(moveText, "Moves: %d", game.moveCount);
            SDL_Texture* textTexture = renderText(moveText, white);
            if (textTexture != nullptr) {
                renderTexture(textTexture, 10, 10);
                SDL_DestroyTexture(textTexture);
            }

            // Vẽ nút Give Up
            SDL_Rect giveUpButton = {SCREEN_WIDTH - 210, SCREEN_HEIGHT - 60, 200, 50};
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_RenderFillRect(renderer, &giveUpButton);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &giveUpButton);

            SDL_Texture* giveUpText = renderText("Give Up", white, 30);
            if (giveUpText != nullptr) {
                int textW, textH;
                SDL_QueryTexture(giveUpText, NULL, NULL, &textW, &textH);
                int textX = giveUpButton.x + (giveUpButton.w - textW) / 2;
                int textY = giveUpButton.y + (giveUpButton.h - textH) / 2;
                renderTexture(giveUpText, textX, textY);
                SDL_DestroyTexture(giveUpText);
            }
        } else {
            // Hiển thị thông báo khi game hoàn thành
            if (game.gaveUp) {
                SDL_Color red = {255, 0, 0, 255};
                SDL_Texture* loseText = renderText("You Lose!", red);
                if (loseText != nullptr) {
                    int loseTextW, loseTextH;
                    SDL_QueryTexture(loseText, NULL, NULL, &loseTextW, &loseTextH);
                    int loseX = SCREEN_WIDTH / 2 - loseTextW / 2;
                    int loseY = SCREEN_HEIGHT / 2 - loseTextH - 50;
                    renderTexture(loseText, loseX, loseY);
                    SDL_DestroyTexture(loseText);
                }
            } else {
                SDL_Color yellow = {255, 255, 0, 255};
                SDL_Texture* winText = renderText("You Win!", yellow);
                if (winText != nullptr) {
                    int winTextW, winTextH;
                    SDL_QueryTexture(winText, NULL, NULL, &winTextW, &winTextH);
                    int winX = SCREEN_WIDTH / 2 - winTextW / 2;
                    int winY = SCREEN_HEIGHT / 2 - winTextH - 50;
                    renderTexture(winText, winX, winY);
                    SDL_DestroyTexture(winText);
                }
            }

            // Hiển thị high score
            char highScoreText[50];
            if (game.highScore == -1) {
                sprintf(highScoreText, "Best: N/A");
            } else {
                sprintf(highScoreText, "Best: %d", game.highScore);
            }
            SDL_Texture* highScoreTexture = renderText(highScoreText, white, 30);
            if (highScoreTexture != nullptr) {
                int highScoreTextW, highScoreTextH;
                SDL_QueryTexture(highScoreTexture, NULL, NULL, &highScoreTextW, &highScoreTextH);
                int highScoreX = SCREEN_WIDTH / 2 - highScoreTextW / 2;
                int highScoreY = SCREEN_HEIGHT / 2;
                renderTexture(highScoreTexture, highScoreX, highScoreY);
                SDL_DestroyTexture(highScoreTexture);
            }

            // Vẽ nút Back
            SDL_Rect backButton = {(SCREEN_WIDTH - 200) / 2, SCREEN_HEIGHT / 2 + 80, 200, 50};
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_RenderFillRect(renderer, &backButton);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &backButton);

            SDL_Texture* backText = renderText("Back", white, 30);
            if (backText != nullptr) {
                int backTextW, backTextH;
                SDL_QueryTexture(backText, NULL, NULL, &backTextW, &backTextH);
                int backX = backButton.x + (backButton.w - backTextW) / 2;
                int backY = backButton.y + (backButton.h - backTextH) / 2;
                renderTexture(backText, backX, backY);
                SDL_DestroyTexture(backText);
            }
        }

        SDL_RenderPresent(renderer);
    }

    void quit() {
        if (backgroundMusic != nullptr) {
            Mix_FreeMusic(backgroundMusic);
        }
        Mix_CloseAudio();
        for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) {
            if (cellTextures[i] != nullptr) {
                SDL_DestroyTexture(cellTextures[i]);
                cellTextures[i] = nullptr;
            }
        }
        TTF_CloseFont(font);
        TTF_Quit();
        IMG_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

#endif
