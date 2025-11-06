#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#define SCREEN_HEIGHT 900
#define SCREEN_WIDTH 1600
#define WIDTH_LIMIT SCREEN_WIDTH*15
#define GRAVITY 1.6
#define MOVE_SPEED 40
#define JUMP_STRENGTH 50
#define SIZE 128


typedef struct
{
    Vector2 position;
    Vector2 velocity;
    int vidas;
    bool amuletos[6];
    Vector2 tamanho;
    bool chao;
    float invtimer;
    bool inbench;
    int coins;

} player;
typedef struct
{
    char m[22][200];
    int rows,cols;
} map;

void initplayer(player *p)
{

    p->velocity.x=00.0f;
    p->velocity.y=00.0f;
    p->tamanho.x=SIZE-4;
    p->tamanho.y=SIZE-4;
    p->chao=false;
    p->vidas = 5;
    p->invtimer = 0;
    p->inbench = false;
    p->coins = 0;
    for(int i = 0; i<6; i++)
        p->amuletos[i]=false;

}

void floodfill(map *ma, int i, int j, char mark)
{
    if (i < 0 || i >= ma->rows || j < 0 || j >= ma->cols) return; // Use actual dimensions

    if (ma->m[i][j] != ' ' ) return;
    if (ma->m[i][j] == mark) return;
    char original = ma->m[i][j];
    ma->m[i][j] = mark;
    floodfill(ma, i+1, j, mark);
    floodfill(ma, i-1, j, mark);
    floodfill(ma, i, j+1, mark);
    floodfill(ma, i, j-1, mark);

}
void markexterior(map *ma)
{
    for (int j = 0; j < ma->cols; j++)
    {
        floodfill(ma, 0, j, 'E');
        floodfill(ma, ma->rows - 1, j, 'E');
    }
    for (int i = 0; i < ma->rows; i++)
    {
        floodfill(ma, i, 0, 'E');
        floodfill(ma, i, ma->cols - 1, 'E');
    }
}

