//
//  snake.c
//  snake
//
//  Created by Hugh Drummond on 6/4/2024.
//

#include "snake.h"

#define WINDOW_X SDL_WINDOWPOS_CENTERED
#define WINDOW_Y SDL_WINDOWPOS_CENTERED
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000

#define GRID_SIZE 30
#define CELL_SIZE 30
#define OFFSET_X (WINDOW_WIDTH - (GRID_SIZE * CELL_SIZE)) / 2
#define OFFSET_Y (WINDOW_HEIGHT - (GRID_SIZE * CELL_SIZE)) / 2

struct cell {
    int x;
    int y;
    struct cell *next;
    struct cell *prev;
};
typedef struct cell cell_t;

struct snake {
    cell_t *head;
    cell_t *tail;
    unsigned int length;
};
typedef struct snake snake_t;

struct apple {
    int x;
    int y;
};
typedef struct apple apple_t;

enum direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};
typedef enum direction direction_t;

snake_t *init_snake(int length, direction_t *direction) {
    *direction = (direction_t)(rand() % 4);
    snake_t *snake = malloc(sizeof(snake_t));
    snake->length = length;
    
    snake->head = malloc(sizeof(cell_t));
    snake->head->x = rand() % (GRID_SIZE / 2) + (GRID_SIZE / 4);
    snake->head->y = rand() % (GRID_SIZE / 2) + (GRID_SIZE / 4);
    snake->head->next = NULL;
    snake->head->prev = NULL;
    snake->tail = snake->head;
    cell_t *curr = snake->head;
    for (int i = 1; i < length; i++) {
        curr->next = malloc(sizeof(cell_t));
        curr->next->prev = curr;
        curr = curr->next;
        curr->next = NULL;
        curr->x = curr->prev->x;
        curr->y = curr->prev->y;
        switch (*direction) {
            case UP:
                curr->y++;
                break;
            case DOWN:
                curr->y--;
                break;
            case LEFT:
                curr->x++;
                break;
            case RIGHT:
                curr->x--;
                break;
        }
        
    }
    snake->tail = curr;
    return snake;
}

void free_snake(snake_t *snake) {
    cell_t *current = snake->head;
    cell_t *next;
    while (current) {
        next = current->next;
        free(current);
        current = next;
    }
    free(snake);
    return;
}

apple_t *init_apple(void) {
    apple_t *apple = malloc(sizeof(apple_t));
    apple->x = rand() % GRID_SIZE;
    apple->y = rand() % GRID_SIZE;
    return apple;
}

bool valid_position(snake_t *snake, int x, int y) {
    cell_t *curr = snake->head;
    while (curr) {
        if (curr->x == x && curr->y == y) {
            return false;
        }
        curr = curr->next;
    }
    return true;
}

void generate_apple(snake_t *snake, apple_t *apple) {
    int x, y;
    do {
        x = rand() % GRID_SIZE;
        y = rand() % GRID_SIZE;
    } while (!valid_position(snake, x, y));
    apple->x = x;
    apple->y = y;
    return;
}

void move_snake(snake_t *snake, apple_t *apple, direction_t direction) {
    cell_t *head = snake->head;
    cell_t *new_head = malloc(sizeof(cell_t));
    new_head->x = head->x;
    new_head->y = head->y;
    new_head->next = head;
    new_head->prev = NULL;
    head->prev = new_head;
    switch (direction) {
        case UP:
            new_head->y--;
            break;
        case DOWN:
            new_head->y++;
            break;
        case LEFT:
            new_head->x--;
            break;
        case RIGHT:
            new_head->x++;
            break;
    }
    snake->head = new_head;
    if (new_head->x != apple->x || new_head->y != apple->y) {
        cell_t *old_tail = snake->tail;
        snake->tail = snake->tail->prev;
        free(old_tail);
        snake->tail->next = NULL;
    } else {
        snake->length++;
        generate_apple(snake, apple);
    }
}

bool is_game_over(snake_t *snake, apple_t *apple) {
    cell_t *head = snake->head;
    if (head->x < 0 || head->x >= GRID_SIZE) {
        return true;
    }
    if (head->y < 0 || head->y >= GRID_SIZE) {
        return true;
    }
    if (head->x == apple->x && head->y == apple->y) {
        return true;
    }
    cell_t *current = head->next;
    while(current) {
        if (head->x == current->x && head->y == current->y) {
            return true;
        }
        current = current->next;
    }
    
    return false;
}

