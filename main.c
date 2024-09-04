#include <stdio.h>
#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
// #if defined(PLATFORM_WEB)
//     #include <emscripten/emscripten.h>
// #endif
#include <emscripten/emscripten.h>

#define GENERATION_WIDTH 100
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000
#define BUTTON_SIZE 20
#define CELL_SIZE 5
const int cell_gap = 0.25 * CELL_SIZE; 
const int max_generations = ((WINDOW_HEIGHT - 100) / (CELL_SIZE + cell_gap)) - 10;
// const int generation_width = (WINDOW_WIDTH / (CELL_SIZE + cell_gap)) - 2;
const int generation_width = GENERATION_WIDTH;
int parent_generation[GENERATION_WIDTH] = {0};
int current_generation[GENERATION_WIDTH] = {0};

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

Rule rules[8];
Button buttons[8];

void populate_rules(Rule *rules);
void generate_next_generation(int generation);
void draw_generations();
void draw_buttons();
void check_button_click(Vector2 mousePoint);
void check_button_click_save_image(Vector2 mousePoint);
bool save_image();
void UpdateDrawFrame();
const char* generate_random_name();

int main()
{

    populate_rules(rules);
    
    
    InitWindow(WINDOW_HEIGHT, WINDOW_WIDTH, "automata");

    #if defined(PLATFORM_WEB)
        emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
    #endif
    // SetTargetFPS(60);
// #endif

    while (!WindowShouldClose())
    {
        UpdateDrawFrame();       
    }
    CloseWindow();

    return 0;
}

void UpdateDrawFrame()
{
    
    Vector2 mousePoint = GetMousePosition();
    check_button_click(mousePoint);
    check_button_click_save_image(mousePoint);
    
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Build your automaton", 50, 12, 20, RED);
        DrawText("Mutate any rule cell - ", 50, 40, 20, DARKGRAY);   
        draw_generations();
        draw_buttons();
    EndDrawing();
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
    // *(rules + 8) = (Rule){1, 1, 2, 0};
    // *(rules + 9) = (Rule){1, 2, 1, 0};
    // *(rules + 10) = (Rule){1, 2, 2, 0};
    // *(rules + 11) = (Rule){2, 1, 1, 0};
    // *(rules + 12) = (Rule){2, 1, 2, 0};
    // *(rules + 13) = (Rule){2, 2, 1, 0};
    // *(rules + 14) = (Rule){2, 2, 2, 0};
    // *(rules + 15) = (Rule){1, 2, 0, 1};
    // *(rules + 16) = (Rule){2, 1, 0, 1};
    // *(rules + 17) = (Rule){2, 2, 0, 1};
    // *(rules + 18) = (Rule){2, 0, 1, 1};
    // *(rules + 19) = (Rule){2, 0, 2, 1};
    // *(rules + 20) = (Rule){0, 1, 2, 1};
    // *(rules + 21) = (Rule){0, 2, 1, 1};
    // *(rules + 22) = (Rule){0, 2, 2, 1};
    // *(rules + 23) = (Rule){0, 2, 0, 1};
    // *(rules + 24) = (Rule){2, 1, 0, 1};
    // *(rules + 25) = (Rule){2, 2, 0, 1};
    // *(rules + 26) = (Rule){2, 0, 2, 1};
}

void generate_next_generation(int generation)
{
    if (generation == 0)
    {
        // *(parents + generation_width/2) = 1;
        current_generation[generation_width/2] = 1;
    }
    for (int i = 1; i < generation_width; i++)
    {

        if (generation != 0 && generation != max_generations - 1)
        {
            int left = parent_generation[i-1];
            int center = parent_generation[i];
            int right = parent_generation[i+1];
            for (int j = 0; j <= 7; j++)
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
            switch (current_generation[i])
            {
                case 0:
                    DrawRectangle(i*(CELL_SIZE + cell_gap), 100+j*(CELL_SIZE + cell_gap), CELL_SIZE, CELL_SIZE, GREEN);
                    continue;
                case 1:
                    DrawRectangle(i*(CELL_SIZE + cell_gap), 100+j*(CELL_SIZE + cell_gap), CELL_SIZE, CELL_SIZE, ORANGE);
                    continue;
                // case 2:
                //     DrawRectangle(i*(CELL_SIZE + cell_gap), 100+j*(CELL_SIZE + cell_gap), CELL_SIZE, CELL_SIZE, RAYWHITE);
                //     continue;
            }

        } 
    }
}

