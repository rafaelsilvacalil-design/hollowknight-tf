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
#define MAX_ROWS 16
#define MAX_COLS 160
#define MAX_ENEMIES 50
#define MAX_ENERGY 10

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
    bool habilidades[3];
    int energy,energyincrease;
    int cur_an;
    int facing;
    float attackTimer;
    bool t;
    float attackCooldown;
    float hitbox_size;
    float speed_mult;
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

typedef struct {
    Rectangle rect;
    float speed;
    int direction;
    bool active;
    int hp;
    float hitTimer;
    int type;
    int dirY;
} Enemy;

void initplayer(player *p)
{
    p->velocity.x=00.0f;
    p->velocity.y=00.0f;
    p->tamanho.x=SIZE-4;
    p->tamanho.y=SIZE-4;
    p->chao=false;
    p->maxlife=5;
    p->vidas = p->maxlife;
    p->invtimer = 0;
    p->inbench = false;
    p->coins = 50000000;
    p->energy = 0;
    p->energyincrease = 2;
    p->speed_mult=1;
    for(int i = 0; i<6; i++)
    {
        p->amuletos[i]=false;
        p->ameq[i]=false;
    }
    for(int i = 0; i<3; i++)
        p->habilidades[i]=false;
    p->facing = 1;
    p->attackTimer = 0;
    p->hitbox_size=1;
    p->attackCooldown = 0;
}

