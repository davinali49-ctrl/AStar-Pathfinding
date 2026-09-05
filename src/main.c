#include <raylib.h>
#include <stdlib.h>
#include <time.h>

#define ROW 20
#define COL 20
#define CELL_SIZE 30
#define FOOTER_HEIGHT 120

typedef struct Node
{
    int row;
    int col;

    int g;
    int h;
    int f;

    int parentRow;
    int parentCol;

} Node;

// menghitung heuristic (perkiraan jarak menuju tujuan. Nilai ini membantu algoritma menentukan arah pencarian agar lebih efisien.)
int heuristic(int row1, int col1, int row2, int col2)
{
    return abs(row1 - row2) + abs(col1 - col2);
}

// mereset data node
void ResetNodes(Node nodes[ROW][COL],
                bool open[ROW][COL],
                bool closed[ROW][COL],
                bool path[ROW][COL])
{
    for (int row = 0; row < ROW; row++)
    {
        for (int col = 0; col < COL; col++)
        {
            nodes[row][col].g = 99999;
            nodes[row][col].h = 0;
            nodes[row][col].f = 99999;

            nodes[row][col].parentRow = -1;
            nodes[row][col].parentCol = -1;

            open[row][col] = false;
            closed[row][col] = false;
            path[row][col] = false;
        }
    }
}

bool astarFound = false;
int astarCurrentRow = -1;
int astarCurrentCol = -1;

bool AStarStep(Node nodes[ROW][COL],
               bool open[ROW][COL],
               bool closed[ROW][COL],
               bool path[ROW][COL],
               int grid[ROW][COL],
               int startRow, int startCol,
               int goalRow, int goalCol)
{
    int lowestF = 99999;
    int currentRow = -1;
    int currentCol = -1;

    for (int row = 0; row < ROW; row++)
    {
        for (int col = 0; col < COL; col++)
        {
            if (open[row][col] && nodes[row][col].f < lowestF)
            {
                lowestF = nodes[row][col].f;
                currentRow = row;
                currentCol = col;
            }
        }
    }

    if (currentRow == -1)
    {
        return true;
    }

    astarCurrentRow = currentRow;
    astarCurrentCol = currentCol;

    open[currentRow][currentCol] = false;
    closed[currentRow][currentCol] = true;

    if (currentRow == goalRow && currentCol == goalCol)
    {
        astarFound = true;

        int row = goalRow;
        int col = goalCol;

        while (!(row == startRow && col == startCol))
        {
            path[row][col] = true;

            int parentRow = nodes[row][col].parentRow;
            int parentCol = nodes[row][col].parentCol;

            row = parentRow;
            col = parentCol;
        }

        path[startRow][startCol] = true;
        return true;
    }

    int dRow[] = {-1, 1, 0, 0};
    int dCol[] = {0, 0, -1, 1};

    for (int i = 0; i < 4; i++)
    {
        int newRow = currentRow + dRow[i];
        int newCol = currentCol + dCol[i];

        if (newRow < 0 || newRow >= ROW || newCol < 0 || newCol >= COL)
        {
            continue;
        }

        if (grid[newRow][newCol] || closed[newRow][newCol])
        {
            continue;
        }

        int newG = nodes[currentRow][currentCol].g + 1;

        if (!open[newRow][newCol] || newG < nodes[newRow][newCol].g)
        {
            nodes[newRow][newCol].g = newG;

            nodes[newRow][newCol].h =
                heuristic(newRow, newCol, goalRow, goalCol);

            nodes[newRow][newCol].f =
                nodes[newRow][newCol].g +
                nodes[newRow][newCol].h;

            nodes[newRow][newCol].parentRow = currentRow;
            nodes[newRow][newCol].parentCol = currentCol;

            open[newRow][newCol] = true;
        }
    }

    return false;
}

