#ifndef _LOGIC__H
#define _LOGIC__H

#include <cstdlib>
#include <ctime>
#include <fstream>
#include "defs.h"

struct SlidingPuzzle {
    int board[BOARD_SIZE][BOARD_SIZE];
    int emptyRow, emptyCol;
    int moveCount;
    int highScore;
    bool gaveUp;

    SlidingPuzzle() : moveCount(0), highScore(-1), gaveUp(false) {
        loadHighScore();
        init();
    }

    void loadHighScore() {
        std::ifstream inFile("highscore.txt");
        if (inFile.is_open()) {
            inFile >> highScore;
            inFile.close();
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loaded highScore: %d", highScore);
        } else {
            highScore = -1;
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "No highScore file found, set to -1");
        }
    }

    void saveHighScore() {
        std::ofstream outFile("highscore.txt");
        if (outFile.is_open()) {
            outFile << highScore;
            outFile.close();
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Saved highScore: %d", highScore);
        } else {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Failed to save highScore to file");
        }
    }

    int getInversions() {
        int inversions = 0;
        int flat[BOARD_SIZE * BOARD_SIZE];
        int k = 0;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                flat[k++] = board[i][j];
            }
        }
        for (int i = 0; i < BOARD_SIZE * BOARD_SIZE - 1; i++) {
            for (int j = i + 1; j < BOARD_SIZE * BOARD_SIZE; j++) {
                if (flat[i] != EMPTY_CELL && flat[j] != EMPTY_CELL && flat[i] > flat[j]) {
                    inversions++;
                }
            }
        }
        return inversions;
    }

    bool isSolvable() {
        int inversions = getInversions();
        return (inversions % 2 == 0);
    }

    void init() {
        moveCount = 0;
        gaveUp = false;
        loadHighScore();

        int values[BOARD_SIZE * BOARD_SIZE];
        for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) {
            values[i] = i;
        }

        srand(time(0));
        do {
            for (int i = BOARD_SIZE * BOARD_SIZE - 1; i > 0; i--) {
                int j = rand() % (i + 1);
                int temp = values[i];
                values[i] = values[j];
                values[j] = temp;
            }

            int index = 0;
            for (int i = 0; i < BOARD_SIZE; i++) {
                for (int j = 0; j < BOARD_SIZE; j++) {
                    board[i][j] = values[index];
                    if (board[i][j] == EMPTY_CELL) {
                        emptyRow = i;
                        emptyCol = j;
                    }
                    index++;
                }
            }
        } while (!isSolvable());

        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Board initialized successfully.");
    }

    void move(int row, int col) {
        if (abs(row - emptyRow) + abs(col - emptyCol) == 1) {
            board[emptyRow][emptyCol] = board[row][col];
            board[row][col] = EMPTY_CELL;
            emptyRow = row;
            emptyCol = col;
            moveCount++;
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Move made, moveCount: %d", moveCount);
        }
    }

    void giveUp() {
        int expected[BOARD_SIZE][BOARD_SIZE] = {
            {1, 2, 3},
            {4, 5, 6},
            {7, 8, 0}
        };
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                board[i][j] = expected[i][j];
                if (board[i][j] == EMPTY_CELL) {
                    emptyRow = i;
                    emptyCol = j;
                }
            }
        }
        gaveUp = true;
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Gave up, board set to solved state");
    }

    void updateHighScore() {
        if (isSolved() && !gaveUp) {
            if (highScore == -1 || moveCount < highScore) {
                highScore = moveCount;
                saveHighScore();
                SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "High score updated: %d", highScore);
            }
        }
    }

    bool isSolved() const {
        int expected[BOARD_SIZE][BOARD_SIZE] = {
            {1, 2, 3},
            {4, 5, 6},
            {7, 8, 0}
        };
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (board[i][j] != expected[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    void resetMoves() {
        moveCount = 0;
        gaveUp = false;
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Move count and gaveUp reset");
    }
};

#endif
