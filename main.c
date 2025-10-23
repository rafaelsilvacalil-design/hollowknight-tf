#include "raylib.h"
#include <stdio.h>
#include <math.h>

#define SCREEN_HEIGHT 900
#define SCREEN_WIDTH 1600
#define WIDTH_LIMIT SCREEN_WIDTH*15
#define GRAVITY 1.6
#define MOVE_SPEED 30
#define JUMP_STRENGTH 50
#define SIZE 128


typedef struct
{
    Vector2 position;
    Vector2 velocity;
    int vidas;
    int amuletos[6];
    Vector2 tamanho;
    bool chao;
} player;
typedef struct
{
    char m[22][200];
} map;

void initplayer(player *p)
{

    p->velocity.x=00.0f;
    p->velocity.y=00.0f;
    p->amuletos[0]=4;
    p->tamanho.x=SIZE;
    p->tamanho.y=SIZE;
    p->chao=false;
    p->vidas = 5;

}


void loadmap(map *mapa, char *filename)
{
    FILE *fmap=fopen(filename,"r");
    for (int i = 0; i < 20; i++)
    {
        fgets(mapa->m[i], 180, fmap);

    }


    fclose(fmap);
}

void loadmaps (map *maps)
{
    loadmap(&maps[0],"maps/maptest.txt");


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
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,GREEN);
                break;
            case 'C': //boss
                DrawCircle(xsize*(0.5+j),ysize*(0.5+i),xsize/2,PINK);
                break;
            case 'H'://habilidade
                DrawRectangle(xsize*j,ysize*i,xsize,ysize,PURPLE);
                break;

            default : //ar
                DrawRectangle(xsize*j, ysize*i, xsize, ysize, LIGHTGRAY);
                break;

            }
        }
    }
    EndMode2D();
}
void initmap(map ma, player *p) //determina a posicao inicial do jogador
{
    char temp;
    for(int i = 0; i < 22; i++)
    {
        for(int j = 0; j < 200; j++)
        {
            temp = ma.m[i][j];
            if(temp == 'J')
            {
                p->position.x = j * SIZE + SIZE/2;
                p->position.y = i * SIZE + SIZE/2;
                return;
            }
        }
    }
}
bool checkhitbox(map ma, player p)
{
    // Calculate player's top-left corner first
    float top_left_x = p.position.x - SIZE/2;
    float top_left_y = p.position.y - SIZE/2;


    int top_i = floor(top_left_y / SIZE);
    int left_j = floor(top_left_x / SIZE);
    int bottom_i = ((top_left_y + SIZE - 0.01) / SIZE);
    int right_j = floor((top_left_x + SIZE - 1) / SIZE);

    Rectangle pr = {top_left_x+3, top_left_y+20, SIZE-3, SIZE-20};


    for (int k = top_i; k <= bottom_i; k++) {
        for (int l = left_j; l <= right_j; l++) {
            if (k < 0 || k >= 22 || l < 0 || l >= 200) continue; // Bounds check

            Rectangle tr = {l * SIZE, k * SIZE, SIZE, SIZE};
            if (CheckCollisionRecs(tr, pr)) {

                if (ma.m[k][l] == 'P' || ma.m[k][l] == 'M' || ma.m[k][l] == 'C') {
                    return false;
                }
            }
        }
    }
    return true;
}






void updateplayer(player *p,int *s, map map)
{
    //input
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

    //colisao
    player tempx = *p;
    tempx.position.x+=tempx.velocity.x;
    if(checkhitbox(map,tempx)&&p->position.x>=0&&p->position.x<200*SIZE)
        p->position.x += p->velocity.x;
    else
        p->velocity.x=0;
       player tempy = *p;
    tempy.position.y += tempy.velocity.y;
    if(checkhitbox(map, tempy) && p->position.y >= 0 && p->position.y < 16*SIZE)
    {
        p->position.y += p->velocity.y;
        p->chao = false;
    }
    else {
        if(p->velocity.y > 0)
            p->chao = true;
        p->velocity.y = 0;
    }


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
    DrawText("a decidir", 300, 40, 30, (s==1)?RED:GREEN);
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
    EndMode2D();
}

int main()
{
    int cur_window=0,sel_window=0;
    bool init=false;
    map maps[6];
    player p1;
    initplayer(&p1);

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
    while(!WindowShouldClose())
    {

        if(init)
        {
            loadmaps(maps);
            initmap(maps[0],&p1);
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
            updateplayer(&p1,&cur_window, maps[0]);
            camera.target.x = (p1.position.x < SCREEN_WIDTH/2.0f) ? SCREEN_WIDTH/2.0f : p1.position.x;
            camera.target.y = (p1.position.y < SCREEN_HEIGHT/2.0f) ? SCREEN_HEIGHT/2.0f : p1.position.y;

            drawmap(maps[0], &p1, camera);
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
