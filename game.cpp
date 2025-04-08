#include "game.h"
#include <cstdlib>   // rand()
#include <string>
#include <iostream>

void Game::init() {
    // Basket
    basketRect = { (SCREEN_WIDTH - BASKET_WIDTH) / 2, SCREEN_HEIGHT - BASKET_HEIGHT - 40, BASKET_WIDTH, BASKET_HEIGHT };
    basketVelX = 0;

    // Buttons
    playButtonRect = { (SCREEN_WIDTH - BUTTON_WIDTH) / 2, (SCREEN_HEIGHT - BUTTON_HEIGHT) / 2, BUTTON_WIDTH, BUTTON_HEIGHT };
    pauseButtonRect = { SCREEN_WIDTH - 60, 10, 50, 50 };

    int confirmButtonY = SCREEN_HEIGHT / 2;
    int confirmButtonSpacing = 20; // Distance between YES and NO buttons
    yesButtonRect = { SCREEN_WIDTH / 2 - BUTTON_WIDTH - confirmButtonSpacing / 2, confirmButtonY, BUTTON_WIDTH, BUTTON_HEIGHT };
	noButtonRect = { SCREEN_WIDTH / 2 + confirmButtonSpacing / 2, confirmButtonY, BUTTON_WIDTH, BUTTON_HEIGHT };

    int gameOverButtonY = SCREEN_HEIGHT / 2 + 80;
    int gameOverButtonSpacing = 20; // Distance between Restart and Home buttons
    restartButtonRect = { SCREEN_WIDTH / 2 - BUTTON_WIDTH - gameOverButtonSpacing / 2, gameOverButtonY, BUTTON_WIDTH, BUTTON_HEIGHT };
	homeButtonRect = { SCREEN_WIDTH / 2 + gameOverButtonSpacing / 2, gameOverButtonY, BUTTON_WIDTH, BUTTON_HEIGHT };

    cout << "Game initialized." << endl;
}

void Game::reset(Audio& audio) {
    score = 0;
    lives = 3;
    basketRect.x = (SCREEN_WIDTH - BASKET_WIDTH) / 2;
    basketVelX = 0;
    isLeftKeyDown = false;
    isRightKeyDown = false;
    items.clear();
    currentState = GameState::PLAYING;
    gamePausedConfirm = false;

    // Start background music
    if (!audio.isMusicPlaying()) {
         audio.play(audio.bgMusic);
    } else {
        audio.resumeMusic();
    }
}

void Game::handleInput(SDL_Event& e, bool& quit, Graphics& graphics, Audio& audio) {
    // Mouse
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        SDL_Point mousePoint = {mouseX, mouseY};
        // Menu
        if (currentState == GameState::MENU) {
            if (SDL_PointInRect(&mousePoint, &playButtonRect)) {
                reset(audio);
                graphics.updateScoreTexture(score);
            }
        }
        // Playing
        else if (currentState == GameState::PLAYING) {
             if (SDL_PointInRect(&mousePoint, &pauseButtonRect)) {
                currentState = GameState::PAUSED;
                gamePausedConfirm = true;
                audio.pauseMusic();
                stopBasketMovement();
             }
        }
        // Paused
        else if (currentState == GameState::PAUSED) {
             if (SDL_PointInRect(&mousePoint, &yesButtonRect)) {
                currentState = GameState::PLAYING;
                gamePausedConfirm = false;
                audio.resumeMusic();
             } else if (SDL_PointInRect(&mousePoint, &noButtonRect)) {
                 currentState = GameState::MENU;
                 gamePausedConfirm = false;
                 audio.haltMusic();
                 items.clear();
                 stopBasketMovement();
             }
        }
        // Game Over
        else if (currentState == GameState::GAME_OVER) {
             if (SDL_PointInRect(&mousePoint, &restartButtonRect)) {
                audio.haltSoundEffects();
                reset(audio);
                graphics.updateScoreTexture(score);
             } else if (SDL_PointInRect(&mousePoint, &homeButtonRect)) {
                audio.haltSoundEffects();
                currentState = GameState::MENU;
                items.clear();
                stopBasketMovement();
             }
        }
    }

    // Keyboard
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
            case SDLK_LEFT:  isLeftKeyDown = true; break;
            case SDLK_RIGHT: isRightKeyDown = true; break;
            case SDLK_p:
                 if (currentState == GameState::PLAYING) {
                     currentState = GameState::PAUSED;
                     gamePausedConfirm = true;
                     audio.pauseMusic();
                     stopBasketMovement();
                 } else if (currentState == GameState::PAUSED) {
                      currentState = GameState::PLAYING;
                      gamePausedConfirm = false;
                      audio.resumeMusic();
                 }
                 break;
        }
    } else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
            case SDLK_LEFT:  isLeftKeyDown = false; break;
            case SDLK_RIGHT: isRightKeyDown = false; break;
        }
    }
}