void loadmap(map *mapa, char *filename)
{
    FILE *fmap = fopen(filename, "r");
    mapa->rows = 0;
    mapa->cols = 0;

    for (int i = 0; i < 22; i++) // Max possible rows
    {
        if (fgets(mapa->m[i], 200, fmap) == NULL) break;
        mapa->rows++;
        int len = strlen(mapa->m[i]);
        if (len > mapa->cols) mapa->cols = len;
    }

    markexterior(mapa);
    fclose(fmap);
}
void loadmaps (map *maps)
{
    loadmap(&maps[0],"maps/lobby.txt");
    loadmap(&maps[1],"maps/maptest.txt");
    loadmap(&maps[2],"maps/mapa2.txt");
}
void drawmap(map ma,player *p, Camera2D camera)
{
    BeginMode2D(camera);
    char temp;
    float xsize=SIZE;
    float ysize=SIZE;
    for(int i = 0; i<16; i++)
    {
        for(int j = 0; j<150; j++)
        {
            temp=ma.m[i][j];
            switch(temp)
            {
            case 'J': //spawn
                DrawRectangle(xsize*j,ysize*i,xsize,ysize,BEIGE);
                break;
            case 'P': //bloco padrao
                DrawRectangle(xsize*j,ysize*i,xsize,ysize,BLUE);
                break;
            case 'M': //monstro
                DrawRectangle(xsize*j,ysize*i,xsize,ysize,YELLOW);
                break;
            case 'A': //amuleto
                DrawRectangle(xsize*j, ysize*i, xsize, ysize, BEIGE);
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,GREEN);
                break;
            case 'C': //boss
                DrawRectangle(xsize*j, ysize*i, xsize, ysize, BEIGE);
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,PINK);
                break;
            case 'H'://habilidade
                DrawRectangle(xsize*j,ysize*i,xsize,ysize,LIME);
                break;

            case 'E': //de fora
                DrawRectangle(xsize*j, ysize*i, xsize, ysize, PURPLE);
                break;
            case 'S': //loja
                DrawRectangle(xsize*j, ysize*i, xsize, ysize, GOLD);
                break;
            case 'B': // banco
                DrawRectangle(xsize*j, ysize*i, xsize, ysize, BEIGE);
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,PINK);
                break;
            case '1': // fase 1
                DrawRectangle(xsize*j, ysize*i, xsize, ysize, BEIGE);
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,DARKPURPLE);
                DrawText("1",xsize*(0.4+j), ysize*(0.4+i), 60, GREEN);
                break;
            case '2': // fase 2
                DrawRectangle(xsize*j, ysize*i, xsize, ysize, BEIGE);
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,DARKPURPLE);
                DrawText("2",xsize*(0.4+j), ysize*(0.4+i), 60, GREEN);
                break;
            case '3': // fase 3
                DrawRectangle(xsize*j, ysize*i, xsize, ysize, BEIGE);
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,DARKPURPLE);
                DrawText("3",xsize*(0.4+j), ysize*(0.4+i), 60, GREEN);
                break;
            case '4': // fase 4
                DrawRectangle(xsize*j, ysize*i, xsize, ysize, BEIGE);
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,DARKPURPLE);
                DrawText("4",xsize*(0.4+j), ysize*(0.4+i), 60, GREEN);
                break;
            case '5': // fase 5
                DrawRectangle(xsize*j, ysize*i, xsize, ysize, BEIGE);
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,DARKPURPLE);
                DrawText("5",xsize*(0.4+j), ysize*(0.4+i), 60, GREEN);
                break;

            default: //de dentro
                DrawRectangle(xsize*j, ysize*i, xsize, ysize, BEIGE);
                break;

            }
        }
    }
    EndMode2D();
}
int initmap(map ma, player *p) //determina a posicao inicial do jogador e o numero de inimigos
{
    char temp;
    int nEn;
    for(int i = 0; i < 22; i++)
    {
        for(int j = 0; j < 200; j++)
        {
            temp = ma.m[i][j];
            if(temp=='M')
                nEn++;
            if(temp == 'J')
            {
                p->position.x = j * SIZE + SIZE/2;
                p->position.y = i * SIZE + SIZE/2;
                return nEn;
            }
        }
    }
    return nEn;
}
void PlayerColi(player *p, map ma)
{
    p->position.x += p->velocity.x;
    Rectangle playerRectX =
    {
        p->position.x - p->tamanho.x / 2.0f,
        p->position.y - p->tamanho.y / 2.0f,
        p->tamanho.x,
        p->tamanho.y
    };
    int startRow = floor(playerRectX.y / SIZE);
    int endRow = floor((playerRectX.y + playerRectX.height - 1) / SIZE);
    int startCol = floor(playerRectX.x / SIZE);
    int endCol = floor((playerRectX.x + playerRectX.width - 1) / SIZE);
    for (int i = startRow; i <= endRow; i++)
    {
        for (int j = startCol; j <= endCol; j++)
        {
            if (i < 0 || i >= 22 || j < 0 || j >= 200) continue;
            char tileType = ma.m[i][j];
            if (tileType == 'P' || tileType == 'M' || tileType == 'C')
            {
                Rectangle tileRect = {j * SIZE, i * SIZE, SIZE, SIZE};
                if (CheckCollisionRecs(playerRectX, tileRect))
                {
                    if (p->velocity.x > 0)
                    {
                        p->position.x = tileRect.x - p->tamanho.x / 2.0f;
                    }
                    else if (p->velocity.x < 0)
                    {
                        p->position.x = tileRect.x + SIZE + p->tamanho.x / 2.0f;
                    }
                    p->velocity.x = 0;
                }
            }
        }
    }
    p->position.y += p->velocity.y;
    Rectangle playerRectY =
    {
        p->position.x - p->tamanho.x / 2.0f,
        p->position.y - p->tamanho.y / 2.0f,
        p->tamanho.x,
        p->tamanho.y
    };
    startRow = floor(playerRectY.y / SIZE);
    endRow = floor((playerRectY.y + playerRectY.height - 1) / SIZE);
    startCol = floor(playerRectY.x / SIZE);
    endCol = floor((playerRectY.x + playerRectY.width - 1) / SIZE);
    p->chao = false;
    for (int i = startRow; i <= endRow; i++)
    {
        for (int j = startCol; j <= endCol; j++)
        {
            if (i < 0 || i >= 22 || j < 0 || j >= 200) continue;
            char tileType = ma.m[i][j];
            if (tileType == 'P')
            {
                Rectangle tileRect = {j * SIZE, i * SIZE, SIZE, SIZE};
                if (CheckCollisionRecs(playerRectY, tileRect))
                {
                    if (p->velocity.y > 0)
                    {
                        p->position.y = tileRect.y - p->tamanho.y / 2.0f;
                        p->chao = true;
                    }
                    else if (p->velocity.y < 0)
                    {
                        p->position.y = tileRect.y + SIZE + p->tamanho.y / 2.0f;
                    }
                    p->velocity.y = 0;
                }
            }
        }
    }
}