void draw_buttons()
{
    for (int i = 0; i <= 7; i++)
    {
        buttons[i].x = 300+i*(BUTTON_SIZE + cell_gap);
        buttons[i].y = 40;
        buttons[i].width = BUTTON_SIZE;
        buttons[i].height = BUTTON_SIZE;
        buttons[i].value = rules[i].result;
        switch (buttons[i].value)
        {
            case 0:
                DrawRectangle(buttons[i].x, buttons[i].y, buttons[i].width, buttons[i].height, GREEN);
                continue;
            case 1:
                DrawRectangle(buttons[i].x, buttons[i].y, buttons[i].width, buttons[i].height, ORANGE);
                continue;
            // case 2:
            //     DrawRectangle(buttons[i].x, buttons[i].y, buttons[i].width, buttons[i].height, RAYWHITE);
            //     break;
        }
    }

    // save image button
    DrawRectangle(50, 65, 100, 20, GRAY);
    DrawText("Save image!", 60, 70, 12, RAYWHITE);   
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
            buttons[i].value = (buttons[i].value + 1) % 2;
            rules[i].result = buttons[i].value;
            buttonact = false;
            break;
        }
    }
}

void check_button_click_save_image(Vector2 mousePoint)
{
    bool buttonact = false;
        
    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){50, 65, 100, 20}))
    {
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) buttonact = true;
    }

    if (buttonact)
    {
        save_image();
        buttonact = false;
    }
}

const char* generate_random_name()
{
    const char* names[] = {
    "Weaver", "Symmetry", "Pixelor", "Fracton", "Automaton", 
    "Quanta", "Tessera", "Chaos", "Nexor", "Matrix",
    "Entropy", "Lattice", "Echora", "Patternix", "Cellor",
    "Archa", "Turinga", "Mosaic", "Algos", "Circuitor"
    };
    
    // Get the number of names
    int numNames = sizeof(names) / sizeof(names[0]);
    
    // Seed the random number generator
    srand(time(NULL));
    
    // Randomly select a name
    int randomIndex = rand() % numNames;

    return names[randomIndex];
}

bool save_image()
{
    // Capture the render texture instead of reading screen pixels
    RenderTexture2D target = LoadRenderTexture(WINDOW_WIDTH, WINDOW_HEIGHT);
    BeginTextureMode(target);
        // Redraw your scene here
        // This is where you would call your drawing functions to recreate the current frame
        // For example:
        ClearBackground(RAYWHITE);
        // DrawText("Hello, World!", 190, 200, 20, LIGHTGRAY);
        // Draw your pattern or whatever content you have on screen
        draw_generations();
    EndTextureMode();

    // Get image from render texture
    Image image = LoadImageFromTexture(target.texture);

    // Flip image vertically
    ImageFlipVertical(&image);

    // Define the region you want to crop
    int cropX = 0;
    int cropY = 100;
    int cropWidth = generation_width * (CELL_SIZE + cell_gap);
    int cropHeight = WINDOW_HEIGHT - 100;

    // Crop the image
    Rectangle cropRect = { cropX, cropY, cropWidth, cropHeight };
    Image croppedImage = ImageFromImage(image, cropRect);

    const char* randomName = generate_random_name();

    // Draw text on the cropped image
    ImageDrawText(&croppedImage, randomName, 10, 10, 20, RED);

    // Export the image to a file in MEMFS
    const char* tempFileName = "/temp_image.png";
    ExportImage(croppedImage, tempFileName);

    // Call the JavaScript function to save the file from MEMFS to disk
    EM_ASM({
        saveFileFromMEMFSToDisk(UTF8ToString($0), UTF8ToString($1));
    }, tempFileName, "automata.png");

    // Clean up
    UnloadImage(croppedImage);
    UnloadImage(image);
    UnloadRenderTexture(target);

    // Remove the temporary file from MEMFS
    EM_ASM({
        FS.unlink(UTF8ToString($0));
    }, tempFileName);

    return true;
}