#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

using namespace std;

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 700;
const char* WINDOW_TITLE = "GeoGuessr";

void logErrorAndExit(const char* msg, const char* error)
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "%s: %s", msg, error);
    SDL_Quit();
}

SDL_Window* initSDL(int SCREEN_WIDTH, int SCREEN_HEIGHT, const char* WINDOW_TITLE)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        logErrorAndExit("SDL_Init", SDL_GetError());

    SDL_Window* window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) logErrorAndExit("CreateWindow", SDL_GetError());

    if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))
        logErrorAndExit("SDL_image error:", IMG_GetError());

    return window;
}

SDL_Renderer* createRenderer(SDL_Window* window)
{
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) logErrorAndExit("CreateRenderer", SDL_GetError());

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    return renderer;
}

void quitSDL(SDL_Window* window, SDL_Renderer* renderer)
{
    IMG_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void renderTexture(SDL_Texture* texture, int x, int y, SDL_Renderer* renderer)
{
    SDL_Rect dest;

    dest.x = x;
    dest.y = y;
    SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

    SDL_RenderCopy(renderer, texture, NULL, &dest);
}

SDL_Texture* loadTexture(const char* filename, SDL_Renderer* renderer)
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);

    SDL_Texture* texture = IMG_LoadTexture(renderer, filename);
    if (texture == NULL)
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Load texture %s", IMG_GetError());

    return texture;
}

bool isInside(int x, int y, SDL_Rect rect)
{
    return x > rect.x && x < rect.x + rect.w && y > rect.y && y < rect.y + rect.h;
}

void mainLoop(SDL_Renderer* renderer, SDL_Texture* playTexture, SDL_Texture* exitTexture, SDL_Texture* logoTexture, SDL_Texture* backgroundTexture)
{
    SDL_Event e;
    bool quit = false;

    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);

                SDL_Rect playRect = { 150, 350, 0, 0 };
                SDL_QueryTexture(playTexture, NULL, NULL, &playRect.w, &playRect.h);

                SDL_Rect exitRect = { 150, 500, 0, 0 };
                SDL_QueryTexture(exitTexture, NULL, NULL, &exitRect.w, &exitRect.h);

                if (isInside(x, y, playRect))
                {
                    SDL_Log("Play button clicked!");

                    SDL_DestroyTexture(logoTexture);
                    SDL_DestroyTexture(playTexture);
                    SDL_DestroyTexture(exitTexture);
                    SDL_DestroyTexture(backgroundTexture);

                    SDL_Texture* background = loadTexture("Play-Background.png", renderer);
                    SDL_RenderCopy( renderer, background, NULL, NULL);

                    SDL_Texture* close = loadTexture("Back.png", renderer);
                    renderTexture(close, 1150, 5, renderer);

                    SDL_RenderPresent( renderer );
                }
                else if (isInside(x, y, exitRect))
                {
                    SDL_Log("Exit button clicked!");
                    quit = true;
                }
            }
        }
        SDL_Delay(100);
    }
}

int main(int argc, char* argv[])
{
    SDL_Window* window = initSDL(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SDL_Renderer* renderer = createRenderer(window);

    SDL_Texture* background = loadTexture("Background.webp", renderer);
    SDL_RenderCopy(renderer, background, NULL, NULL);

    SDL_RenderPresent(renderer);

    SDL_Texture* logo = loadTexture("Logo.png", renderer);
    renderTexture(logo, 240, 100, renderer);

    SDL_Texture* play = loadTexture("Play.png", renderer);
    renderTexture(play, 150, 350, renderer);

    SDL_Texture* exit = loadTexture("Exit.png", renderer);
    renderTexture(exit, 150, 500, renderer);

    SDL_RenderPresent(renderer);

    mainLoop(renderer, play, exit, logo, background);

    SDL_DestroyTexture(logo);
    SDL_DestroyTexture(play);
    SDL_DestroyTexture(exit);
    SDL_DestroyTexture(background);

    quitSDL(window, renderer);
    return 0;
}
