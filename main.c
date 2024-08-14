#include <stdio.h>
#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <string.h>

#define GENERATION_WIDTH 81
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000
#define BUTTON_SIZE 20
#define CELL_SIZE 10
// #define GENERATION_WIDTH 100
const int max_generations = 100;
const int generation_width = GENERATION_WIDTH;
int parent_generation[GENERATION_WIDTH] = {0};
int current_generation[GENERATION_WIDTH] = {0}; 
// int parent_active_index[]


typedef struct {
    int val1;
    int val2;
    int val3;
    int result;
} Rule;

typedef struct {
    int x;
    int y;
    int width;
    int height;
    int value;
} Button;

Rule rules[7];
Button buttons[7];

void populate_rules(Rule *rules);
void generate_next_generation(int generation);
void draw_generations();
void draw_buttons();
void check_button_click(Vector2 mousePoint);

int main()
{

    populate_rules(rules);

    InitWindow(WINDOW_HEIGHT, WINDOW_WIDTH, "automata");

    // Camera2D camera = { 0 };
    // camera.zoom = 1.0f;
    Vector2 mousePoint = { 0.0f, 0.0f };

    int frame = 0;

    while (!WindowShouldClose())
    {
        mousePoint = GetMousePosition();
        check_button_click(mousePoint);
        BeginDrawing();
        // DrawFPS(10, 10);
        DrawText("Mutate any rule cell - ", 100, 12, 15, DARKGRAY);   
        draw_generations();
        draw_buttons();
        ClearBackground(RAYWHITE);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void populate_rules(Rule *rules)
{
    *(rules + 0) = (Rule){1, 1, 1, 0};
    *(rules + 1) = (Rule){1, 1, 0, 1};
    *(rules + 2) = (Rule){1, 0, 1, 1};
    *(rules + 3) = (Rule){1, 0, 0, 1};
    *(rules + 4) = (Rule){0, 1, 1, 1};
    *(rules + 5) = (Rule){0, 1, 0, 1};
    *(rules + 6) = (Rule){0, 0, 1, 1};
    *(rules + 7) = (Rule){0, 0, 0, 0};
}

void generate_next_generation(int generation)
{
    if (generation == 0)
    {
        // *(parents + generation_width/2) = 1;
        current_generation[generation_width/2] = 1;
    }
    for (int i = 0; i < generation_width; i++)
    {

        if (generation != 0 && generation != max_generations - 1)
        {
            // int left = *(parents + i - 1);
            // int center = *(parents + i);
            // int right = *(parents + i + 1);
            int left = parent_generation[i-1];
            int center = parent_generation[i];
            int right = parent_generation[i+1];
            for (int j = 0; j < 7; j++)
            {
                if (rules[j].val1 == left && rules[j].val2 == center && rules[j].val3 == right)
                {
                    // *(children + i) = rules[j].result;
                    current_generation[i] = rules[j].result;
                }
            }
        }
    }
    if (generation == max_generations - 1)
    {
        memset(parent_generation, 0, sizeof(parent_generation));
        memset(current_generation, 0, sizeof(current_generation));
        return;
    }
    memcpy(parent_generation, current_generation, sizeof(current_generation));
}

void draw_generations()
{
    for (int j = 0; j < max_generations; j++)
    {
        generate_next_generation(j);
        for (int i = 0; i < generation_width; i++)
        {
            DrawRectangle(i*12, 100+j*12, 10, 10, (current_generation[i] == 1) ? ORANGE : GREEN); 

        } 
    }
}

void draw_buttons()
{
    for (int i = 0; i <= 7; i++)
    {
        buttons[i].x = 400+i*24;
        buttons[i].y = 12;
        buttons[i].width = 20;
        buttons[i].height = 20;
        buttons[i].value = rules[i].result;
        DrawRectangle(buttons[i].x, buttons[i].y, buttons[i].width, buttons[i].height, (buttons[i].value == 1) ? ORANGE : GREEN);
    }
}

void check_button_click(Vector2 mousePoint)
{
    bool buttonact = false;
    for (int i = 0; i <= 7; i++)
    {
        
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){buttons[i].x, buttons[i].y, buttons[i].width, buttons[i].height}))
        {
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) buttonact = true;
        }

        if (buttonact)
        {
            buttons[i].value = !buttons[i].value;
            rules[i].result = buttons[i].value;
            buttonact = false;
            break;
        }
    }
}