#include <SDL.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "defs.h"
#include "structs.h"
#include "graphics.h"
#include "audio.h"
#include "game.h"

int main(int argc, char* argv[])
{
    srand(time(0));

    Graphics graphics;
    Audio audio;
    Game game;

    graphics.init();
    audio.init();
    // 1. Khởi tạo các hệ thống con
//    if (!graphics.init() || !audio.init()) {
//        std::cerr << "ERROR: Failed to initialize Graphics or Audio!" << std::endl;
//        // Gọi destroy để dọn dẹp những gì đã init được (an toàn)
//        graphics.destroy();
//        audio.destroy();
//        SDL_Quit(); // Vẫn cần quit SDL chính
//        return 1; // Thoát với mã lỗi
//    }

    graphics.loadGameTextures();
    audio.loadSounds();
    // 2. Tải tài nguyên
//    if (!graphics.loadGameTextures() || !audio.loadSounds()) {
//        std::cerr << "ERROR: Failed to load textures or sounds!" << std::endl;
//        graphics.destroy();
//        audio.destroy();
//        SDL_Quit();
//        return 1;
//    }
    graphics.updateScoreTexture(game.score);
     // Cập nhật texture điểm ban đầu sau khi font đã load
//    if (!graphics.updateScoreTexture(game.score)) { // Truy cập score ban đầu từ game
//         std::cerr << "ERROR: Failed to create initial score texture!" << std::endl;
//         graphics.destroy();
//         audio.destroy();
//         SDL_Quit();
//         return 1;
//    }


    // 3. Khởi tạo trạng thái game
    game.init(); // Thiết lập vị trí ban đầu của các đối tượng game

    // 4. Vòng lặp chính
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        // Xử lý Input (trong game object)
        while (SDL_PollEvent(&e) != 0) {
             // Kiểm tra sự kiện thoát cơ bản ở đây
             if (e.type == SDL_QUIT) {
                 quit = true;
             }
             // Giao phần còn lại cho game object xử lý
             game.handleInput(e, quit, graphics, audio);
        }

        // Cập nhật Game Logic (trong game object)
        game.update(graphics, audio);

        // Render (dùng graphics object)
        graphics.clearScreen();

        // Vẽ background (có thể chuyển vào game.render nếu muốn)
        graphics.renderTexture(graphics.backgroundTexture, nullptr, nullptr);

        // Yêu cầu game object tự vẽ các thành phần của nó
        game.render(graphics);

        graphics.presentScene();

        // Giới hạn FPS (~60)
        SDL_Delay(16);
    }
    graphics.destroy();
    audio.destroy();
    SDL_Quit();

    return 0;
}