// membuat maze acak menggunakan algoritma depth-first search (DFS) dengan backtracking
void GenerateRandomMaze(int grid[ROW][COL])
{
    for (int row = 0; row < ROW; row++)
    {
        for (int col = 0; col < COL; col++)
        {
            grid[row][col] = 1;
        }
    }

    int stack[ROW * COL][2];
    int top = 0;
    int currentRow = 1;
    int currentCol = 1;

    grid[currentRow][currentCol] = 0;
    stack[top][0] = currentRow;
    stack[top][1] = currentCol;
    top++;

    while (top > 0)
    {
        currentRow = stack[top - 1][0];
        currentCol = stack[top - 1][1];

        int dirs[4][2] = {{-2, 0}, {2, 0}, {0, -2}, {0, 2}};
        for (int i = 0; i < 4; i++)
        {
            int j = rand() % 4;
            int temp0 = dirs[i][0];
            int temp1 = dirs[i][1];
            dirs[i][0] = dirs[j][0];
            dirs[i][1] = dirs[j][1];
            dirs[j][0] = temp0;
            dirs[j][1] = temp1;
        }

        bool carved = false;
        for (int i = 0; i < 4; i++)
        {
            int newRow = currentRow + dirs[i][0];
            int newCol = currentCol + dirs[i][1];

            if (newRow > 0 && newRow < ROW && newCol > 0 && newCol < COL && grid[newRow][newCol] == 1)
            {
                grid[(currentRow + newRow) / 2][(currentCol + newCol) / 2] = 0;
                grid[newRow][newCol] = 0;

                stack[top][0] = newRow;
                stack[top][1] = newCol;
                top++;
                carved = true;
                break;
            }
        }

        if (!carved)
        {
            top--;
        }
    }
}

