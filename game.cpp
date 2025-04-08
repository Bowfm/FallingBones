#include "game.h"
#include <cstdlib>   // For rand()
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
                isLeftKeyDown = false;
                isRightKeyDown = false;
                basketVelX = 0;
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
                 isLeftKeyDown = false;
                 isRightKeyDown = false;
                 basketVelX = 0;
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
                isLeftKeyDown = false;
                isRightKeyDown = false;
                basketVelX = 0;
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
                     isLeftKeyDown = false;
                     isRightKeyDown = false;
                     basketVelX = 0;
                 } else if (currentState == GameState::PAUSED && gamePausedConfirm) {
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
    if (currentState != GameState::PLAYING) {
        // Đảm bảo vận tốc bằng 0 nếu không chơi
        // basketVelX = 0; // Không cần thiết nếu đã reset khi pause/menu
        return;
    }

    // 1. Tính toán lại vận tốc dựa trên trạng thái phím hiện tại
    basketVelX = 0; // Reset vận tốc mỗi frame
    if (isLeftKeyDown) {
        basketVelX -= BASKET_SPEED;
    }
    if (isRightKeyDown) {
        basketVelX += BASKET_SPEED;
    }
    // Giờ basketVelX sẽ là -SPEED, 0, hoặc +SPEED tùy thuộc phím nào đang được nhấn

    // 2. Cập nhật vị trí Basket (sử dụng vận tốc vừa tính)
    basketRect.x += basketVelX;
    // Giữ basket trong màn hình
    if (basketRect.x < 0) {
        basketRect.x = 0;
    } else if (basketRect.x > SCREEN_WIDTH - basketRect.w) {
        basketRect.x = SCREEN_WIDTH - basketRect.w;
    }

    // 3. Spawn Items (Giữ nguyên logic cũ)
    if (rand() % ITEM_SPAWN_RATE == 0) {
       // ... (code spawn item) ...
       Item newItem;
       newItem.rect.w = ITEM_WIDTH;
       newItem.rect.h = ITEM_HEIGHT;
       newItem.rect.x = rand() % (SCREEN_WIDTH - newItem.rect.w); // Random X pos within screen bounds
       newItem.rect.y = -newItem.rect.h; // Start just above the screen
       newItem.active = true;
       items.push_back(newItem);
    }


    // 4. Update Items (Giữ nguyên logic cũ)
    for (auto it = items.begin(); it != items.end(); /* no increment here */) {
         // ... (code di chuyển, kiểm tra va chạm, xóa item) ...
          if (it->active) {
            // Move item down
            it->rect.y += ITEM_SPEED;

            // Check for collision (catch) with basket
            if (SDL_HasIntersection(&it->rect, &basketRect)) {
                score++;
                graphics.updateScoreTexture(this->score); // Update score display
                audio.play(audio.catchSound);                   // Play catch sound
                it->active = false;                       // Deactivate caught item
            }
            // Check for miss (item passed below the screen)
            else if (it->rect.y > SCREEN_HEIGHT) {
                lives--;
                it->active = false; // Deactivate missed item
                cout << "Item missed. Lives left: " << lives << endl;
                // Check for game over immediately after losing a life
                if (lives <= 0) {
                    currentState = GameState::GAME_OVER;
                    audio.haltMusic();        // Stop background music
                    audio.play(audio.gameOverSound); // Play game over sound
                    cout << "GAME OVER!" << endl;
                    // Reset trạng thái di chuyển khi Game Over
                    isLeftKeyDown = false;
                    isRightKeyDown = false;
                    basketVelX = 0;
                    return; // Exit update function early
                }
            }
        }

        // Remove inactive items from the vector
        if (!it->active) {
            it = items.erase(it); // erase returns iterator to the next element
        } else {
            ++it; // Only increment if the item was not erased
        }
    }
}

