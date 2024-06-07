#pragma once

// Tools
enum Tool
{
    BRUSH,
    BUCKET,
    ERASER
};

void drawGame(Screen *screen)
{
    static bool initialized = false;
    static ColorPalette *palette;
    static Canvas *canvas;
    static Painter *painter;
    static Tool currentTool = BRUSH;
    int colorIndex, originalColor;

    if (!initialized)
    {
        palette = new ColorPalette();
        canvas = new Canvas(800, 450, *palette);
        painter = new Painter(*palette, *canvas);
        initialized = true;
    }

    Vector2 position = GetMousePosition();

    float mouseWheelMove = GetMouseWheelMove();
    painter->SetBrushSize(mouseWheelMove);

    // Switch tool with SPACE key
    if (IsKeyPressed(KEY_SPACE))
    {
        currentTool = (currentTool == BRUSH) ? BUCKET : BRUSH;
    }

    // Paint or fill if the left mouse button is pressed
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        // Check if a color in the palette is clicked
        colorIndex = canvas->CheckPaletteClick(*palette);
        if (colorIndex >= 0)
        {
            painter->SetColor(colorIndex);
            originalColor = colorIndex;
        }
        else
        {
            if (currentTool == BRUSH)
            {
                painter->Paint(position);
            }
            else if (currentTool == BUCKET)
            {
                painter->Fill(position);
            }
        }
    }
    else
    {
        painter->ResetLastPosition();
    }

    // Check if right mouse button is pressed for eraser
    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
    {
        painter->SetColor(0); // Set color to white for erasing
        painter->Paint(position);
        painter->SetColor(originalColor); // Restore the original color
    }

    // Draw
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawTexture(screen->background, 0, 0, WHITE);

    // Canvas
    Rectangle rec = {0, 0, (float)canvas->GetTarget().texture.width, (float)-canvas->GetTarget().texture.height};
    Vector2 canvasPosition;
    canvasPosition.x = GetScreenWidth() / 2.0f - canvas->GetTarget().texture.width / 2.0f;
    canvasPosition.y = GetScreenHeight() / 2.0f - canvas->GetTarget().texture.height / 2.0f;

    DrawTextureRec(canvas->GetTarget().texture, rec, canvasPosition, WHITE);

    // Brush outline
    if (currentTool == BRUSH)
    {
        DrawCircleLines(GetMouseX(), GetMouseY(), painter->GetBrushSize(), painter->GetColor());
    }
    else if (currentTool == BUCKET)
    {
        DrawCircleLines(GetMouseX(), GetMouseY(), 10, painter->GetColor()); // Small circle for bucket tool indicator
    }

    // Palette
    canvas->DrawPalette(*palette);
    EndDrawing();
}