void Game::update(Graphics& graphics, Audio& audio) {
    // No need to update if not playing
    if (currentState != GameState::PLAYING) {
        return;
    }

    basketVelX = 0; // Reset velocity every frame
    if (isLeftKeyDown) {
        basketVelX -= BASKET_SPEED;
    }
    if (isRightKeyDown) {
        basketVelX += BASKET_SPEED;
    }

    // Update basket position
    basketRect.x += basketVelX;
    // Ensure that basket is in screen
    if (basketRect.x < 0) {
        basketRect.x = 0;
    } else if (basketRect.x > SCREEN_WIDTH - basketRect.w) {
        basketRect.x = SCREEN_WIDTH - basketRect.w;
    }

    // Spawn items
    if (rand() % ITEM_SPAWN_RATE == 0) {
       Item newItem;
       newItem.rect.w = ITEM_WIDTH;
       newItem.rect.h = ITEM_HEIGHT;
       newItem.rect.x = rand() % (SCREEN_WIDTH - newItem.rect.w); // Random X position
       newItem.rect.y = -newItem.rect.h;
       newItem.active = true;
       items.push_back(newItem);
    }

    // Update items
    for (auto it = items.begin(); it != items.end();) {
        if (it->active) {
            // Move item down
            it->rect.y += ITEM_SPEED;
            // Check for collision
            if (SDL_HasIntersection(&it->rect, &basketRect)) {
                score++;
                graphics.updateScoreTexture(score);
                audio.play(audio.catchSound);
                it->active = false;
            }
            // Check for miss
            else if (it->rect.y > SCREEN_HEIGHT) {
                lives--;
                it->active = false;
                cout << "Item missed. Lives left: " << lives << endl;
                // Check for game over
                if (lives <= 0) {
                    currentState = GameState::GAME_OVER;
                    audio.haltMusic();
                    audio.play(audio.gameOverSound);
                    cout << "GAME OVER!" << endl;
                    stopBasketMovement();
                    return;
                }
            }
        }

        // Remove inactive items from vector
        if (!it->active) {
            it = items.erase(it);
        } else {
            ++it; // Increment if item was not erased
        }
    }
}

void Game::render(Graphics& graphics) {
    graphics.renderTexture(graphics.backgroundTexture, nullptr, nullptr);
    // MENU
    if (currentState == GameState::MENU) {
        // Title
        if (graphics.titleTextTexture) {
            int titleW, titleH;
            SDL_QueryTexture(graphics.titleTextTexture, nullptr, nullptr, &titleW, &titleH);
            int titleX = (SCREEN_WIDTH - titleW) / 2; // Middle
            int titleY = playButtonRect.y - titleH - 30; // Above play button
            graphics.renderTexture(graphics.titleTextTexture, titleX, titleY);
        } else {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Title texture not available for rendering.");
        }

        // Play button
        graphics.renderTexture(graphics.playButtonTexture, nullptr, &playButtonRect);

    } else if (currentState == GameState::PLAYING || currentState == GameState::PAUSED) {
        // Basket
        graphics.renderTexture(graphics.basketTexture, nullptr, &basketRect);

        // Active Items
        for (const auto& item : items) {
            if (item.active) {
                graphics.renderTexture(graphics.itemTexture, nullptr, &item.rect);
            }
        }

        // Score
        if (graphics.scoreTextTexture) {
            graphics.renderTexture(graphics.scoreTextTexture, 10, 10);
        }

        // Lives
        int heartX = 10;
        int heartY = 50;
        for (int i = 0; i < lives; ++i) {
             SDL_Rect heartDestRect = {heartX + i * HEART_SIZE, heartY, HEART_SIZE, HEART_SIZE};
             graphics.renderTexture(graphics.heartTexture, nullptr, &heartDestRect);
        }

        // Pause Button
        graphics.renderTexture(graphics.pauseButtonTexture, nullptr, &pauseButtonRect);

        // PAUSED
        if (currentState == GameState::PAUSED) {
            // Darken background
            SDL_Color darkColor = {0, 0, 0, 150}; // Semi-transparent black
            graphics.setDrawBlendMode(SDL_BLENDMODE_BLEND); // Enable blending
            SDL_Rect screenRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            graphics.renderFilledRect(&screenRect, darkColor);
            graphics.setDrawBlendMode(SDL_BLENDMODE_NONE); // Disable blending

            // "Continue" text
             graphics.createPauseTextTexture();
             if (graphics.pauseTextTexture) {
                 int textW, textH;
                 SDL_QueryTexture(graphics.pauseTextTexture, nullptr, nullptr, &textW, &textH);
                 int textX = (SCREEN_WIDTH - textW) / 2;
                 int textY = yesButtonRect.y - textH - 30; // Above YES/NO buttons
                 graphics.renderTexture(graphics.pauseTextTexture, textX, textY);
             }

            // YES/NO buttons
            graphics.renderTexture(graphics.yesButtonTexture, nullptr, &yesButtonRect);
            graphics.renderTexture(graphics.noButtonTexture, nullptr, &noButtonRect);
        }

    } else if (currentState == GameState::GAME_OVER) {
        // Game Over
        if (graphics.gameOverTexture) {
            int imgW, imgH;
            imgW = 600; imgH = 300; // Scalable
            SDL_Rect gameOverDestRect;
            gameOverDestRect.w = imgW;
            gameOverDestRect.h = imgH;
            gameOverDestRect.x = (SCREEN_WIDTH - imgW) / 2;
            gameOverDestRect.y = SCREEN_HEIGHT / 2 - imgH + 100; // Adjustable
            graphics.renderTexture(graphics.gameOverTexture, nullptr, &gameOverDestRect);
        }

        // Final Score
        if (graphics.scoreTextTexture) {
            int textW, textH;
            SDL_QueryTexture(graphics.scoreTextTexture, nullptr, nullptr, &textW, &textH);
            int scoreX = (SCREEN_WIDTH - textW) / 2;
            int scoreY = restartButtonRect.y - textH - 20;
            graphics.renderTexture(graphics.scoreTextTexture, scoreX, scoreY);
        }

         // Restart and Home buttons
         graphics.renderTexture(graphics.restartButtonTexture, nullptr, &restartButtonRect);
         graphics.renderTexture(graphics.homeButtonTexture, nullptr, &homeButtonRect);
    }
}

void Game::stopBasketMovement()
{
    isLeftKeyDown = false;
    isRightKeyDown = false;
    basketVelX = 0;
}