int main()
{
    InitWindow(
        COL * CELL_SIZE,
        ROW * CELL_SIZE + FOOTER_HEIGHT,
        "Pathfinding");

    SetTargetFPS(60);
    srand((unsigned int)time(NULL));

    int grid[ROW][COL] = {0};

    Node nodes[ROW][COL];

    bool open[ROW][COL] = {false};
    bool closed[ROW][COL] = {false};
    bool path[ROW][COL] = {false};

    int startRow = -1, startCol = -1;
    int goalRow = -1, goalCol = -1;
    int mode = 0;
    int status = 0;

    bool isAnimating = false;
    int animationStep = 0;
    int animationSpeed = 5;

    for (int row = 0; row < ROW; row++)
    {
        for (int col = 0; col < COL; col++)
        {
            nodes[row][col].row = row;
            nodes[row][col].col = col;

            nodes[row][col].g = 99999;
            nodes[row][col].h = 0;
            nodes[row][col].f = 99999;

            nodes[row][col].parentRow = -1;
            nodes[row][col].parentCol = -1;
        }
    }

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_S))
            mode = 1;
        if (IsKeyPressed(KEY_G))
            mode = 2;

        if (IsKeyPressed(KEY_M))
        {
            GenerateRandomMaze(grid);
            ResetNodes(nodes, open, closed, path);
            startRow = startCol = goalRow = goalCol = -1;
            status = 0;
            isAnimating = false;
            astarFound = false;
        }

        // algoritma A* dijalankan
        if (IsKeyPressed(KEY_SPACE))
        {
            if (startRow != -1 && goalRow != -1)
            {
                ResetNodes(nodes, open, closed, path);

                nodes[startRow][startCol].g = 0;
                nodes[startRow][startCol].h =
                    heuristic(startRow, startCol, goalRow, goalCol);
                nodes[startRow][startCol].f =
                    nodes[startRow][startCol].g +
                    nodes[startRow][startCol].h;

                open[startRow][startCol] = true;

                isAnimating = true;
                astarFound = false;
                status = 0;
            }
        }

        if (IsKeyPressed(KEY_UP))
        {
            if (animationSpeed < 20)
                animationSpeed++;
        }
        if (IsKeyPressed(KEY_DOWN))
        {
            if (animationSpeed > 1)
                animationSpeed--;
        }

        if (isAnimating)
        {
            for (int step = 0; step < animationSpeed; step++)
            {
                if (AStarStep(nodes, open, closed, path, grid,
                              startRow, startCol, goalRow, goalCol))
                {
                    isAnimating = false;

                    if (astarFound)
                    {
                        status = 1;
                    }
                    else
                    {
                        status = 2;
                    }
                    break;
                }
            }
        }
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mousePos = GetMousePosition();
            int clickCol = mousePos.x / CELL_SIZE;
            int clickRow = mousePos.y / CELL_SIZE;

            if (clickRow >= 0 && clickRow < ROW && clickCol >= 0 && clickCol < COL)
            {
                status = 0;
                if (mode == 1)
                {
                    if (!grid[clickRow][clickCol])
                    {
                        startRow = clickRow;
                        startCol = clickCol;
                    }

                    mode = 0;
                }
                else if (mode == 2)
                {
                    if (!grid[clickRow][clickCol])
                    {
                        goalRow = clickRow;
                        goalCol = clickCol;
                    }

                    mode = 0;
                }
                else
                {
                    if (clickRow != startRow || clickCol != startCol)
                    {
                        if (clickRow == goalRow && clickCol == goalCol)
                        {
                            goalRow = goalCol = -1;
                        }
                        grid[clickRow][clickCol] = !grid[clickRow][clickCol];
                    }
                }
            }
        }

        // membuat tampilan visual dari grid, jalur, dan status pencarian
        BeginDrawing();

        ClearBackground(RAYWHITE);
        for (int row = 0; row < ROW; row++)
        {
            for (int col = 0; col < COL; col++)
            {
                int x = col * CELL_SIZE;
                int y = row * CELL_SIZE;

                Color cellColor = WHITE;

                if (row == startRow && col == startCol)
                {
                    cellColor = GREEN;
                }
                else if (row == goalRow && col == goalCol)
                {
                    cellColor = RED;
                }
                else if (grid[row][col])
                {
                    cellColor = BLACK;
                }
                else if (path[row][col])
                {
                    cellColor = BLUE;
                }
                else if (closed[row][col])
                {
                    cellColor = ORANGE;
                }
                else if (open[row][col])
                {
                    cellColor = YELLOW;
                }

                DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, cellColor);
                DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, LIGHTGRAY);
            }
        }

        int footerY = ROW * CELL_SIZE + 8;
        int footerLineHeight = 18;

        DrawRectangle(0, ROW * CELL_SIZE, COL * CELL_SIZE, FOOTER_HEIGHT, Fade(LIGHTGRAY, 0.9f));
        DrawRectangleLines(0, ROW * CELL_SIZE, COL * CELL_SIZE, FOOTER_HEIGHT, LIGHTGRAY);

        if (mode == 1)
        {
            DrawText("Klik cell untuk menetapkan START", 10, footerY, 20, GREEN);
        }
        else if (mode == 2)
        {
            DrawText("Klik cell untuk menetapkan GOAL", 10, footerY, 20, RED);
        }
        else
        {
            DrawText("S: START, G: GOAL, M: Maze Acak", 10, footerY, 15, DARKGRAY);
            DrawText("Klik cell untuk tambahkan/hapus dinding", 10, footerY + footerLineHeight, 15, DARKGRAY);
        }

        if (status == 1)
        {
            DrawText("Jalur Ditemukan", 10, footerY + footerLineHeight * 2, 16, GREEN);
        }
        else if (status == 2)
        {
            DrawText("Jalur Tidak Ditemukan", 10, footerY + footerLineHeight * 2, 16, RED);
        }

        if (isAnimating)
        {
            DrawText("Mencari...", 220, footerY + footerLineHeight * 2, 16, BLUE);
        }

        DrawText(TextFormat("Kecepatan: %d (UP/DOWN untuk atur)", animationSpeed),
                 10, footerY + footerLineHeight * 3, 15, DARKGRAY);

        DrawText("Hijau=START  Merah=GOAL  Hitam=Dinding  Biru=Jalur  Oranye=Tertutup  Kuning=Terbuka",
                 10, footerY + footerLineHeight * 4, 13, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}