void floodfill(map *ma, int i, int j, char mark)
{
    if (i < 0 || i >= ma->rows || j < 0 || j >= ma->cols-1) return;
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
    DrawTexturePro(assets.fundo, sourceFundo, destFundo, (Vector2){0, 0}, 0.0f, WHITE);
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
            case 'J':
                break;
            case 'P':
            {
                Rectangle source = { 0, 0, (float)assets.parede.width, (float)assets.parede.height };
                Rectangle dest = { (float)(xsize*j), (float)(ysize*i), (float)xsize, (float)ysize };
                DrawTexturePro(assets.parede, source, dest, (Vector2){0,0}, 0.0f, WHITE);
                break;
            }
            case 'M':
                DrawRectangle(xsize*j,ysize*i,xsize,ysize,YELLOW);
                break;
            case 'A':
                DrawRectangle(xsize*j, ysize*i, xsize, ysize, BEIGE);
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,GREEN);
                break;
            case 'C':
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,PINK);
                break;
            case 'H':
                DrawRectangle(xsize*j,ysize*i,xsize,ysize,LIME);
                break;
            case 'E':
                DrawRectangle(xsize*j, ysize*i, xsize, ysize, BLACK);
                break;
            case 'S':
                DrawRectangle(xsize*j, ysize*i, xsize, ysize, (!ma.shopen?GOLD:GREEN));
                break;
            case 'B':
                DrawRectangle(xsize*j, ysize*i, xsize, ysize, BEIGE);
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,(p->inbench?PINK:LIME));
                break;
            case '1':
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,DARKPURPLE);
                DrawText("1",xsize*(0.4+j), ysize*(0.4+i), 60, GREEN);
                break;
            case '2':
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,DARKPURPLE);
                DrawText("2",xsize*(0.4+j), ysize*(0.4+i), 60, GREEN);
                break;
            case '3':
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,DARKPURPLE);
                DrawText("3",xsize*(0.4+j), ysize*(0.4+i), 60, GREEN);
                break;
            case '4':
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,DARKPURPLE);
                DrawText("4",xsize*(0.4+j), ysize*(0.4+i), 60, GREEN);
                break;
            case '5':
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,DARKPURPLE);
                DrawText("5",xsize*(0.4+j), ysize*(0.4+i), 60, GREEN);
                break;
            default:
                break;
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
        }
    }
    return count;
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
    int endRow = floor((playerRectX.y + playerRectX.height) / SIZE);
    int startCol = floor(playerRectX.x / SIZE);
    int endCol = floor((playerRectX.x + playerRectX.width) / SIZE);
    for (int i = startRow; i <= endRow; i++)
    {
        for (int j = startCol; j <= endCol; j++)
        {
            if (i < 0 || i >= ma.rows || j < 0 || j >= ma.cols) continue;
            char tileType = ma.m[i][j];
            if (tileType == 'P'||(p->invtimer>0&&(tileType=='C'||tileType=='M')))
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
    endRow = floor((playerRectY.y + playerRectY.height) / SIZE);
    startCol = floor(playerRectY.x / SIZE);
    endCol = floor((playerRectY.x + playerRectY.width) / SIZE);
    p->chao = false;
    for (int i = startRow; i <= endRow; i++)
    {
        for (int j = startCol; j <= endCol; j++)
        {
            if (i < 0 || i >= ma.rows || j < 0 || j >= ma.cols) continue;
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

void CheckObjectCollisions(player *p, map *ma, int *curmap, bool *init)
{
    Rectangle playerRect =
    {
        p->position.x - p->tamanho.x / 2.0f,
        p->position.y - p->tamanho.y / 2.0f,
        p->tamanho.x,
        p->tamanho.y
    };
    int startRow = floor(playerRect.y / SIZE);
    int endRow = floor((playerRect.y + playerRect.height) / SIZE);
    int startCol = floor(playerRect.x / SIZE);
    int endCol = floor((playerRect.x + playerRect.width) / SIZE);
    for (int i = startRow; i <= endRow; i++)
    {
        for (int j = startCol; j <= endCol; j++)
        {
            if (i < 0 || i >= ma->rows || j < 0 || j >= ma->cols) continue;
            char tileType = ma->m[i][j];
            Rectangle tileRect = {j * SIZE, i * SIZE, SIZE, SIZE};
            if (CheckCollisionRecs(playerRect, tileRect))
            {
                switch (tileType)
                {
                case 'A':
                    if (p->chao && IsKeyPressed(KEY_W))
                    {
                        if(p->amuletos[3])p->amuletos[4]=true;
                        else if(p->amuletos[0])p->amuletos[3]=true;
                        else p->amuletos[0]=true;


                        p->amuletos[0]=true;
                        ma->m[i][j]=' ';
                    }
                    break;
                case 'H':
                    if (p->chao && IsKeyPressed(KEY_W))
                    {
                         ma->m[i][j]=' ';


                         p->habilidades[0]=true;
                    }
                    break;
                case 'S':
                    if (p->chao && IsKeyPressed(KEY_W))
                    {
                        ma->shopen=true;
                    }
                    break;
                case 'B':
                    if(!p->inbench) DrawText("aperte W para sentar e, sentado, aperte o para acessar o inventario",p->position.x,p->position.y-400,30,PINK);//n funciona pq o drawmap desenha por cima, preguica de arrumar (pode fazer uma struct pra informar q tem q desenhar)
                    if (p->chao && IsKeyPressed(KEY_W))
                    {
                        p->inbench=true;
                        p->vidas=5;
                    }
                    break;
                case 'M':
                    break;
                case 'C':
                    if (p->invtimer<=0 )
                    {
                        p->vidas--;
                        p->invtimer = 1.0f;
                        float tileCenterX = tileRect.x + tileRect.width / 2.0f;
                        float playerCenterX = p->position.x;
                        if (playerCenterX < tileCenterX)
                        {
                            p->velocity.x = -200;
                        }
                        else
                        {
                            p->velocity.x = 200;
                        }
                        p->velocity.y = -10;
                    }
                    break;
                case '1':
                    if (p->chao && IsKeyPressed(KEY_W))
                    {
                        *init= true;
                        *curmap = 1;
                    }
                    break;
                case '2':
                    if (p->chao && IsKeyPressed(KEY_W))
                    {
                        *init=true;
                        *curmap = 2;
                    }
                    break;
                case '3':
                    if (p->chao && IsKeyPressed(KEY_W))
                    {
                        *init= true;
                        *curmap = 3;
                    }
                    break;
                case '4':
                    if (p->chao && IsKeyPressed(KEY_W))
                    {
                        *init= true;
                        *curmap = 4;
                    }
                    break;
                case '5':
                    if (p->chao && IsKeyPressed(KEY_W))
                    {
                        *init= true;
                        *curmap = 5;
                    }
                    break;
                }
            }
        }
    }
}

void updateplayer(player *p,int *s, map *map, int *curmap,bool *init)
{
    if(p->ameq[5])p->speed_mult=1.4;
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
    if(IsKeyDown(KEY_S)&&p->t){p->t=false;p->velocity.y*=0.1;}
    if (IsKeyPressed(KEY_SPACE)&&p->chao)
    {
        p->velocity.y = -JUMP_STRENGTH;
        p->chao = false;
        p->t=true;
    }

    if(IsKeyPressed(KEY_ESCAPE))
    {
        *s = 3;
    }
    if(p->inbench)
    {
         if(IsKeyDown(KEY_O))
        {
            *s = 2;
        }
        if(p->velocity.x!=0||p->velocity.y!=0)
             p->inbench=false;
    }

    if(map->shopen)
    {
         if(p->velocity.x!=0||p->velocity.y!=0)
             map->shopen=false;
    }
    if(IsKeyDown(KEY_L))
    {
        p->vidas=0;
    }
    if(IsKeyDown(KEY_LEFT_SHIFT)&&p->ameq[1])
    {
        p->velocity.x*=2;
    }
    if(IsKeyDown(KEY_N)&&p->habilidades[0]&&p->energy>=4)
    {
        p->energy-=4;
    }
    p->velocity.y += GRAVITY;
    if(p->invtimer>0) p->invtimer-=0.02;
    CheckObjectCollisions(p, map, curmap, init);
    PlayerColi(p, *map);
    if(p->vidas<=0)
        *s=8;
}

void UpdateEnemies(Enemy enemies[], int enemyCount, map ma)
{
    for(int i = 0; i < enemyCount; i++)
    {
        if(!enemies[i].active) continue;
        if(enemies[i].hitTimer > 0) enemies[i].hitTimer -= 0.02f;
        if (enemies[i].type == 0)
        {
            enemies[i].rect.x += enemies[i].speed * enemies[i].direction;
            float checkX = (enemies[i].direction == 1) ?
                           enemies[i].rect.x + enemies[i].rect.width + 5 :
                           enemies[i].rect.x - 5;
            int col = (int)(checkX / SIZE);
            int row = (int)((enemies[i].rect.y + SIZE/2) / SIZE);
            if (col < 0 || col >= ma.cols || ma.m[row][col] == 'P')
            {
                enemies[i].direction *= -1;
            }
            else
            {
                int rowBelow = (int)((enemies[i].rect.y + SIZE + 5) / SIZE);
                if(rowBelow < ma.rows && ma.m[rowBelow][col] == ' ')
                     enemies[i].direction *= -1;
            }
        }
        else if (enemies[i].type == 1)
        {
            enemies[i].rect.x += enemies[i].speed * enemies[i].direction;
            float checkX = (enemies[i].direction == 1) ?
                           enemies[i].rect.x + enemies[i].rect.width + 5 :
                           enemies[i].rect.x - 5;
            int colX = (int)(checkX / SIZE);
            int rowCenter = (int)((enemies[i].rect.y + SIZE/2) / SIZE);
            if (colX < 0 || colX >= ma.cols || ma.m[rowCenter][colX] == 'P')
            {
                enemies[i].direction *= -1;
            }
            enemies[i].rect.y += enemies[i].speed * enemies[i].dirY;
            float checkY = (enemies[i].dirY == 1) ?
                           enemies[i].rect.y + enemies[i].rect.height + 5 :
                           enemies[i].rect.y - 5;
            int colCenter = (int)((enemies[i].rect.x + SIZE/2) / SIZE);
            int rowY = (int)(checkY / SIZE);
            if (rowY < 0 || rowY >= ma.rows || ma.m[rowY][colCenter] == 'P')
            {
                enemies[i].dirY *= -1;
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
            Color corBase;
            if (enemies[i].type == 1)
                corBase = ORANGE;
            else
                corBase = YELLOW;
            Color corFinal = (enemies[i].hitTimer > 0) ? BEIGE : corBase;
            DrawRectangleRec(enemies[i].rect, corFinal);
        }
    }
}

void CheckEnemyCollisions(player *p, Enemy enemies[], int enemyCount)
{
    if (p->invtimer > 0) return;
    Rectangle playerRect = {
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
            p->vidas--;
            p->invtimer = 1.0f;
            if (p->position.x < enemies[i].rect.x + enemies[i].rect.width/2)
                p->velocity.x = -200;
            else
                p->velocity.x = 200;
            p->velocity.y = -10;
            break;
        }
    }
}

void PlayerAttack(player *p, Enemy enemies[], int enemyCount)
{
    if(p->attackCooldown > 0) p->attackCooldown -= 0.02f;
    if(p->attackTimer > 0) p->attackTimer -= 0.02f;
    if(IsKeyPressed(KEY_J) && p->attackCooldown <= 0)
    {
        p->attackTimer = 0.2f;
        p->attackCooldown = 0.4f;
    }
    if(p->attackTimer > 0)
    {
        Rectangle attackRect;
        attackRect.width = 120;
        attackRect.height = 80;
        if(IsKeyDown(KEY_W))
        {
            attackRect.width = 80;
            attackRect.height = 120;
            attackRect.x = p->position.x - attackRect.width/2;
            attackRect.y = p->position.y - p->tamanho.y/2 - attackRect.height;
        }
        else if(IsKeyDown(KEY_S) && !p->chao)
        {
            attackRect.width = 80;
            attackRect.height = 120;
            attackRect.x = p->position.x - attackRect.width/2;
            attackRect.y = p->position.y + p->tamanho.y/2;
        }
        else
        {

            attackRect.y = p->position.y - 40;
            if(p->facing == 1)
                attackRect.x = p->position.x + p->tamanho.x/2;
            else
                attackRect.x = p->position.x - p->tamanho.x/2 - attackRect.width;
        }
        DrawRectangleRec(attackRect, WHITE);
        for(int i = 0; i < enemyCount; i++)
        {
            if(enemies[i].active)
            {
                if(CheckCollisionRecs(attackRect, enemies[i].rect) && enemies[i].hitTimer <= 0)
                {
                    enemies[i].hp-=(p->ameq[2]?2:1);
                    enemies[i].hitTimer = 0.5f;
                    p->invtimer = 0.2f;
                    p->energy+=p->energyincrease;
                    if(p->energy>MAX_ENERGY)p->energy=MAX_ENERGY;

                    if(IsKeyDown(KEY_S) && !p->chao)
                    {
                        p->velocity.y = -JUMP_STRENGTH * 0.8f;}


                    else if (IsKeyDown(KEY_W)) p->velocity.y=0;
                    else p->velocity.x=0;
                    if(enemies[i].hp <= 0)
                    {
                        enemies[i].active = false;
                        p->coins+=15;
                    }
                }
            }
        }
    }
}

bool updateshop(player *p, int *c, bool comprados[])
{
    struct ShopItem { const char* label; int price; };
    struct ShopItem items[5] = {
    { "amuleto 2 (vida), 20 moedas", 20 },
    { "habilidade 2 (dash), 25 moedas", 25 },
    { "amuleto 3(dano, 30 moedas", 30 },
    { "habilidade 3 (pulo duplo), 65 moedas", 45 },
    { "amuleto 6 , 500 moedas", 500 },

    };
    int activeItems[5];
    int activeCount = 0;
    for (int i = 0; i < 5; i++) {
    if (!comprados[i]) {
    activeItems[activeCount++] = i;
    }
    }
    if (activeCount > 0) {
    if (IsKeyPressed(KEY_W)) (*c)--;
    if (IsKeyPressed(KEY_S)) (*c)++;
    if (*c < 0) *c = activeCount - 1;
    if (*c >= activeCount) *c = 0;
    } else {
    *c = -1;
    }
    DrawRectangle(p->position.x - 60, p->position.y - 150, 510, 190, (Color){ 130, 191, 255, 200 });
    if (activeCount == 0) {
        DrawText("Sold Out!", p->position.x - 50, p->position.y - 140, 50, GOLD);
    } else {
        for (int i = 0; i < activeCount; i++) {
        int realIndex = activeItems[i];
        DrawText(items[realIndex].label,p->position.x - 50,p->position.y - 140 + (i * 40),30,(*c == i) ? RED : GREEN);
        }
    }
    bool r = true;
    if (IsKeyPressed(KEY_ENTER) && activeCount > 0)
    {
        int selectedRealIndex = activeItems[*c];
        if (p->coins >= items[selectedRealIndex].price)
        {
        p->coins -= items[selectedRealIndex].price;
        comprados[selectedRealIndex] = true;
        switch (selectedRealIndex)
        {
            case 0: p->amuletos[1] = true; break;
            case 1: p->habilidades[1] = true; break;
            case 2: p->amuletos[2] = true; break;
            case 3: p->habilidades[2] = true; break;
            case 4: p->amuletos[5] = true; break;
            }
        *c = 0;
        r = false;
        }
    }
    return r;
}

void updateinv(int *c,player *p)
{
    int maxop = 5,maxeq=(p->ameq[3]?4:2);
   if(IsKeyPressed(KEY_W))
    {
        if(*c<=0)
        {
            *c=maxop;
        }
        else
            (*c)--;
    }
    if(IsKeyPressed(KEY_S))
    {
        if(*c>=maxop)
        {
            *c=0;
        }
        else
            (*c)++;
    }
    int t=0;
    for(int i = 0;i<6;i++) if(p->ameq[i])t++;
    if(IsKeyPressed(KEY_ENTER))
    {
        if(p->ameq[*c]){
            p->ameq[*c]=false;
            if(*c==1&&p->vidas>5){p->vidas=5;p->maxlife=5;} //garante q n fica com mais de 5 vidas
            else if(*c==5){//volta ao tamanho normal
                p->tamanho.x=SIZE;
                p->tamanho.y=SIZE;
                p->hitbox_size=1;
            }
        }
        else if(t<maxeq&&p->amuletos[*c]) {
            p->ameq[*c]=true;
            if(*c==1){p->maxlife=8;p->vidas+=3;}//caso aumente a vida maxima ja aumenta direto

            else if(*c==5){
                p->tamanho.x*=0.5;
                p->tamanho.y*=0.5;
                p->hitbox_size=0.6;
            }}
        else if (t>=maxeq) printf("mais q maximo de amuleto");//se quiser fazer aparecer a mensagem direitinho dps da pra fazer
        else printf("amuleto nao desbloqueado");
    }
}

void drawinv(int s,player p)
{
    char a[100];
    DrawRectangle(300,20*(s+2),150,30,(p.amuletos[s]?WHITE:GRAY));
    DrawText("am1:", 300, 40, 20, (p.ameq[0])?RED:GREEN);
    DrawText("am2", 300, 60, 20, (p.ameq[1])?RED:GREEN);
    DrawText("am3", 300, 80, 20, (p.ameq[2])?RED:GREEN);
    DrawText("am4", 300, 100, 20, (p.ameq[3])?RED:GREEN);
    DrawText("am5", 300, 120, 20, (p.ameq[4])?RED:GREEN);
    DrawText("am6", 300, 140, 20, (p.ameq[5])?RED:GREEN);
    switch(s){
    case 0:strcpy(a,"amuleto para te permitir correr (segurando shift)"); break;
    case 1:strcpy(a,"amuleto para te dar 3 vidas extras"); break;
    case 2:strcpy(a,"amuleto para aumentar seu dano"); break;
    case 3:strcpy(a,"amuleto para te permitir equipar 1 amuleto extra(sem contar esse)"); break;
    case 4:strcpy(a,"amuleto para duplicar seu ganho de energia"); break;
    case 5:strcpy(a,"smol"); break;
    }
    DrawText(a,500,100,30, WHITE);
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
            *s=15;
        if(*c==2)
            *s=4;
    }
}

void drawmenu(int s)
{
    DrawText("continuar jogo", 300, 20, 30, (s==0)?RED:GREEN);
    DrawText("new game", 300, 40, 30, (s==1)?RED:GREEN);
    DrawText("sair do jogo", 300, 60, 30, (s==2)?RED:GREEN);
}

void updateoption(int *c,int *s)
{
    int maxop=5;
    if(IsKeyPressed(KEY_W))
    {
        if(*c<=0)
        {
            *c=maxop;
        }
        else
            (*c)--;
    }
    if(IsKeyPressed(KEY_S))
    {
        if(*c>=maxop)
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
            *s=10;
        if(*c==5)
            *s=4;
    }
}

void drawoption(int s)
{
    DrawText("continua:", 300, 20, 20, (s==0)?RED:GREEN);
    DrawText("reinicia", 300, 40, 20, (s==1)?RED:GREEN);
    DrawText("configuracoes", 300, 60, 20, (s==2)?RED:GREEN);
    DrawText("retornar ao menu", 300, 80, 20, (s==3)?RED:GREEN);
    DrawText("Retornar a vila", 300, 100, 20, (s==4)?RED:GREEN);
    DrawText("sair", 300, 120, 20, (s==5)?RED:GREEN);
}

void drawplayer(player *p,Camera2D camera)
{
    DrawRectangle(p->position.x - 0.5 * p->tamanho.x, p->position.y - 0.5f * p->tamanho.y, p->tamanho.x, p->tamanho.y, RED);
    DrawText(TextFormat("vidas: %d/%d", p->vidas,p->maxlife), p->position.x-SCREEN_WIDTH+10, p->position.y-SCREEN_HEIGHT+10, 20, RED);
    DrawText(TextFormat("moedas: %d", p->coins), p->position.x-SCREEN_WIDTH+10, p->position.y-SCREEN_HEIGHT+30, 20, RED);
    DrawText(TextFormat("energia: %d", p->energy), p->position.x-SCREEN_WIDTH+10, p->position.y-SCREEN_HEIGHT+50, 20, RED);
}

int main()
{
    int cur_window=0,sel_window=0;
    bool init=false;
    map maps;
    player p1;
    initplayer(&p1);
    bool comprados [6]= {false};
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
            UpdateEnemies(enemies, enemyCount, maps);
            CheckEnemyCollisions(&p1, enemies, enemyCount);
            camera.target.x = p1.position.x;
            camera.target.y = (p1.position.y < SCREEN_HEIGHT/2.0f) ? SCREEN_HEIGHT/2.0f : p1.position.y;
            BeginMode2D(camera);
            drawmap(maps, &p1, camera, minhasTexturas);
            DrawEnemies(enemies, enemyCount);
            drawplayer(&p1,camera);
            PlayerAttack(&p1, enemies,enemyCount);
            if(maps.shopen) maps.shopen=updateshop(&p1,&shop_cursor,comprados);
            EndMode2D();
            break;
        case 2:
            DrawText("INVENTARIO",SCREEN_WIDTH/2-20,SCREEN_HEIGHT/2-20,40,GREEN);
            updateinv(&inv_cursor,&p1);
            drawinv(inv_cursor, p1);
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
            return 0;
            break;
        case 5:
            DrawText("CONFIGS",SCREEN_WIDTH/2,SCREEN_HEIGHT/2,40,GREEN);
            if(IsKeyPressed(KEY_ESCAPE))
                cur_window=3;
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
    UnloadTexture(minhasTexturas.parede);
    UnloadTexture(minhasTexturas.player);
    UnloadTexture(minhasTexturas.fundo);
    CloseWindow();
    return 0;
}
