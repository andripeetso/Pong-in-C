#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int PADDLE_WIDTH = 20;
const int PADDLE_HEIGHT = 80;
const int BALL_SIZE = 15;
const int PADDLE_SPEED = 5;
const int MAX_SCORE = 5;

typedef enum {
    GAME_START_SCREEN,
    GAME_PLAYING,
    GAME_END_SCREEN
} GameState;

void render_text(SDL_Renderer *renderer, const char *text, int x, int y, TTF_Font *font) {
    SDL_Color color = {255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("Pong",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font *font = TTF_OpenFont("assets/fonts/PixeloidSansBold-GOjpP.ttf", 24);
    
    if (font == NULL) {
    printf("Error loading font: %s\n", TTF_GetError());
    return 1;
    }

    SDL_Rect left_paddle = {40, (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2, PADDLE_WIDTH, PADDLE_HEIGHT};
    SDL_Rect right_paddle = {SCREEN_WIDTH - 40 - PADDLE_WIDTH, (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2, PADDLE_WIDTH, PADDLE_HEIGHT};
    SDL_Rect ball = {(SCREEN_WIDTH - BALL_SIZE) / 2, (SCREEN_HEIGHT - BALL_SIZE) / 2, BALL_SIZE, BALL_SIZE};

    int ball_dx = 1, ball_dy = 1;
    int ball_speed = 1;
    int left_score = 0, right_score = 0;
    int timer = 0;

    GameState state = GAME_START_SCREEN;

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);

        if (state == GAME_START_SCREEN) {
            if (keyboardState[SDL_SCANCODE_RETURN]) {
                state = GAME_PLAYING;
            }
        } else if (state == GAME_PLAYING) {
            if (keyboardState[SDL_SCANCODE_W] && left_paddle.y > 0) {
                left_paddle.y -= PADDLE_SPEED;
            }
            if (keyboardState[SDL_SCANCODE_S] && left_paddle.y < SCREEN_HEIGHT - PADDLE_HEIGHT) {
                left_paddle.y += PADDLE_SPEED;
            }
            if (keyboardState[SDL_SCANCODE_UP] && right_paddle.y > 0) {
                right_paddle.y -= PADDLE_SPEED;
            }
            if (keyboardState[SDL_SCANCODE_DOWN] && right_paddle.y < SCREEN_HEIGHT - PADDLE_HEIGHT) {
                right_paddle.y += PADDLE_SPEED;
            }

            ball.x += ball_dx * ball_speed;
            ball.y += ball_dy * ball_speed;

            if (ball.y <= 0 || ball.y + BALL_SIZE >= SCREEN_HEIGHT) {
                ball_dy = -ball_dy;
            }

            if (SDL_HasIntersection(&ball, &left_paddle) || SDL_HasIntersection(&ball, &right_paddle)) {
                ball_dx = -ball_dx;
                ball_speed++;
            }

            if (ball.x <= 0) {
                right_score++;
                ball.x = (SCREEN_WIDTH - BALL_SIZE) / 2;
                ball.y = (SCREEN_HEIGHT - BALL_SIZE) / 2;
                ball_speed = 1;
            } else if (ball.x + BALL_SIZE >= SCREEN_WIDTH) {
                left_score++;
                ball.x = (SCREEN_WIDTH - BALL_SIZE) / 2;
                ball.y = (SCREEN_HEIGHT - BALL_SIZE) / 2;
                ball_speed = 1;
            }

            if (left_score >= MAX_SCORE || right_score >= MAX_SCORE) {
                state = GAME_END_SCREEN;
            }

            timer++;
        } else if (state == GAME_END_SCREEN) {
            if (keyboardState[SDL_SCANCODE_RETURN]) {
                left_score = 0;
                right_score = 0;
                state = GAME_START_SCREEN;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (state == GAME_START_SCREEN) {
            render_text(renderer, "ANDRI'S PONG", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, font);
            render_text(renderer, "Press Enter to start", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, font);
        } else if (state == GAME_PLAYING) {
            char timer_text[10];
            sprintf(timer_text, "%d", timer / 60);
            render_text(renderer, timer_text, SCREEN_WIDTH / 2 - 10, 10, font);

            char score_text[20];
            sprintf(score_text, "%d - %d", left_score, right_score);
            render_text(renderer, score_text, SCREEN_WIDTH / 2 - 30, 40, font);

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &left_paddle);
            SDL_RenderFillRect(renderer, &right_paddle);
            SDL_RenderFillRect(renderer, &ball);
        } else if (state == GAME_END_SCREEN) {
            char winner_text[20];
            if (left_score >= MAX_SCORE) {
                sprintf(winner_text, "Player 1 wins");
            } else {
                sprintf(winner_text, "Player 2 wins");
            }
            render_text(renderer, winner_text, SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50, font);
            render_text(renderer, "Press Enter for a new game", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, font);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(1000 / 60);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}