void Game::render(Graphics& graphics) {
    // Rendering logic depends heavily on the current game state

    if (currentState == GameState::MENU) {
        // Render Title (if loaded)
        if (graphics.titleTextTexture) {
            // Center the title above the play button
            int titleW, titleH;
            SDL_QueryTexture(graphics.titleTextTexture, nullptr, nullptr, &titleW, &titleH);
            int titleX = (SCREEN_WIDTH - titleW) / 2;
             // Adjust Y position relative to play button or screen height
            int titleY = playButtonRect.y - titleH - 30; // 30 pixels above play button
            if (titleY < 20) titleY = 20; // Ensure it doesn't go off screen top
            graphics.renderTexture(graphics.titleTextTexture, titleX, titleY);
        } else {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Warning: Title texture not available for rendering.");
        }

        // Render Play button
        graphics.renderTexture(graphics.playButtonTexture, nullptr, &playButtonRect);

    } else if (currentState == GameState::PLAYING || currentState == GameState::PAUSED) {
        // Render common elements for Playing and Paused states

        // Render Basket
        graphics.renderTexture(graphics.basketTexture, nullptr, &basketRect);

        // Render Active Items
        for (const auto& item : items) {
            if (item.active) {
                graphics.renderTexture(graphics.itemTexture, nullptr, &item.rect);
            }
        }

        // Render Score
        if (graphics.scoreTextTexture) {
            // Position score at top-left
            graphics.renderTexture(graphics.scoreTextTexture, 10, 10);
        }

        // Render Hearts (Lives)
        int heartStartX = 10;
        int heartStartY = 50; // Below score
        for (int i = 0; i < lives; ++i) {
             SDL_Rect heartDestRect = {heartStartX + i * (HEART_SIZE + 5), heartStartY, HEART_SIZE, HEART_SIZE};
             graphics.renderTexture(graphics.heartTexture, nullptr, &heartDestRect);
        }

        // Render Pause Button (always visible in playing/paused state)
        graphics.renderTexture(graphics.pauseButtonTexture, nullptr, &pauseButtonRect);

        // --- Specific rendering for PAUSED state ---
        if (currentState == GameState::PAUSED && gamePausedConfirm) {
            // 1. Darken background slightly for focus
            graphics.setDrawBlendMode(SDL_BLENDMODE_BLEND); // Enable blending
            SDL_Color darkColor = {0, 0, 0, 150}; // Semi-transparent black
            SDL_Rect screenRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            graphics.renderFilledRect(&screenRect, darkColor);
            graphics.setDrawBlendMode(SDL_BLENDMODE_NONE); // Disable blending for subsequent renders

            // 2. Render "Continue?" text (ensure texture exists)
             graphics.createPauseTextTexture(); // Create if not already done
             if (graphics.pauseTextTexture) {
                 int textW, textH;
                 SDL_QueryTexture(graphics.pauseTextTexture, nullptr, nullptr, &textW, &textH);
                 int textX = (SCREEN_WIDTH - textW) / 2;
                 // Position above the Yes/No buttons
                 int textY = yesButtonRect.y - textH - 30;
                 graphics.renderTexture(graphics.pauseTextTexture, textX, textY);
             }

            // 3. Render Yes/No buttons
            graphics.renderTexture(graphics.yesButtonTexture, nullptr, &yesButtonRect);
            graphics.renderTexture(graphics.noButtonTexture, nullptr, &noButtonRect);
        }

    } else if (currentState == GameState::GAME_OVER) {
        // Render Game Over Image (centered)
        if (graphics.gameOverTexture) {
            int imgW, imgH;
            SDL_QueryTexture(graphics.gameOverTexture, nullptr, nullptr, &imgW, &imgH);
             // Optionally scale it if it's too large/small
            imgW = 600; imgH = 300; // Example fixed size
            SDL_Rect gameOverDestRect;
            gameOverDestRect.w = imgW;
            gameOverDestRect.h = imgH;
            gameOverDestRect.x = (SCREEN_WIDTH - imgW) / 2;
            // Position it above the final score and buttons
            gameOverDestRect.y = SCREEN_HEIGHT / 2 - imgH + 100 ; // Adjust vertical position as needed
            graphics.renderTexture(graphics.gameOverTexture, nullptr, &gameOverDestRect);
        }


        // Render Final Score (centered below Game Over image)
        if (graphics.scoreTextTexture) {
            int textW, textH;
            SDL_QueryTexture(graphics.scoreTextTexture, nullptr, nullptr, &textW, &textH);
            int scoreX = (SCREEN_WIDTH - textW) / 2;
             // Position below game over image, above buttons
            int scoreY = restartButtonRect.y - textH - 20;
            graphics.renderTexture(graphics.scoreTextTexture, scoreX, scoreY);
        }

         // Render Restart and Home Buttons
         graphics.renderTexture(graphics.restartButtonTexture, nullptr, &restartButtonRect);
         graphics.renderTexture(graphics.homeButtonTexture, nullptr, &homeButtonRect);
    }
}