void CheckObjectCollisions(player *p, map *ma[], int *curmap)
{
    Rectangle playerRect =
    {
        p->position.x - p->tamanho.x / 2.0f,
        p->position.y - p->tamanho.y / 2.0f,
        p->tamanho.x,
        p->tamanho.y
    };
    int startRow = floor(playerRect.y / SIZE);
    int endRow = floor((playerRect.y + playerRect.height - 1) / SIZE);
    int startCol = floor(playerRect.x / SIZE);
    int endCol = floor((playerRect.x + playerRect.width - 1) / SIZE);
    for (int i = startRow; i <= endRow; i++)
    {
        for (int j = startCol; j <= endCol; j++)
        {
            if (i < 0 || i >= ma[*curmap]->rows || j < 0 || j >= ma[*curmap]->cols) continue;
            char tileType = ma[*curmap]->m[i][j];
            Rectangle tileRect = {j * SIZE, i * SIZE, SIZE, SIZE};
            if (CheckCollisionRecs(playerRect, tileRect))
            {
                switch (tileType)
                {
                case 'A':
                    if (p->chao && IsKeyPressed(KEY_W))
                    {
                    }
                    break;
                case 'M':
                case 'C':
                    if (p->invtimer<= 0.0f)
                    {
                        p->vidas--;
                        p->invtimer = 1.0f;
                        p->position.x=-p->velocity.x;
                        p->position.y=-p->velocity.y;
                    }
                    break;
                case '1':
                    if (p->chao && IsKeyPressed(KEY_W))
                    {
                        initmap(*ma[1],p);
                        *curmap = 1;
                    }
                    break;
                case '2':
                    if (p->chao && IsKeyPressed(KEY_W))
                    {
                        initmap(*ma[2], p);
                        *curmap = 2;
                    }
                    break;
                case '3':
                    if (p->chao && IsKeyPressed(KEY_W))
                    {
                        initmap(*ma[3], p);
                        *curmap = 3;
                    }
                    break;
                case '4':
                    if (p->chao && IsKeyPressed(KEY_W))
                    {
                        initmap(*ma[4], p);
                        *curmap = 4;
                    }
                    break;
                case '5':
                    if (p->chao && IsKeyPressed(KEY_W))
                    {
                        initmap(*ma[5], p);
                        *curmap = 5;
                    }
                    break;
                }
            }
        }
    }
}


void updateplayer(player *p,int *s, map map[], int *curmap)
{
    if(IsKeyDown(KEY_D))
    {
        p->velocity.x = MOVE_SPEED;
    }
    else if(IsKeyDown(KEY_A))
    {
        p->velocity.x = -MOVE_SPEED;
    }
    else p->velocity.x=0;
    if (IsKeyPressed(KEY_SPACE)&&p->chao)
    {
        p->velocity.y = -JUMP_STRENGTH;
        p->chao = false;
    }
    if (IsKeyDown(KEY_S))
        p->velocity.y += MOVE_SPEED*0.5;
    if(IsKeyPressed(KEY_ESCAPE))
    {
        *s = 3;
    }
    if(IsKeyDown(KEY_O))
    {
        *s = 2;
    }
    p->velocity.y += GRAVITY;
    p->invtimer-=0.02;

    PlayerColi(p, map[*curmap]);
    CheckObjectCollisions(p, &map, curmap);
}

