#include "raylib.h"
#include "math.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

float percentOf(float percent, float number)
{
    return (percent / number) * 100.0f;
}

float asPercentOf(float number, float percent)
{
    return (percent / 100.0f) * number;
}

int randomInt(int min, int max)
{
    return (rand() % max) + min;
}

enum Direction {
    UP, DOWN, LEFT, RIGHT
};

struct Snake {
    int x;
    int y;
    float speed;
    float moveTimer;
    enum Direction direction;
    enum Direction nextDirection;
};

struct Snake SnakeCreate(int x, int y)
{
    return (struct Snake) {
        .x = x,
        .y = y,
        .speed = 0.2f,
        .moveTimer = 0,
        .direction = RIGHT,
        .nextDirection = RIGHT,
    };
}

void SnakeMoveInCurrentDirection(struct Snake *s)
{
    switch(s->direction)
    {
        case UP:
            s->y -= 1;
            break;
        case DOWN:
            s->y += 1;
            break;
        case LEFT:
            s->x -= 1;
            break;
        case RIGHT:
            s->x += 1;
            break;
        default:
            break;
    }
}

void SnakeScreenWrap(struct Snake *s, int colCount, int rowCount)
{
    if(s->x < 0) {
        s->x = colCount - 1;
        s->direction = LEFT;
    }
    if(s->x > colCount - 1) {
        s->x = 0;
        s->direction = RIGHT;
    }
    if(s->y < 0) {
        s->y = rowCount - 1;
        s->direction = UP;
    }
    if(s->y > rowCount - 1) {
        s->y = 0;
        s->direction = DOWN;
    }
}

void SnakeUpdate(struct Snake *s, float dt, struct Snake *target)
{
    s->moveTimer += dt;
    if(s->moveTimer >= s->speed) {
        s->moveTimer -= s->speed;

        if(target == NULL) {
            if(s->direction != s->nextDirection)
            {
                s->direction = s->nextDirection;
            }

            SnakeMoveInCurrentDirection(s);
        } else {
            s->x = target->x;
            s->y = target->y;
            s->direction = target->direction;
        }
    }
}

void SnakeSetDirection(struct Snake *s, int dirX, int dirY)
{
    if(dirX && dirY) return;
    if(!dirX && !dirY) return;
    if(dirY && (s->direction == UP || s->direction == DOWN)) return;
    if(dirX && (s->direction == LEFT || s->direction == RIGHT)) return;

    if(dirX == -1) {
        s->nextDirection = LEFT;
    }
    if(dirX == 1) {
        s->nextDirection = RIGHT;
    }
    if(dirY == -1) {
        s->nextDirection = UP;
    }
    if(dirY == 1) {
        s->nextDirection = DOWN;
    }
}

void SnakeDrawHead(struct Snake *s, int cellSize, Color color)
{
    int w, h;
    float growthPercent = percentOf(s->moveTimer, s->speed);

    switch(s->direction)
    {
        case UP:
            h = floor(asPercentOf(cellSize, growthPercent));
            DrawRectangle(s->x * cellSize, (s->y * cellSize) + (cellSize - h), cellSize, h, color);
            break;
        case DOWN:
            h = floor(asPercentOf(cellSize, growthPercent));
            DrawRectangle(s->x * cellSize, s->y * cellSize, cellSize, h, color);
            break;
        case RIGHT:
            w = floor(asPercentOf(cellSize, growthPercent));
            DrawRectangle(s->x * cellSize, s->y * cellSize, w, cellSize, color);
            break;
        case LEFT:
            w = floor(asPercentOf(cellSize, growthPercent));
            DrawRectangle((s->x * cellSize) + (cellSize - w), s->y * cellSize, w, cellSize, color);
            break;
        default:
            break;
    }
}

void SnakeDrawTail(struct Snake *s, int cellSize, Color color)
{
    int w, h;
    float growthPercent = percentOf(s->moveTimer, s->speed);

    switch(s->direction)
    {
        case UP:
            h = floor(asPercentOf(cellSize, growthPercent));
            DrawRectangle(s->x * cellSize, s->y * cellSize, cellSize, cellSize - h, color);
            break;
        case DOWN:
            h = floor(asPercentOf(cellSize, growthPercent));
            DrawRectangle(s->x * cellSize, s->y * cellSize + h, cellSize, cellSize - h, color);
            break;
        case RIGHT:
            w = floor(asPercentOf(cellSize, growthPercent));
            DrawRectangle(s->x * cellSize + w, s->y * cellSize, cellSize - w, cellSize, color);
            break;
        case LEFT:
            w = floor(asPercentOf(cellSize, growthPercent));
            DrawRectangle(s->x * cellSize, s->y * cellSize, cellSize - w, cellSize, color);
            break;
        default:
            break;
    }
}

void SnakeDraw(struct Snake *s, int cellSize, Color color)
{
    DrawRectangle(s->x * cellSize, s->y * cellSize, cellSize, cellSize, color);
}

struct Apple {
	int x;
	int y;
	int active;
	float lifeSpan;
	float lifeTimer;
};

struct Apple AppleCreate()
{
	return (struct Apple) {
		.x = 0,
		.y = 0,
		.active = 0,
		.lifeSpan = 0.0f,
		.lifeTimer = 0.0f,
	};
}