void render_grid(SDL_Renderer *renderer, int x, int y) {
    SDL_SetRenderDrawColor(renderer, 0x55, 0x55, 0x55, SDL_ALPHA_OPAQUE);
    SDL_Rect cell;
    #if 0
    cell.w = CELL_SIZE;
    cell.h = CELL_SIZE;
    
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            cell.x = x + i * CELL_SIZE;
            cell.y = y + j * CELL_SIZE;
            SDL_RenderDrawRect(renderer, &cell);
        }
    }
    #else
    cell.w = GRID_SIZE * CELL_SIZE;
    cell.h = GRID_SIZE * CELL_SIZE;
    cell.x = x;
    cell.y = y;
    SDL_RenderDrawRect(renderer, &cell);
    #endif
    
    return;
}

void render_snake(SDL_Renderer *renderer, snake_t *snake) {
    cell_t *current = snake->head;
    SDL_Rect cell;
    cell.w = CELL_SIZE;
    cell.h = CELL_SIZE;
    Uint8 alpha = 255;
    Uint8 step = 160 / snake->length;
    while (current) {
        cell.x = OFFSET_X + current->x * CELL_SIZE;
        cell.y = OFFSET_Y + current->y * CELL_SIZE;
        SDL_SetRenderDrawColor(renderer, 0x34, 0xEB, 0x61, alpha);
        SDL_RenderFillRect(renderer, &cell);
        alpha -= step;
        current = current->next;
    }
    return;
}

void render_apple(SDL_Renderer *renderer, apple_t *apple) {
    SDL_SetRenderDrawColor(renderer, 0xF5, 0x42, 0x42, SDL_ALPHA_OPAQUE);
    SDL_Rect cell;
    cell.w = CELL_SIZE;
    cell.h = CELL_SIZE;
    cell.x = OFFSET_X + apple->x * CELL_SIZE;
    cell.y = OFFSET_Y + apple->y * CELL_SIZE;
    SDL_RenderFillRect(renderer, &cell);
    return;
}

void render(SDL_Renderer *renderer, snake_t *snake, apple_t *apple) {
    SDL_RenderClear(renderer);
    render_grid(renderer, OFFSET_X, OFFSET_Y);
    render_snake(renderer, snake);
    render_apple(renderer, apple);
    SDL_SetRenderDrawColor(renderer, 0x11, 0x11, 0x11, SDL_ALPHA_OPAQUE);
    SDL_RenderPresent(renderer);
    SDL_Delay(60);
}

void game_loop(SDL_Renderer *renderer) {
    bool quit = false;
    direction_t direction;
    SDL_Event event;
    snake_t *snake = init_snake(5, &direction);
    apple_t *apple = init_apple();

    while (!quit) {
        bool direction_changed = false;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYUP:
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            quit = true;
                            break;
                        case SDLK_w:
                        case SDLK_UP:
                            if (direction != DOWN && !direction_changed) {
                                direction = UP;
                                direction_changed = true;
                            }
                            break;
                        case SDLK_s:
                        case SDLK_DOWN:
                            if (direction != UP && !direction_changed) {
                                direction = DOWN;
                                direction_changed = true;
                            }
                            break;
                        case SDLK_a:
                        case SDLK_LEFT:
                            if (direction != RIGHT && !direction_changed) {
                                direction = LEFT;
                                direction_changed = true;
                            }
                            break;
                        case SDLK_d:
                        case SDLK_RIGHT:
                            if (direction != LEFT && !direction_changed) {
                                direction = RIGHT;
                                direction_changed = true;
                            }
                            break;

                    }
            }
        }
        // Render LOOP START
        move_snake(snake, apple, direction);
        if (is_game_over(snake, apple)) {
            free(snake);
            snake = init_snake(5, &direction);

        }
        render(renderer, snake, apple);
        // RENDER LOOP END
    }
}

int main(int c, char* args[]) {
    srand((unsigned)time(NULL));
    SDL_Window *window;
    SDL_Renderer *renderer;
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "ERROR: Init Video");
    }
    window = SDL_CreateWindow(
        "Snake",
        WINDOW_X,
        WINDOW_Y,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        fprintf(stderr, "ERROR: !window");
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "ERROR: !renderer");
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    game_loop(renderer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}