void updatemenu(int *c,int *s, bool *init)
{
    if(*c>=3||*c<0)
        *c=0;
    if(IsKeyPressed(KEY_W))
    {
        if(*c<=0)
        {
            *c=2;
        }
        else
            (*c)--;
    }
    if(IsKeyPressed(KEY_S))
    {
        if(*c>=2)
        {
            *c=0;
        }
        else
            (*c)++;
    }
    if(IsKeyPressed(KEY_ENTER)||IsKeyPressed(KEY_SPACE))
    {
        if(*c==0)
        {
            *init=true;
            *s=1;
        }
        if(*c==1)
            *s=0;
        if(*c==2)
            *s=4;
    }

}
void drawmenu(int s)
{

    DrawText("comecar jogo", 300, 20, 30, (s==0)?RED:GREEN);
    DrawText("controles?", 300, 40, 30, (s==1)?RED:GREEN);
    DrawText("sair do jogo", 300, 60, 30, (s==2)?RED:GREEN);

}
void updateoption(int *c,int *s)
{
    if(IsKeyPressed(KEY_W))
    {
        if(*c<=0)
        {
            *c=4;
        }
        else
            (*c)--;
    }
    if(IsKeyPressed(KEY_S))
    {
        if(*c>=4)
        {
            *c=0;
        }
        else
            (*c)++;
    }
    if(IsKeyPressed(KEY_ESCAPE))
        *s=1;
    if(IsKeyPressed(KEY_ENTER)||IsKeyPressed(KEY_SPACE))
    {
        if(*c==0)
            *s=1;
        if(*c==1)
            *s=6;
        if(*c==2)
            *s=5;
        if(*c==3)
            *s=0;
        if(*c==4)
            *s=4;

    }
}
void drawoption(int s)
{

    DrawText("continua:", 300, 20, 20, (s==0)?RED:GREEN);
    DrawText("reinicia", 300, 40, 20, (s==1)?RED:GREEN);
    DrawText("configuracoes", 300, 60, 20, (s==2)?RED:GREEN);
    DrawText("retornar ao menu", 300, 80, 20, (s==3)?RED:GREEN);
    DrawText("sair", 300, 100, 20, (s==4)?RED:GREEN);

}

void drawplayer(player *p,Camera2D camera)
{
    BeginMode2D(camera);
    DrawRectangle(p->position.x - 0.5 * p->tamanho.x, p->position.y - 0.5f * p->tamanho.y, p->tamanho.x, p->tamanho.y, RED);
    DrawText(TextFormat("vidas: %d", p->vidas), p->position.x-SCREEN_WIDTH+10, p->position.y-SCREEN_HEIGHT+10, 20, RED);
    EndMode2D();
}

int main()
{
    int cur_window=0,sel_window=0,nEn;
    bool init=false;
    map maps[6];
    player p1;
    initplayer(&p1);
    int cur_map = 0;
    SetTargetFPS(60);
    Camera2D camera = {0};
    camera.target = p1.position;
    camera.zoom=0.5;
    camera.offset = (Vector2)
    {
        SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2
    };
    InitWindow(SCREEN_WIDTH,SCREEN_HEIGHT,"jogo");
    SetExitKey(KEY_HOME);
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        if(init)
        {
            loadmaps(maps);
            nEn=initmap(maps[cur_map],&p1);
            initplayer(&p1);
            init=false;
        }
        BeginDrawing();
        ClearBackground(PURPLE);
        switch (cur_window)
        {
        case 0:
            updatemenu(&sel_window,&cur_window,&init);
            drawmenu(sel_window);

            break;
        case 1:
            updateplayer(&p1,&cur_window, maps,&cur_map);
            camera.target.x = (p1.position.x < SCREEN_WIDTH/2.0f) ? SCREEN_WIDTH/2.0f : p1.position.x;
            camera.target.y = (p1.position.y < SCREEN_HEIGHT/2.0f) ? SCREEN_HEIGHT/2.0f : p1.position.y;
            drawmap(maps[cur_map], &p1, camera);
            drawplayer(&p1,camera);
            break;
        case 2:
            DrawText("INVENTARIO",SCREEN_WIDTH/2-20,SCREEN_HEIGHT/2-20,40,GREEN);
            if(IsKeyPressed(KEY_ESCAPE))
                cur_window=1;
            break;
        case 3:
            updateoption(&sel_window,&cur_window);
            drawoption(sel_window);
            break;

        case 4:
            EndDrawing();
            CloseWindow();

        case 5:
            DrawText("CONFIGS",SCREEN_WIDTH/2,SCREEN_HEIGHT/2,40,GREEN);
            if(IsKeyPressed(KEY_ESCAPE))
                cur_window=3;
            break;
        case 6:
            init=true;
            cur_window=1;
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
