#include "Platform.hpp"

#include <stdexcept>
#include <string>

Platform::Platform(
    char const* title,
    int windowWidth,
    int windowHeight,
    int textureWidth,
    int textureHeight
)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
    }

    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_SHOWN
    );
    if (window == nullptr)
    {
        SDL_Quit();
        throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }
    if (renderer == nullptr)
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error(std::string("SDL_CreateRenderer failed: ") + SDL_GetError());
    }

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        textureWidth,
        textureHeight
    );
    if (texture == nullptr)
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error(std::string("SDL_CreateTexture failed: ") + SDL_GetError());
    }
}

Platform::~Platform()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Platform::Update(void const* buffer, int pitch)
{
    SDL_UpdateTexture(texture, nullptr, buffer, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

bool Platform::ProcessInput(uint8_t* keys)
{
    bool quit = false;
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            quit = true;
            break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
            bool pressed = event.type == SDL_KEYDOWN;

            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                quit = true;
                break;

            // CHIP-8 keypad:
            //
            // 1 2 3 C
            // 4 5 6 D
            // 7 8 9 E
            // A 0 B F
            //
            // Keyboard:
            //
            // 1 2 3 4
            // Q W E R
            // A S D F
            // Z X C V

            case SDLK_x:
                keys[0x0] = pressed;
                break;

            case SDLK_1:
                keys[0x1] = pressed;
                break;

            case SDLK_2:
                keys[0x2] = pressed;
                break;

            case SDLK_3:
                keys[0x3] = pressed;
                break;

            // 0x4: Q / A / Left Arrow
            case SDLK_q:
            case SDLK_a:
            case SDLK_LEFT:
                keys[0x4] = pressed;
                break;

            case SDLK_w:
                keys[0x5] = pressed;
                break;

            // 0x6: E / D / Right Arrow
            case SDLK_e:
            case SDLK_d:
            case SDLK_RIGHT:
                keys[0x6] = pressed;
                break;

            case SDLK_s:
                keys[0x8] = pressed;
                break;

            case SDLK_z:
                keys[0xA] = pressed;
                break;

            case SDLK_c:
                keys[0xB] = pressed;
                break;

            case SDLK_4:
                keys[0xC] = pressed;
                break;

            case SDLK_r:
                keys[0xD] = pressed;
                break;

            case SDLK_f:
                keys[0xE] = pressed;
                break;

            case SDLK_v:
                keys[0xF] = pressed;
                break;

            default:
                break;
            }
        }
        break;

        default:
            break;
        }
    }

    return quit;
}
