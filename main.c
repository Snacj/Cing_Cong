#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <math.h>
#include <stdio.h>

#define WIDTH 1200
#define HEIGHT 600
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000

struct Circle {
    double x;
    double y;
    double radius;
};

void drawCircle(SDL_Surface* surface, struct Circle circle, Uint32 color) {
    double radius_squared = pow(circle.radius, 2);
    for (double x = circle.x - circle.radius; x<=circle.x + circle.radius; x++) {
        for (double y = circle.y - circle.radius; y<=circle.y + circle.radius; y++) {
            double distance_squared = pow(x - circle.x, 2) + pow(y - circle.y, 2);
            if ( distance_squared < radius_squared ) {
                SDL_Rect pixel = (SDL_Rect) {x,y,1,1};
                SDL_FillRect(surface, &pixel, color);
            }
        }
    }
}

void drawRect(SDL_Surface* surface, int x, int y, Uint32 color) {
    SDL_Rect rect = {x,y,20,HEIGHT - 20};
    SDL_FillRect(surface, &rect, color);
}

int detectCollision(SDL_Surface* surface, struct Circle circle) {

    // left collision
    if (circle.y - circle.radius <= 0 ) return 1;
    // right collision
    if (circle.y + circle.radius >= HEIGHT ) return 2;
    // top collision
    if (circle.x - circle.radius <= 0 ) return 3;
    // bottom collision
    if (circle.x + circle.radius >= WIDTH ) return 4;
    
    return -1;
}

int detectPing(struct Circle circle, SDL_Rect rect, int* xVel) {

    int horizontal_overlap = (circle.x + circle.radius > rect.x) && (circle.x - circle.radius < rect.x + rect.w);
    int vertical_overlap = (circle.y + circle.radius > rect.y) && (circle.y - circle.radius < rect.y + rect.h);

    if (horizontal_overlap && vertical_overlap) {
        *xVel = -*xVel;  // Reverse velocity
    }
    
    return -1;
}

void moveRect(SDL_Rect* rect, int y) {
    rect->y += y;
}

int main (int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Bouncy Ball", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

    SDL_Surface* surface = SDL_GetWindowSurface(window);

    struct Circle circle = {HEIGHT/2,200,40};
    int x_speed = 6;
    int y_speed = 2;

    SDL_Rect left_rect = {10,HEIGHT/4,20,HEIGHT/2};
    SDL_Rect right_rect = {WIDTH-30,HEIGHT/4,20,HEIGHT/2};

    SDL_Rect erase_rect = {0,0,WIDTH,HEIGHT};

    int upPressed = 0;
    int downPressed = 0;
    int wPressed = 0;
    int sPressed = 0;

    int game_running = 1;
    int game_over = 0;
    SDL_Event event;
    while (game_running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                game_running = 0;
            } 
        }

        if (!game_over) {
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        upPressed = 1;
                        break;
                    case SDLK_DOWN:
                        downPressed = 1;
                        break;
                    case SDLK_w:
                        wPressed = 1;
                        break;
                    case SDLK_s:
                        sPressed = 1;
                        break;
                }
            } else if (event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        upPressed = 0;
                        break;
                    case SDLK_DOWN:
                        downPressed = 0;
                        break;
                    case SDLK_w:
                        wPressed = 0;
                        break;
                    case SDLK_s:
                        sPressed = 0;
                        break;
                }
            }

            if (upPressed) {
                moveRect(&left_rect, -5);
                moveRect(&right_rect, -5);
            }
            if (downPressed) {
                moveRect(&left_rect, 5);
                moveRect(&right_rect, 5);
            }
            if (wPressed) {
                moveRect(&left_rect, -5);
                moveRect(&right_rect, -5);
            }
            if (sPressed) {
                moveRect(&left_rect, 5);
                moveRect(&right_rect, 5);
            }


            
            circle.x += x_speed;
            circle.y += y_speed;

            if (detectPing(circle, left_rect, &x_speed) == 1) {
                x_speed = -x_speed;
                printf("Ping left\n");
            }
            if (detectPing(circle, right_rect, &x_speed) == 2) {
                x_speed = -x_speed;
                printf("Ping right\n");
            }

            if (detectCollision(surface, circle) == 1) y_speed = -y_speed;
            if (detectCollision(surface, circle) == 2) y_speed = -y_speed;

            if (detectCollision(surface, circle) == 3)game_over = 1;
            if (detectCollision(surface, circle) == 4)game_over = 1;

            SDL_FillRect(surface, &erase_rect, COLOR_BLACK);
            SDL_FillRect(surface, &left_rect, COLOR_WHITE);
            SDL_FillRect(surface, &right_rect, COLOR_WHITE);
            drawCircle(surface, circle, COLOR_WHITE);
        } else {
            SDL_FillRect(surface, &erase_rect, COLOR_BLACK);
        }
        SDL_UpdateWindowSurface(window);

        SDL_Delay(10);
    }

    return 0;
}