void AppleUpdate(struct Apple *a, float dt)
{
	if(!a->active) return;

	a->lifeTimer += dt;
	if(a->lifeTimer > a->lifeSpan)
	{
		a->lifeTimer -= a->lifeSpan;
		a->active = 0;
	}
}

void AppleActivateAt(struct Apple *a, int x, int y)
{
	a->x = x;
	a->y = y;
	a->active = 1;
	a->lifeTimer = 0;
	a->lifeSpan = 5.0f;
}

void AppleDraw(struct Apple *a, int cellSize, Color color)
{
	if(!a->active) return;
    DrawRectangle(a->x * cellSize, a->y * cellSize, cellSize, cellSize, color);
}

int collidesWithSnake(struct Snake s[], int snakeLength, int x, int y)
{
	for(int i = 0; i < snakeLength; i++)
	{
		if(s[i].x == x && s[i].y == y) return 1;
	}

	return 0;
}

Vector2 getRandomCellCoords(int colCount, int rowCount)
{
	int x = randomInt(0, colCount);
	int y = randomInt(0, rowCount);
	return (Vector2) { x, y };
}

int main(void)
{
    srand((unsigned)time(NULL));

    const int screenSize = 600;
    const int screenWidth = screenSize;
    const int screenHeight = screenSize;

    InitWindow(screenWidth, screenHeight, "snake");
    SetTargetFPS(60);

    const int gridSize = 20;
    const int cellSize = floor(screenSize / gridSize);
    const int colCount = floor(screenWidth / cellSize);
    const int rowCount = floor(screenHeight / cellSize);
	
	int gameInProgress = 1;

    int snakeLength = 4;
    int snakeMaxLength = colCount * rowCount;
    struct Snake snake[snakeMaxLength];
    for(int i = 0; i < snakeLength; i++)
    {
        snake[i] = SnakeCreate(floor(colCount / 2) - i, floor(rowCount / 2));
    }

	struct Apple apple = AppleCreate();

	float appleSpawnInterval = 8.0f;
	float appleSpawnTimer = 0.0f;

    while(!WindowShouldClose())
    {
        // update
		if(gameInProgress) {
			int up, down, left, right, dirX, dirY;

			float dt = GetFrameTime();

			up = IsKeyDown(KEY_UP) ? -1 : 0;
			down = IsKeyDown(KEY_DOWN) ? 1 : 0;
			left = IsKeyDown(KEY_LEFT) ? -1 : 0;
			right = IsKeyDown(KEY_RIGHT) ? 1 : 0;

			dirY = up + down;
			dirX = left + right;

			for(int i = snakeLength; i > 0; i--)
			{
				SnakeUpdate(&snake[i], dt, &snake[i - 1]);
			}

			snake[snakeLength - 1].direction = snake[snakeLength - 2].direction;
			SnakeSetDirection(&snake[0], dirX, dirY);
			SnakeUpdate(&snake[0], dt, NULL);
			SnakeScreenWrap(&snake[0], colCount, rowCount);

			if(apple.active && snake[0].x == apple.x && snake[0].y == apple.y)
			{
				apple.active = 0;
				snake[snakeLength] = snake[snakeLength - 1];
				snakeLength++;
			}

			if(!apple.active)
			{
				appleSpawnTimer += dt;
				if(appleSpawnTimer > appleSpawnInterval)
				{
					appleSpawnTimer -= appleSpawnInterval;

					Vector2 newPos = getRandomCellCoords(colCount, rowCount);
					while(collidesWithSnake(snake, snakeLength, newPos.x, newPos.y)) {
						newPos = getRandomCellCoords(colCount, rowCount);
					}
					AppleActivateAt(&apple, newPos.x, newPos.y);
				}
			}

			AppleUpdate(&apple, dt);

			int tailLength = snakeLength - 1;
			struct Snake tail[tailLength];
			for(int i = 1; i < tailLength; i++)
			{
				tail[i] = snake[i];
			}
			if(collidesWithSnake(tail, tailLength, snake[0].x, snake[0].y))
			{
				gameInProgress = 0;
			}
		}

        // draw
        BeginDrawing();
        ClearBackground(BLACK);

        for(int r = 0; r < rowCount; r++)
        {
            for(int c = 0; c < colCount; c++) {
                Color color = ((r + c) % 2) == 0 ? RAYWHITE : WHITE;
                DrawRectangle(c * cellSize, r * cellSize, cellSize, cellSize, color);
            }
        }

        for(int i = 1; i < snakeLength - 1; i++)
        {
            SnakeDraw(&snake[i], cellSize, DARKGREEN);
        }
        SnakeDrawHead(&snake[0], cellSize, DARKGREEN);
        SnakeDrawTail(&snake[snakeLength - 1], cellSize, DARKGREEN);

		AppleDraw(&apple, cellSize, RED);

		if(!gameInProgress) {
			const char *gameOverText = "GAME OVER";
			int fontSize = 40;
			int textLength = MeasureText(gameOverText, fontSize);

			int posX = floor(screenWidth / 2) - floor(textLength / 2);
			int posY = floor(screenHeight / 2);
			DrawText(gameOverText, posX, posY, fontSize, BLACK);
		}

        EndDrawing();
    }

    CloseWindow();
}
