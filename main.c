#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_HEIGHT 900
#define SCREEN_WIDTH 1600
#define WIDTH_LIMIT SCREEN_WIDTH*15
#define GRAVITY 2000.0f
#define MOVE_SPEED 1500.0f
#define JUMP_STRENGTH 1500.0f
#define SIZE 128
#define MAX_ROWS 16
#define MAX_COLS 160
#define MAX_ENEMIES 50
#define MAX_ENERGY 10
#define ANIM_IDLE 0
#define ANIM_RUN 1
#define ANIM_JUMP 2
#define ANIM_ATTACK 3
#define MAX_ANIMS 4
#define MAX_SKILLS 4
#define MAX_AMULET 6
#define NUM_MAPS 4
#define DASHSPEED 30
typedef struct
{
    Texture2D tex;
    int numFrames;
} Sprite;
typedef struct
{
    Vector2 position;
    Vector2 velocity;
    int vidas,maxlife;
    bool amuletos[6];
    bool ameq[6];
    Vector2 tamanho;
    bool chao;
    float invtimer;
    bool inbench;
    int coins;
    bool habilidades[MAX_SKILLS];
    bool hab_owned[MAX_SKILLS];
    int energy,energyincrease;
    float dashDuration;
    float dashCooldown;
    bool isDashing;
    int cur_an;
    bool podePuloDuplo;
    int facing;
    float attackTimer;
    bool t;
    float attackCooldown;
    float hitbox_size;
    float speed_mult;
    int attackDir;
    Sprite sprites[MAX_ANIMS];
    int currentAnim;
    int currentFrame;
    float frameTimer;
    float frameSpeed;
    char notificacao[100];
    float notifTimer;
    float healhold;
    bool beat[6], cheat;
} player;
typedef struct
{
    char m[MAX_ROWS][MAX_COLS+1];
    int rows,cols;
    bool shopen;
    int nen;
} map;
typedef struct
{
    Texture2D parede;
    Texture2D player;
    Texture2D fundo;
} GameAssets;
typedef struct
{
    Rectangle rect;
    float speed;
    int direction;
    bool active;
    int hp;
    float hitTimer;
    int type;
    int dirY;
    float originX;
    int state;
    float abilityTimer;
    float velY;
} Enemy;
void LoadPlayerAnims(player *p)
{
    p->sprites[ANIM_IDLE].tex = LoadTexture("texturas/IDLE.png");
    p->sprites[ANIM_IDLE].numFrames = 7;
    p->sprites[ANIM_RUN].tex = LoadTexture("texturas/RUN.png");
    p->sprites[ANIM_RUN].numFrames = 8;
    p->sprites[ANIM_JUMP].tex = LoadTexture("texturas/JUMP.png");
    p->sprites[ANIM_JUMP].numFrames = 5;
    p->sprites[ANIM_ATTACK].tex = LoadTexture("texturas/ATTACK 1.png");
    p->sprites[ANIM_ATTACK].numFrames = 6;
    p->currentAnim = ANIM_IDLE;
    p->currentFrame = 0;
    p->frameTimer = 0.0f;
    p->frameSpeed = 0.1f;
}
void UpdatePlayerAnimation(player *p)
{
    int novaAnim = p->currentAnim;
    if (p->attackTimer > 0)
    {
        novaAnim = ANIM_ATTACK;
        p->frameSpeed = 0.06f;
    }
    else if (!p->chao)
    {
        novaAnim = ANIM_JUMP;
        p->frameSpeed = 0.15f;
    }
    else if (fabs(p->velocity.x) > 10)
    {
        novaAnim = ANIM_RUN;
        p->frameSpeed = 0.1f;
    }
    else
    {
        novaAnim = ANIM_IDLE;
        p->frameSpeed = 0.15f;
    }
    if (novaAnim != p->currentAnim)
    {
        p->currentAnim = novaAnim;
        p->currentFrame = 0;
        p->frameTimer = 0.0f;
    }
    p->frameTimer += GetFrameTime();
    if (p->frameTimer >= p->frameSpeed)
    {
        p->frameTimer = 0.0f;
        if (p->currentAnim == ANIM_JUMP && p->currentFrame == p->sprites[ANIM_JUMP].numFrames - 1)
        {
        }
        else
        {
            p->currentFrame++;
        }
        if (p->currentFrame >= p->sprites[p->currentAnim].numFrames)
        {
            if (p->currentAnim != ANIM_JUMP)
            {
                p->currentFrame = 0;
            }
            else
            {
                p->currentFrame = p->sprites[p->currentAnim].numFrames - 1;
            }
        }
    }
}
void DrawHUD(player *p)
{
    DrawText(TextFormat("VIDA: %d / %d", p->vidas, p->maxlife), 20, 20, 30, RED);
    DrawText(TextFormat("ENERGIA: %d / %d", p->energy, MAX_ENERGY), 20, 60, 20, PURPLE);
    DrawText(TextFormat("MOEDAS: %d", p->coins), 20, 90, 20, GOLD);
    if(p->cheat) DrawText("CHEAT ATIVADO", 50, 120, 20, GREEN);
    if (p->notifTimer > 0)
    {
        p->notifTimer -= GetFrameTime();
        DrawRectangle(SCREEN_WIDTH/2 - 400, 150, 800, 100, (Color){ 0, 0, 0, 200 });
        DrawRectangleLines(SCREEN_WIDTH/2 - 400, 150, 800, 100, WHITE);
        int textWidth = MeasureText(p->notificacao, 20);
        DrawText(p->notificacao, SCREEN_WIDTH/2 - 380, 170, 20, WHITE);
    }
}
void initplayer(player *p)
{
    p->velocity.x=00.0f;
    p->velocity.y=00.0f;
    p->tamanho.x=50.0f;
    p->tamanho.y=110.0f;
    p->chao=false;
    p->maxlife=5;
    p->vidas = p->maxlife;
    p->invtimer = 0;
    p->inbench = false;
    p->coins = 0;
    p->energy = 0;
    p->energyincrease = 2;
    p->cheat=false;
    p->healhold=0;
    p->speed_mult=1;
    p->notifTimer = 0;
    strcpy(p->notificacao, "");
    for(int i = 0; i<MAX_AMULET; i++)
    {
        p->amuletos[i]=false;
        p->ameq[i]=false;
        p->beat[i]=false;
    }
    for(int i = 0; i<MAX_SKILLS; i++)
    {
        p->habilidades[i]=false;
        p->hab_owned[i]=false;
    }
    p->facing = 1;
    p->dashDuration = 0.0f;
    p->dashCooldown = 0.0f;
    p->isDashing = false;
    p->podePuloDuplo = false;
    p->attackTimer = 0;
    p->hitbox_size=1;
    p->attackCooldown = 0;
    p->attackDir = 0;
}
void floodfill(map *ma, int i, int j, char mark)
{
    if (i < 0 || i >= ma->rows || j < 0 || j >= ma->cols-1) return;
    if (ma->m[i][j] != ' ' ) return;
    if (ma->m[i][j] == mark) return;
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
Texture2D carregaTexture(char *arquivo)
{
    Texture2D textura = LoadTexture(arquivo);
    return textura;
}
void loadmap(map *mapa, char *filename)
{
    for (int i = 0; i < MAX_ROWS; i++)
    {
        for (int j = 0; j < MAX_COLS; j++)
        {
            mapa->m[i][j] = ' ';
        }
        mapa->m[i][MAX_COLS] = '\0';
    }
    FILE *fmap = fopen(filename, "r");
    mapa->rows = 0;
    mapa->cols = 0;
    for (int i = 0; i < MAX_ROWS; i++)
    {
        if (fgets(mapa->m[i], MAX_COLS, fmap) == NULL) break;
        mapa->rows++;
        int len = strlen(mapa->m[i]);
        if (mapa->m[i][len - 1] == '\n') mapa->m[i][len - 1] = '\0';
        if (len > mapa->cols) mapa->cols = len;
    }
    mapa->cols-=1;
    markexterior(mapa);
    fclose(fmap);
    mapa->shopen=false;
}
void loadmaps (map *maps, int n)
{
    switch (n)
    {
    case 0:
        loadmap(maps,"maps/lobby.txt");
        break;
    case 1:
        loadmap(maps,"maps/maptest.txt");
        break;
    case 2:
        loadmap(maps,"maps/mapa2.txt");
        break;
    case 3:
        loadmap(maps,"maps/mapa3.txt");
        break;
    case 4:
        loadmap(maps,"maps/mapa4.txt");
    }
}
void drawmap(map ma, player *p, Camera2D camera, GameAssets assets)
{
    Rectangle sourceFundo = { 0, 0, (float)assets.fundo.width, (float)assets.fundo.height };
    Rectangle destFundo = { 0, 0, (float)(ma.cols * SIZE), (float)(ma.rows * SIZE) };
    DrawTexturePro(assets.fundo, sourceFundo, destFundo, (Vector2){ 0, 0 }, 0.0f, WHITE);
    char temp;
    float xsize=SIZE;
    float ysize=SIZE;
    for(int i = 0; i<ma.rows; i++)
    {
        for(int j = 0; j<ma.cols; j++)
        {
            temp=ma.m[i][j];
            switch(temp)
            {
            case 'J': break;
            case 'P':
            {
                Rectangle source = { 0, 0, (float)assets.parede.width, (float)assets.parede.height };
                Rectangle dest = { (float)(xsize*j), (float)(ysize*i), (float)xsize, (float)ysize };
                DrawTexturePro(assets.parede, source, dest, (Vector2){ 0,0 }, 0.0f, WHITE);
                break;
            }
            case 'M':
                DrawRectangle(xsize*j,ysize*i,xsize,ysize,YELLOW);
                break;
            case 'A':
                DrawRectangle(xsize*j, ysize*i, xsize, ysize, SKYBLUE);
                DrawCircle(xsize*(0.5+j), ysize*(0.5+i), xsize/3, WHITE);
                DrawText("BUFF", (xsize*j)+20, (ysize*i)+20, 20, BLACK);
                break;
            case 'C': break;
            case 'H':
                DrawRectangle(xsize*j, ysize*i, xsize, ysize, LIME);
                DrawText("SKILL", (xsize*j)+20, (ysize*i)+20, 20, BLACK);
                break;
            case 'E':
                DrawRectangle(xsize*j, ysize*i, xsize, ysize, BLACK);
                break;
            case 'S':
                DrawRectangle(xsize*j, ysize*i, xsize, ysize, (!ma.shopen ? GOLD : GREEN));
                DrawText("LOJA", (xsize*j)+30, (ysize*i)+30, 30, BLACK);
                break;
            case 'B':
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2, (p->inbench ? PINK : LIME));
                DrawText("BANCO", (xsize*j)+20, (ysize*i)+40, 25, BLACK);
                break;
            case 'X':
                DrawCircle(xsize*(0.5+j), ysize*(0.5+i), xsize/2, PURPLE);
                DrawCircleLines(xsize*(0.5+j), ysize*(0.5+i), xsize/2 - 10, WHITE);
                DrawText("EXIT", (xsize*j)+40, (ysize*i)+50, 20, WHITE);
                break;
            case 'K':
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,DARKPURPLE);
                DrawText("coin",xsize*(0.4+j), ysize*(0.4+i), 60, GOLD);
                break;
            case '1':
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,DARKPURPLE);
                DrawText("1",xsize*(0.4+j), ysize*(0.4+i), 60, GREEN);
                break;
            case '2':
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,DARKPURPLE);
                DrawText("2",xsize*(0.4+j), ysize*(0.4+i), 60, (p->beat[1]?GREEN:RED));
                break;
            case '3':
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,DARKPURPLE);
                DrawText("3",xsize*(0.4+j), ysize*(0.4+i), 60, (p->beat[2]?GREEN:RED));
                break;
            case '4':
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,DARKPURPLE);
                DrawText("4",xsize*(0.4+j), ysize*(0.4+i), 60, (p->beat[3]?GREEN:RED));
                break;
            case '5':
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,DARKPURPLE);
                DrawText("5",xsize*(0.4+j), ysize*(0.4+i), 60, (p->beat[4]?GREEN:RED));
                break;
            default: break;
            }
        }
    }
}
int initmap(map *ma, player *p, Enemy enemies[])
{
    char temp;
    int count = 0;
    for(int k=0; k<MAX_ENEMIES; k++) enemies[k].active = false;
    for(int i = 0; i < ma->rows; i++)
    {
        for(int j = 0; j < ma->cols; j++)
        {
            temp = ma->m[i][j];
            if(temp == 'J')
            {
                p->position.x = j * SIZE + SIZE/2;
                p->position.y = i * SIZE + SIZE/2;
            }
            else if(temp == 'M')
            {
                if(count < MAX_ENEMIES)
                {
                    enemies[count].rect.x = j * SIZE;
                    enemies[count].rect.y = i * SIZE;
                    enemies[count].rect.width = SIZE;
                    enemies[count].rect.height = SIZE;
                    enemies[count].speed = 5.0f;
                    enemies[count].active = true;
                    enemies[count].hitTimer = 0;
                    if (count == 0) enemies[count].type = GetRandomValue(0, 1);
                    else if (count == 1) enemies[count].type = (enemies[0].type == 0) ? 1 : 0;
                    else enemies[count].type = GetRandomValue(0, 1);
                    if(enemies[count].type == 0)
                    {
                        enemies[count].hp = 4;
                        enemies[count].direction = 1;
                        enemies[count].dirY = 0;
                    }
                    else
                    {
                        enemies[count].hp = 2;
                        enemies[count].direction = (GetRandomValue(0,1) == 0) ? 1 : -1;
                        enemies[count].dirY = (GetRandomValue(0,1) == 0) ? 1 : -1;
                    }
                    ma->m[i][j] = ' ';
                    count++;
                }
            }
            else if(temp == 'C')
            {
                if(count < MAX_ENEMIES)
                {
                    enemies[count].rect.x = j * SIZE;
                    enemies[count].rect.y = i * SIZE;
                    enemies[count].rect.width = SIZE * 1.2f;
                    enemies[count].rect.height = SIZE * 1.2f;
                    enemies[count].speed = 7.5f;
                    enemies[count].state = 0;
                    enemies[count].abilityTimer = 3.0f;
                    enemies[count].active = true;
                    enemies[count].velY = 0;
                    enemies[count].hitTimer = 0;
                    enemies[count].type = 2;
                    enemies[count].hp = 16;
                    enemies[count].originX = enemies[count].rect.x;
                    ma->m[i][j] = ' ';
                    count++;
                }
            }
        }
    }
    return count;
}
bool IsSolid(char tile)
{
    if (tile == 'P') return true;
    return false;
}
void PlayerColi(player *p, map ma)
{
    float dt = GetFrameTime();
    p->position.x += p->velocity.x * dt;
    Rectangle playerRectX =
    {
        p->position.x - p->tamanho.x / 2.0f,
        p->position.y - p->tamanho.y / 2.0f,
        p->tamanho.x,
        p->tamanho.y
    };
    int startRow = floor(playerRectX.y / SIZE);
    int endRow = floor((playerRectX.y + playerRectX.height) / SIZE);
    int startCol = floor(playerRectX.x / SIZE);
    int endCol = floor((playerRectX.x + playerRectX.width) / SIZE);
    for (int i = startRow; i <= endRow; i++)
    {
        for (int j = startCol; j <= endCol; j++)
        {
            if (i < 0 || i >= ma.rows || j < 0 || j >= ma.cols) continue;
            if (IsSolid(ma.m[i][j]))
            {
                Rectangle tileRect = {j * SIZE, i * SIZE, SIZE, SIZE};
                if (CheckCollisionRecs(playerRectX, tileRect))
                {
                    if (p->velocity.x > 0) p->position.x = tileRect.x - p->tamanho.x / 2.0f;
                    else if (p->velocity.x < 0) p->position.x = tileRect.x + SIZE + p->tamanho.x / 2.0f;
                    p->velocity.x = 0;
                }
            }
        }
    }
    p->position.y += p->velocity.y * dt;
    Rectangle playerRectY =
    {
        p->position.x - p->tamanho.x / 2.0f,
        p->position.y - p->tamanho.y / 2.0f,
        p->tamanho.x,
        p->tamanho.y
    };
    p->chao = false;
    startRow = floor(playerRectY.y / SIZE);
    endRow = floor((playerRectY.y + playerRectY.height) / SIZE);
    startCol = floor(playerRectY.x / SIZE);
    endCol = floor((playerRectY.x + playerRectY.width) / SIZE);
    for (int i = startRow; i <= endRow; i++)
    {
        for (int j = startCol; j <= endCol; j++)
        {
            if (i < 0 || i >= ma.rows || j < 0 || j >= ma.cols) continue;
            if (IsSolid(ma.m[i][j]))
            {
                Rectangle tileRect = {j * SIZE, i * SIZE, SIZE, SIZE};
                if (CheckCollisionRecs(playerRectY, tileRect))
                {
                    if (p->velocity.y > 0)
                    {
                        p->position.y = tileRect.y - p->tamanho.y / 2.0f;
                        p->chao = true;
                        p->velocity.y = 0;
                        p->podePuloDuplo = true;
                    }
                    else if (p->velocity.y < 0)
                    {
                        p->position.y = tileRect.y + SIZE + p->tamanho.y / 2.0f;
                        p->velocity.y = 0;
                    }
                }
            }
        }
    }
}
void CheckObjectCollisions(player *p, map *ma, int *curmap, bool *init)
{
    Rectangle sensor =
    {
        p->position.x - (p->tamanho.x / 2.0f) - 10,
        p->position.y - (p->tamanho.y / 2.0f) - 10,
        p->tamanho.x + 20,
        p->tamanho.y + 20
    };
    int startRow = (int)(sensor.y / SIZE);
    int endRow = (int)((sensor.y + sensor.height) / SIZE);
    int startCol = (int)(sensor.x / SIZE);
    int endCol = (int)((sensor.x + sensor.width) / SIZE);
    for (int i = startRow; i <= endRow; i++)
    {
        for (int j = startCol; j <= endCol; j++)
        {
            if (i < 0 || i >= ma->rows || j < 0 || j >= ma->cols) continue;
            char tileType = ma->m[i][j];
            Rectangle tileRect = {j * SIZE, i * SIZE, SIZE, SIZE};
            if (CheckCollisionRecs(sensor, tileRect))
            {
                if(tileType == 'S' || tileType == 'B' || (tileType >= '1' && tileType <= '5'))
                {
                    DrawRectangleRec(tileRect, (Color){ 0, 255, 0, 50 });
                    DrawText("APERTE W", p->position.x - 40, p->position.y - 100, 20, GREEN);
                }
                switch (tileType)
                {
                case 'A':
                    if (IsKeyPressed(KEY_W))
                    {
                        if(*curmap==1)p->amuletos[0]=true;
                        if(*curmap==2)p->amuletos[1]=true;
                        if(*curmap==3)p->amuletos[5]=true;
                        ma->m[i][j] = ' ';
                        strcpy(p->notificacao, "NOVO AMULETO: EQUIPE NO BANCO");
                        p->notifTimer = 5.0f;
                    }
                    break;
                case 'H':
                    if (IsKeyPressed(KEY_W))
                    {
                        if(*curmap==1){p->habilidades[0] = true; p->hab_owned[0]=true;}
                        if(*curmap==4){p->habilidades[2] = true;p->hab_owned[2]=true;}
                        ma->m[i][j] = ' ';
                        strcpy(p->notificacao, "NOVA HABILIDADE: ATAQUE ESPECIAL");
                        p->notifTimer = 5.0f;
                    }
                    break;
                case 'S':
                    if (IsKeyPressed(KEY_W)) { ma->shopen = true; p->velocity.x = 0; }
                    break;
                case 'K':
                    p->coins+=12;
                    ma->m[i][j] = ' ';
                    break;
                case 'B':
                    if(!p->inbench && IsKeyPressed(KEY_W))
                    {
                        p->inbench = true;
                        p->vidas = p->maxlife;
                        p->velocity.x = 0;
                        strcpy(p->notificacao, "DESCANSO");
                        p->notifTimer = 2.0f;
                    }
                    break;
                case '1': if (IsKeyPressed(KEY_W)) { *init = true; *curmap = 1; } break;
                case '2':
                    if (IsKeyPressed(KEY_W)) {
                        if(p->beat[1]) {*init = true; *curmap = 2;}
                        else {strcpy(p->notificacao, "BLOQUEADO: Venca o Mapa 1"); p->notifTimer=2;}
                    } break;
                case '3':
                    if (IsKeyPressed(KEY_W)) {
                        if(p->beat[2]) {*init = true; *curmap = 3;}
                        else {strcpy(p->notificacao, "BLOQUEADO: Venca o Mapa 2"); p->notifTimer=2;}
                    } break;
                case '4':
                    if (IsKeyPressed(KEY_W)) {
                        if(p->beat[3]) {*init = true; *curmap = 4;}
                        else {strcpy(p->notificacao, "BLOQUEADO: Venca o Mapa 3"); p->notifTimer=2;}
                    } break;
                case '5':
                    if (IsKeyPressed(KEY_W)) {
                        if(p->beat[4]) {*init = true; *curmap = 5;}
                        else {strcpy(p->notificacao, "BLOQUEADO: Venca o Mapa 4"); p->notifTimer=2;}
                    } break;
                case 'X':
                    if (IsKeyPressed(KEY_W)) { *init = true; *curmap = 0; }
                    break;
                case 'C':
                    if (p->invtimer <= 0)
                    {
                        p->vidas--;
                        p->invtimer = 1.0f;
                        if (p->position.x < tileRect.x + tileRect.width / 2.0f) p->velocity.x = -200;
                        else p->velocity.x = 200;
                        p->velocity.y = -200;
                    }
                    break;
                }
            }
        }
    }
}
void updateplayer(player *p, int *s, map *map, int *curmap, bool *init)
{
    float dt = GetFrameTime();
    if (map->shopen)
    {
        p->velocity.x = 0;
        return;
    }
    if (p->dashCooldown > 0) p->dashCooldown -= dt;
    if (p->isDashing)
    {
        p->dashDuration -= dt;
        p->velocity.x = p->facing * 1500.0f;
        p->velocity.y = 0;
        if (p->dashDuration <= 0)
        {
            p->isDashing = false;
            p->velocity.x = 0;
        }
        PlayerColi(p, *map);
        CheckObjectCollisions(p, map, curmap, init);
        return;
    }
    if (p->habilidades[3] && p->dashCooldown <= 0 && IsKeyDown(KEY_Z))
    {
        p->isDashing = true;
        p->dashDuration = 0.3f;
        p->dashCooldown = 2.0f;
    }

    if(p->ameq[5]) p->speed_mult=1.4;
    else p->speed_mult=1;
    if(IsKeyDown(KEY_LEFT_SHIFT)&&p->ameq[0]) p->speed_mult*=1.6;
    if(IsKeyDown(KEY_D))
    {
        p->velocity.x = MOVE_SPEED*p->speed_mult;
        p->facing = 1;
    }
    else if(IsKeyDown(KEY_A))
    {
        p->velocity.x = -MOVE_SPEED*p->speed_mult;
        p->facing = -1;
    }
    else p->velocity.x=0;
    if(IsKeyDown(KEY_V)&&p->energy>=5&&p->vidas!=p->maxlife&&p->habilidades[0]){
        p->healhold+=GetFrameTime();
        if(p->healhold>0.4){
            p->vidas=(p->vidas+4>p->maxlife)?p->maxlife:p->vidas+4;
            p->healhold=0;
            p->energy-=5;
        }
    }
    else p->healhold=0;
    if(IsKeyDown(KEY_S)&&p->t)
    {
        p->t=false;
        p->velocity.y*=0.1;
    }
    if (IsKeyPressed(KEY_SPACE))
    {
        if(p->chao)
        {
            p->velocity.y = -JUMP_STRENGTH;
            p->chao = false;
            p->t = true;
            p->podePuloDuplo = true;
        }
        else if (p->habilidades[2] && p->podePuloDuplo)
        {
            p->velocity.y = -JUMP_STRENGTH;
            p->podePuloDuplo = false;
            p->currentAnim = ANIM_JUMP;
            p->currentFrame = 0;
        }
    }
    if(IsKeyPressed(KEY_C))p->cheat=!p->cheat;
    if(IsKeyPressed(KEY_ESCAPE)) *s = 3;
    if(IsKeyDown(KEY_O)) *s = 2;
    if(p->inbench)
    {

        if(IsKeyDown(KEY_A) || IsKeyDown(KEY_D) || IsKeyPressed(KEY_SPACE)) p->inbench = false;
    }
    if(map->shopen)
    {
        if(IsKeyDown(KEY_A) || IsKeyDown(KEY_D) || IsKeyPressed(KEY_SPACE)) map->shopen = false;
    }
    if(IsKeyDown(KEY_L)) p->vidas=0;
    p->velocity.y += GRAVITY * dt;
    if(p->invtimer>0) p->invtimer-=dt;
    PlayerColi(p, *map);
    CheckObjectCollisions(p, map, curmap, init);
    if(p->vidas<=0) *s=8;
}
void UpdateEnemies(Enemy enemies[], int enemyCount, map ma, player p)
{
    for(int i = 0; i < enemyCount; i++)
    {
        if(!enemies[i].active) continue;
        if(enemies[i].hitTimer > 0) enemies[i].hitTimer -= 0.02f;
        if (enemies[i].type == 0)
        {
            enemies[i].rect.x += enemies[i].speed * enemies[i].direction;
            float checkX = (enemies[i].direction == 1) ? enemies[i].rect.x + enemies[i].rect.width + 5 : enemies[i].rect.x - 5;
            int col = (int)(checkX / SIZE);
            int row = (int)((enemies[i].rect.y + SIZE/2) / SIZE);
            if (col < 0 || col >= ma.cols || ma.m[row][col] == 'P'|| ma.m[row][col] == '-') enemies[i].direction *= -1;
            else
            {
                int rowBelow = (int)((enemies[i].rect.y + SIZE + 5) / SIZE);
                if(rowBelow < ma.rows && ma.m[rowBelow][col] == ' ') enemies[i].direction *= -1;
            }
        }
        else if (enemies[i].type == 1)
        {
            enemies[i].rect.x += enemies[i].speed * enemies[i].direction;
            float checkX = (enemies[i].direction == 1) ? enemies[i].rect.x + enemies[i].rect.width + 5 : enemies[i].rect.x - 5;
            int colX = (int)(checkX / SIZE);
            int rowCenter = (int)((enemies[i].rect.y + SIZE/2) / SIZE);
            if (colX < 0 || colX >= ma.cols || ma.m[rowCenter][colX] == 'P' ||  ma.m[rowCenter][colX] == '-' ) enemies[i].direction *= -1;
            enemies[i].rect.y += enemies[i].speed * enemies[i].dirY;
            float checkY = (enemies[i].dirY == 1) ? enemies[i].rect.y + enemies[i].rect.height + 5 : enemies[i].rect.y - 5;
            int rowY = (int)(checkY / SIZE);
            int colCenter = (int)((enemies[i].rect.x + SIZE/2) / SIZE);
            if (rowY < 0 || rowY >= ma.rows || ma.m[rowY][colCenter] == 'P'||  ma.m[rowY][colCenter] == '-') enemies[i].dirY *= -1;
        }
        else if (enemies[i].type == 2)
        {
            if (enemies[i].abilityTimer > 0) enemies[i].abilityTimer -= 0.02f;
            float playerX = p.position.x;
            float bossCenter = enemies[i].rect.x + enemies[i].rect.width / 2;
            if (enemies[i].state == 0)
            {
                if (enemies[i].abilityTimer <= 0)
                {
                    enemies[i].state = 1;
                    enemies[i].abilityTimer = 0.6f;
                    enemies[i].direction = (playerX > bossCenter) ? 1 : -1;
                }
                else
                {
                    float distPlayerToOrigin = fabs(playerX - enemies[i].originX);
                    float alvoX = (distPlayerToOrigin <= 1500) ? playerX : enemies[i].originX;
                    float distToTarget = fabs(alvoX - bossCenter);
                    if(distToTarget > 10)
                    {
                        int dir = (alvoX > bossCenter) ? 1 : -1;
                        enemies[i].rect.x += enemies[i].speed * dir;
                        float checkX = (dir == 1) ? enemies[i].rect.x + enemies[i].rect.width : enemies[i].rect.x;
                        int col = (int)(checkX / SIZE);
                        int row = (int)((enemies[i].rect.y + enemies[i].rect.height/2) / SIZE);
                        if (col >= 0 && col < ma.cols && ma.m[row][col] == 'P'||  ma.m[row][col] == '-')
                            enemies[i].rect.x -= enemies[i].speed * dir;
                    }
                }
            }
            else if (enemies[i].state == 1)
            {
                if (enemies[i].abilityTimer <= 0)
                {
                    enemies[i].state = 2;
                    enemies[i].abilityTimer = 0.8f;
                }
            }
            else if (enemies[i].state == 2)
            {

                enemies[i].rect.x += DASHSPEED * enemies[i].direction;
                float checkX = (enemies[i].direction == 1) ? enemies[i].rect.x + enemies[i].rect.width : enemies[i].rect.x;
                int col = (int)(checkX / SIZE);
                int row = (int)((enemies[i].rect.y + enemies[i].rect.height/2) / SIZE);
                if (col >= 0 && col < ma.cols && (ma.m[row][col] == 'P'||ma.m[row][col] == '-'))
                {
                    enemies[i].rect.x -= DASHSPEED * enemies[i].direction;
                    enemies[i].abilityTimer = 0;
                }
                if (enemies[i].abilityTimer <= 0)
                {
                    enemies[i].state = 0;
                    enemies[i].abilityTimer = 2.5f;
                }
            }
            enemies[i].rect.y += 15.0f;
            int rowFeet = (int)((enemies[i].rect.y + enemies[i].rect.height) / SIZE);
            int colCenter = (int)((enemies[i].rect.x + enemies[i].rect.width/2) / SIZE);
            if (rowFeet < ma.rows && ma.m[rowFeet][colCenter] == 'P')
            {
                enemies[i].rect.y = (rowFeet * SIZE) - enemies[i].rect.height;
            }
        }
    }
}
void DrawEnemies(Enemy enemies[], int enemyCount)
{
    for(int i = 0; i < enemyCount; i++)
    {
        if(enemies[i].active)
        {
            Color cor;
            if(enemies[i].type == 1) cor = ORANGE;
            else if(enemies[i].type == 2)
            {
                if (enemies[i].state == 1) cor = RED;
                else if (enemies[i].state == 2) cor = PURPLE;
                else cor = PINK;
            }
            else cor = YELLOW;
            if(enemies[i].hitTimer > 0) cor = WHITE;
            if(enemies[i].type == 2)
            {
                DrawCircle(enemies[i].rect.x + enemies[i].rect.width/2,
                           enemies[i].rect.y + enemies[i].rect.height/2,
                           enemies[i].rect.width/2, cor);
            }
            else
            {
                DrawRectangleRec(enemies[i].rect, cor);
            }
        }
    }
}
void CheckEnemyCollisions(player *p, Enemy enemies[], int enemyCount)
{
    if (p->invtimer > 0) return;
    Rectangle playerRect =
    {
        p->position.x - p->tamanho.x / 2.0f,
        p->position.y - p->tamanho.y / 2.0f,
        p->tamanho.x,
        p->tamanho.y
    };
    for(int i = 0; i < enemyCount; i++)
    {
        if(!enemies[i].active) continue;
        if (CheckCollisionRecs(playerRect, enemies[i].rect))
        {
            if(enemies[i].type == 2) p->vidas -= 2;
            else p->vidas -= 1;
            p->invtimer = 1.0f;
            if (p->position.x < enemies[i].rect.x + enemies[i].rect.width/2)
                p->velocity.x = -200;
            else
                p->velocity.x = 200;
            p->velocity.y = -200;
            break;
        }
    }
}
void PlayerAttack(player *p, Enemy enemies[], int enemyCount, Camera2D cam, int *cur_map, bool *init, map *ma)
{
    if (ma->shopen) return;
    if(p->attackCooldown > 0) p->attackCooldown -= 0.02f;
    if(p->attackTimer > 0) p->attackTimer -= 0.02f;
    bool ataqueNormal = (IsKeyPressed(KEY_J) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
    bool ataqueEspecial = IsKeyPressed(KEY_F);
    int danoCausado = 0;
    bool gastouEnergia = false;
    if (p->attackCooldown <= 0)
    {
        if (ataqueEspecial && p->habilidades[1])
        {
            if (p->energy >= 5)
            {
                p->energy -= 5;
                p->attackTimer = 0.3f;
                p->attackCooldown = 0.6f;
                danoCausado = 4;
                gastouEnergia = true;
                if (IsKeyDown(KEY_W)) p->attackDir = 1;
                else if (IsKeyDown(KEY_S) && !p->chao) p->attackDir = 2;
                else p->attackDir = 0;
            }
        }
        else if (ataqueNormal)
        {
            p->attackTimer = 0.2f;
            p->attackCooldown = 0.4f;
            danoCausado = 1;

            {
                if (IsKeyDown(KEY_W)) p->attackDir = 1;
                else if (IsKeyDown(KEY_S) && !p->chao) p->attackDir = 2;
                else p->attackDir = 0;
            }
        }
        if(p->ameq[3])danoCausado*=2;
        if(p->cheat) danoCausado=1000;
    }
    if(p->attackTimer > 0 && danoCausado > 0)
    {
        Rectangle attackRect;
        if(p->attackDir == 1)
        {
            attackRect.width = 120;
            attackRect.height = 160;
            attackRect.x = p->position.x - attackRect.width/2;
            attackRect.y = p->position.y - p->tamanho.y/2 - attackRect.height;
        }
        else if(p->attackDir == 2)
        {
            attackRect.width = 120;
            attackRect.height = 160 ;
            attackRect.x = p->position.x - attackRect.width/2;
            attackRect.y = p->position.y + p->tamanho.y/2;
        }
        else
        {
            attackRect.width = 160 ;
            attackRect.height = 120;
            attackRect.y = p->position.y - 40;
            if(p->facing == 1) attackRect.x = p->position.x + p->tamanho.x/2;
            else attackRect.x = p->position.x - p->tamanho.x/2 - attackRect.width;
        }
        for(int i = 0; i < enemyCount; i++)
        {
            if(enemies[i].active)
            {
                if(CheckCollisionRecs(attackRect, enemies[i].rect) && enemies[i].hitTimer <= 0)
                {
                    enemies[i].hp -= danoCausado;
                    enemies[i].hitTimer = 0.5f;
                    p->invtimer = 0.2f;
                    if (!gastouEnergia)
                    {
                        int ganho = p->energyincrease;
                        p->energy += ganho;
                        if(p->energy > MAX_ENERGY) p->energy = MAX_ENERGY;
                    }
                    if(p->attackDir == 2) p->velocity.y = -JUMP_STRENGTH * 0.8f;
                    else if(p->attackDir != 1) p->velocity.x = (p->facing == 1) ? -10 : 10;
                    srand(time(NULL));
                    if(enemies[i].hp <= 0)
                    {
                        enemies[i].active = false;
                        p->coins += 10 + rand()%5;
                        if(enemies[i].type == 2)
                        {
                            p->coins += 1000;
                            p->beat[*cur_map] = true;

                            int gridX = (int)(enemies[i].rect.x / SIZE);
                            int gridY = (int)(enemies[i].rect.y / SIZE);
                            if(gridX >= 0 && gridX < ma->cols && gridY >= 0 && gridY < ma->rows)
                            {
                                ma->m[gridY][gridX] = 'X';
                            }
                        }
                    }
                }
            }
        }
    }
}
bool updateshop(player *p, int *c)
{
    typedef struct
    {
        const char* nome;
        const char* desc;
        int preco;
        int tipo;
        int id_real;
    } ShopItem;
    int totalItems = 5;
    ShopItem itens[] =
    {
        { "Amuleto de Vida",    "+3 Vidas",              20,  0, 2 },
        { "Pedra de Dano",      "Dobra seu dano",        30,  0, 3 },
        { "Slot Extra",         "Mais um amuleto",       500, 0, 4 },
        { "Manto de Dash",      "Aperte K/Dir",          25,  1, 3 },
        { "Ataque especial",    "Aperte f para um ataque poderoso", 65,  1, 1 }
    };

    if (IsKeyPressed(KEY_W)) (*c)--;
    if (IsKeyPressed(KEY_S)) (*c)++;
    if (*c < 0) *c = totalItems - 1;
    if (*c >= totalItems) *c = 0;
    if (IsKeyPressed(KEY_ESCAPE)) return false;
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 220 });
    int panelX = 200, panelY = 100, panelW = SCREEN_WIDTH - 400, panelH = SCREEN_HEIGHT - 200;
    DrawRectangle(panelX, panelY, panelW, panelH, (Color){ 30, 30, 45, 255 });
    DrawRectangleLines(panelX, panelY, panelW, panelH, GRAY);
    DrawText("LOJA DO VIAJANTE", SCREEN_WIDTH/2 - MeasureText("LOJA DO VIAJANTE", 50)/2, panelY + 30, 50, GOLD);
    DrawText(TextFormat("Suas Moedas: %d", p->coins), panelX + 40, panelY + panelH - 60, 30, GOLD);
    DrawText("W/S: Sel | ENTER: Comprar | ESC: Sair", panelX + panelW - 400, panelY + panelH - 60, 20, GRAY);
    int col1X = panelX + 100, col2X = panelX + panelW/2 + 50, startY = panelY + 150, spacing = 80;
    DrawText("AMULETOS", col1X, startY - 50, 30, PURPLE);
    DrawText("HABILIDADES", col2X, startY - 50, 30, BLUE);
    int countAm = 0, countHab = 0;
    for(int i = 0; i < totalItems; i++)
    {
        int drawX, drawY;
        bool owned;

        if(itens[i].tipo == 0) {
            drawX = col1X; drawY = startY + (countAm++ * spacing);
            owned = p->amuletos[itens[i].id_real]; // Check amulet ownership
        } else {
            drawX = col2X; drawY = startY + (countHab++ * spacing);
            owned = p->hab_owned[itens[i].id_real]; // Check skill ownership
        }

        bool sel = (*c == i);
        bool bought = owned;

        if (sel && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)))
        {
            if (!bought && p->coins >= itens[i].preco)
            {
                p->coins -= itens[i].preco;

                if(itens[i].tipo == 0) {
                    p->amuletos[itens[i].id_real] = true;
                } else {
                    p->habilidades[itens[i].id_real] = true;
                    p->hab_owned[itens[i].id_real] = true;
                }
            }
        }
        Color cNome = bought ? GRAY : (sel ? RED : WHITE);
        if (sel) DrawText(">", drawX - 30, drawY, 30, RED);
        DrawText(itens[i].nome, drawX, drawY, 30, cNome);
        if (bought) DrawText("COMPRADO", drawX, drawY + 35, 20, GREEN);
        else DrawText(TextFormat("$ %d", itens[i].preco), drawX, drawY + 35, 20, (sel ? GOLD : DARKGRAY));
    }
    return true;
}void updateinv(player *p, int *cur_window, int *cursor)
{

    int totalItems = MAX_SKILLS+MAX_AMULET;
    if (IsKeyPressed(KEY_W)) (*cursor)--;
    if (IsKeyPressed(KEY_S)) (*cursor)++;
    if (*cursor < 0) *cursor = totalItems - 1;
    if (*cursor >= totalItems) *cursor = 0;
    if (IsKeyPressed(KEY_ESCAPE)) *cur_window = 1;
    typedef struct { const char* nome; int tipo; int id; } InvItem;
    InvItem itens[] = {
        { "Correr", 0, 0 },
        { "Energia extra", 0, 1 },
        { "Amuleto de Vida", 0, 2 },
        { "Pedra de Dano", 0, 3 },
        { "Slot Extra", 0, 4 },
        { "Amuleto Pequeno", 0, 5 },
        { "Recupera vida", 1, 0 },
        { "Ataque especial", 1, 1 },
        { "Pulo Duplo", 1, 2 },
        { "Manto do Dash", 1, 3 }
    };
    int limitAm = MAX_AMULET;
    int limitHab = MAX_SKILLS;

    int cAm = 0; for(int i=0; i<MAX_AMULET; i++) if(p->ameq[i]) cAm++;
    int cHab = 0; for(int i=0; i<MAX_SKILLS; i++) if(p->habilidades[i]) cHab++;

    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 230 });
    int pX = 150, pY = 80, pW = SCREEN_WIDTH - 300, pH = SCREEN_HEIGHT - 160;
    DrawRectangle(pX, pY, pW, pH, (Color){ 20, 20, 30, 255 });
    DrawRectangleLines(pX, pY, pW, pH, DARKGRAY);
    DrawText(TextFormat("AMULETOS (%d/%d)", cAm, MAX_AMULET), pX+80, pY+80, 30, PURPLE);
    DrawText(TextFormat("HABILIDADES (%d/%d)", cHab, MAX_SKILLS), pX+pW/2+50, pY+80, 30, BLUE);
    int countAm = 0, countHab = 0;
    for(int i=0; i<totalItems; i++)
    {
        int dx, dy;
        bool owned, equipped;

        if(itens[i].tipo == 0) {
            dx = pX+80; dy = pY+140 + (countAm++ * 70);
            owned = p->amuletos[itens[i].id];
            equipped = p->ameq[itens[i].id];
        } else {
            dx = pX+pW/2+50; dy = pY+140 + (countHab++ * 70);
            owned = p->hab_owned[itens[i].id];
            equipped = p->habilidades[itens[i].id];
        }

        bool sel = (*cursor == i);
        if(sel && owned && IsKeyPressed(KEY_ENTER) && p->inbench )
        {
            if(equipped) {
                if(itens[i].tipo == 0) p->ameq[itens[i].id] = false;
                else p->habilidades[itens[i].id] = false;
                if(itens[i].id == 2 && itens[i].tipo == 0 && p->vidas > 5) { p->vidas=5; p->maxlife=5; }
                if(itens[i].id == 1 && itens[i].tipo == 0) { p->energyincrease=2;}
                if(itens[i].id == 5 && itens[i].tipo == 0) { p->tamanho.x=50; p->tamanho.y=110; }
            } else {
                bool can = (itens[i].tipo == 0 ? cAm < limitAm : cHab < limitHab);
                if(can) {
                    if(itens[i].tipo == 0) p->ameq[itens[i].id] = true;
                    else p->habilidades[itens[i].id] = true;
                    if(itens[i].id == 2 && itens[i].tipo == 0) { p->maxlife=8; p->vidas+=3; }
                    if(itens[i].id == 1 && itens[i].tipo == 0) { p->energyincrease=5;}
                    if(itens[i].id == 5 && itens[i].tipo == 0) { p->tamanho.x*=0.5; p->tamanho.y*=0.5; }
                }
            }
        }
        Color col = owned ? (equipped ? GREEN : WHITE) : DARKGRAY;
        if(sel) DrawText(">", dx-20, dy, 20, RED);
        DrawText(itens[i].nome, dx, dy, 20, col);
        if(!owned) DrawText("(Bloqueado)", dx+200, dy, 20, DARKGRAY);
        else if(equipped) DrawText("(Equipado)", dx+200, dy, 20, GREEN);
        char c[100] = "";
        switch (*cursor){
        case 0:
                strcpy(c, "Aumenta a velocidade de movimento ao segurar Shift");
                break;
        case 1:
                strcpy(c, "Faz com que receba 5 de energia por acerto (padrao: 2)");
                break;
        case 2:
                strcpy(c, "Aumenta a vida maxima de 5 para 8");
                break;
        case 3:
                strcpy(c, "Dobra o dano causado em todos os ataques");
                break;
        case 4:
                strcpy(c, "Permite equipar 4 amuletos ao mesmo tempo(contando esse)");
                break;
        case 5:
                strcpy(c, "Diminui o tamanho do jogador pela metade");
                break;
        case 6:
                strcpy(c, "Recupera 4 de vida ao segurar V (consome 5 de energia)");
                break;
        case 7:
                strcpy(c, "Permite executar um ataque poderoso que custa 5 de energia (Tecla F)");
                break;
        case 8:
                strcpy(c, "Permite realizar um segundo pulo enquanto estiver no ar.");
                break;
        case 9:
                strcpy(c, "Permite realizar um avanco rapido e invulneravel (Tecla Z)");
        }
        DrawText(c, pX + 50, pY + pH - 120, 20, WHITE);
    }
}
void updatemenu(int *c,int *s, bool *init)
{
    if(*c>=3||*c<0) *c=0;
    if(IsKeyPressed(KEY_W)) { if(*c<=0) *c=2; else (*c)--; }
    if(IsKeyPressed(KEY_S)) { if(*c>=2) *c=0; else (*c)++; }
    if(IsKeyPressed(KEY_ENTER)||IsKeyPressed(KEY_SPACE))
    {
        if(*c==0) { *init=true; *s=1; }
        if(*c==1) *s=15;
        if(*c==2) *s=4;
    }
}
void drawmenu(int s)
{
    int startY = (SCREEN_HEIGHT / 2) - 60;
    DrawText("HOLLOW KNIGHT", (SCREEN_WIDTH - MeasureText("HOLLOW KNIGHT", 60))/2, 150, 60, PURPLE);
    DrawText("CONTINUAR JOGO", (SCREEN_WIDTH - MeasureText("CONTINUAR JOGO", 40))/2, startY, 40, (s==0)?RED:GREEN);
    DrawText("NOVO JOGO", (SCREEN_WIDTH - MeasureText("NOVO JOGO", 40))/2, startY+60, 40, (s==1)?RED:GREEN);
    DrawText("SAIR DO JOGO", (SCREEN_WIDTH - MeasureText("SAIR DO JOGO", 40))/2, startY+120, 40, (s==2)?RED:GREEN);
}
void updateoption(int *c,int *s)
{
    int maxop=5;
    if(IsKeyPressed(KEY_W)) { if(*c<=0) *c=maxop; else (*c)--; }
    if(IsKeyPressed(KEY_S)) { if(*c>=maxop) *c=0; else (*c)++; }
    if(IsKeyPressed(KEY_ESCAPE)) *s=1;
    if(IsKeyPressed(KEY_ENTER)||IsKeyPressed(KEY_SPACE))
    {
        if(*c==0) *s=1;
        if(*c==1) *s=6;
        if(*c==2) *s=5;
        if(*c==3) *s=0;
        if(*c==4) *s=10;
        if(*c==5) *s=4;
    }
}
void drawoption(int s)
{
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 200 });
    int startY = (SCREEN_HEIGHT - 300) / 2;
    DrawText("JOGO PAUSADO", (SCREEN_WIDTH - MeasureText("JOGO PAUSADO", 50))/2, startY - 80, 50, WHITE);
    const char *opts[6] = { "Continuar", "Reiniciar", "Salvar jogador", "Retornar ao Menu", "Retornar a Vila", "Sair do Jogo" };
    for(int i = 0; i < 6; i++) {
        DrawText(opts[i], (SCREEN_WIDTH - MeasureText(opts[i], 30)) / 2, startY + (i * 50), 30, (s == i) ? RED : GREEN);
    }
}
void drawplayer(player *p)
{
    Sprite anim = p->sprites[p->currentAnim];
    float frameWidth = (float)anim.tex.width / anim.numFrames;
    float frameHeight = (float)anim.tex.height;
    Rectangle sourceRec = { p->currentFrame * frameWidth, 0.0f, frameWidth, frameHeight };
    if (p->facing == -1) sourceRec.width *= -1;
    float spriteOffsetY = 80.0f;
    float scale = 3.5f;
    if(p->ameq[5]) { scale = 1.75f; spriteOffsetY = 40.0f; }
    Rectangle destRec = { p->position.x, (p->position.y + p->tamanho.y/2) - (frameHeight * scale / 2) + spriteOffsetY, frameWidth * scale, frameHeight * scale };
    Vector2 origin = { destRec.width / 2.0f, destRec.height / 2.0f };
    DrawTexturePro(anim.tex, sourceRec, destRec, origin, 0.0f, WHITE);
}
void loadplayer(player *p){
    FILE *arq = fopen("player.bin", "rb");
        if (arq != NULL)
        {
            size_t read = fread(p, sizeof(player), 1, arq);
            fclose(arq);}
        else initplayer(p);
}
int main()
{
    int cur_window=0,sel_window=0;
    bool init=false;
    map maps;
    player p1;
    int shop_cursor = 0;
    int inv_cursor = 0;
    Enemy enemies[MAX_ENEMIES];
    int enemyCount = 0;
    int cur_map = 0;
    SetTargetFPS(60);
    Camera2D camera = {0};
    float deathtimer=0;
    camera.target = p1.position;
    camera.zoom=0.5;
    camera.offset = (Vector2)
    {
        SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2
    };
    InitWindow(SCREEN_WIDTH,SCREEN_HEIGHT,"jogo");
    loadplayer(&p1);
    LoadPlayerAnims(&p1);
    GameAssets minhasTexturas;
    minhasTexturas.parede = carregaTexture("texturas/paredes.png");
    minhasTexturas.player = carregaTexture("texturas/player.png");
    minhasTexturas.fundo = carregaTexture("texturas/fundo.png");
    SetExitKey(KEY_HOME);
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        if(init)
        {
            loadmaps(&maps,cur_map);
            enemyCount = initmap(&maps, &p1, enemies);
            p1.velocity.y=0;
            init=false;
        }
        BeginDrawing();
        ClearBackground(BLACK);
        switch (cur_window)
        {
        case 0:
            updatemenu(&sel_window,&cur_window,&init);
            drawmenu(sel_window);
            break;
        case 1:
            updateplayer(&p1,&cur_window, &maps,&cur_map,&init);
            UpdatePlayerAnimation(&p1);
            UpdateEnemies(enemies, enemyCount, maps,p1);
            CheckEnemyCollisions(&p1, enemies, enemyCount);
            camera.target.x = p1.position.x;
            camera.target.y = (p1.position.y < SCREEN_HEIGHT/2.0f) ? SCREEN_HEIGHT/2.0f : p1.position.y;
            BeginMode2D(camera);
            drawmap(maps, &p1, camera, minhasTexturas);
            DrawEnemies(enemies, enemyCount);
            drawplayer(&p1);
            PlayerAttack(&p1, enemies,enemyCount,camera,&cur_map, &init,&maps);
            EndMode2D();
            if(maps.shopen)
            {
                maps.shopen = updateshop(&p1, &shop_cursor);
            }
            DrawHUD(&p1);
            break;
        case 2:
            updateinv(&p1, &cur_window, &inv_cursor);
            break;
        case 3:
            updateoption(&sel_window,&cur_window);
            drawoption(sel_window);
            break;
        case 4:
            EndDrawing();
            CloseWindow();
            return 0;
            break;
        case 5:
            FILE *arq = fopen("player.bin", "wb");
                if (arq != NULL)
                {
                    fwrite(&p1, sizeof(player), 1, arq);
                    fclose(arq);
                }
            cur_window=1;
            break;
        case 6:
            init=true;
            cur_window=1;
            break;
        case 8:
            if(deathtimer<=1)
            {
                ClearBackground(BLACK);
                DrawText("MORREU OTARIO", 150, SCREEN_HEIGHT/2-100, 150, RED);
                deathtimer += GetFrameTime();
            }
            if(deathtimer>1)
            {
                deathtimer=0;
                cur_map=0;
                p1.vidas=p1.maxlife;
                p1.coins*=0.6;
                init=true;
                cur_window=1;
            }
            break;
        case 10:
            cur_map=0;
            init=true;
            cur_window=1;
            break;
        case 15:
            cur_map=1;
            initplayer(&p1);
            init=true;
            cur_window=1;
            break;
        }
        EndDrawing();
    }
    for(int i=0; i<MAX_ANIMS; i++) UnloadTexture(p1.sprites[i].tex);
    UnloadTexture(minhasTexturas.parede);
    UnloadTexture(minhasTexturas.player);
    UnloadTexture(minhasTexturas.fundo);
    CloseWindow();
    return 0;
}
