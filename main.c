#include "raylib.h"
#include <stdio.h>

#define SCREEN_HEIGHT 900
#define SCREEN_WIDTH 1600
#define GRAVITY 1
#define MOVE_SPEED 8
#define JUMP_STRENGTH 20

typedef struct {
    Vector2 position;
    Vector2 velocity;
    int vidas;
    int amuletos[6];
    Vector2 tamanho;
    bool chao;
}player;
typedef struct {
    char m[16][150];
    }map;

        void initplayer(player *p){
        p->position.x=64.0f;
        p->position.y=64.0f;
        p->velocity.x=00.0f;
        p->velocity.y=00.0f;
        p->amuletos[0]=4;
        p->tamanho.x=32;
        p->tamanho.y=32;
        p->chao=false;
        p->vidas = 5;

        }
void updateplayer(player *p,int *s){
    if(IsKeyDown(KEY_D)){
        p->velocity.x = MOVE_SPEED;
    }
    else if(IsKeyDown(KEY_A)){
        p->velocity.x = -MOVE_SPEED;}
    else p->velocity.x=0;

    if (IsKeyPressed(KEY_SPACE)&&p->chao)
        {
        p->velocity.y = -JUMP_STRENGTH;
        p->chao = false;}
    if (IsKeyDown(KEY_S))
        p->velocity.y += MOVE_SPEED*0.5;
    p->velocity.y += GRAVITY;

    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;

    if(p->position.x - p->tamanho.x/2  <= 0)
        p->position.x = p->tamanho.x/2;
    if(p->position.y + 1.5*p->tamanho.y>= SCREEN_HEIGHT){
        p->position.y = SCREEN_HEIGHT - 1.5*p->tamanho.y;
        p->velocity.y = 0;
        p->chao=true;
    }

    if(IsKeyPressed(KEY_ESCAPE)){
        *s = 3;
    }
    if(IsKeyDown(KEY_O)){
        *s = 2;
    }
}
    void updatemenu(int *c,int *s){
        if(*c>=3||*c<0)
            *c=0;
        if(IsKeyPressed(KEY_W)){
            if(*c<=0){
                *c=2;
            }
            else
                (*c)--;
        }
        if(IsKeyPressed(KEY_S)){
            if(*c>=2){
                *c=0;
            }
            else
                (*c)++;
        }
        if(IsKeyPressed(KEY_ENTER)){
            if(*c==0)
                *s=1;
            if(*c==1)
                *s=0;
            if(*c==2)
                *s=4;
        }

    }
    void drawmenu(int s){

        DrawText("comecar jogo", 300, 20, 30, (s==0)?RED:GREEN);
        DrawText("a decidir", 300, 40, 30, (s==1)?RED:GREEN);
        DrawText("sair do jogo", 300, 60, 30, (s==2)?RED:GREEN);

    }
    void updateoption(int *c,int *s){
        if(IsKeyPressed(KEY_W)){
            if(*c<=0){
                *c=4;
            }
            else
                (*c)--;
        }
        if(IsKeyPressed(KEY_S)){
            if(*c>=4){
                *c=0;
            }
            else
                (*c)++;
        }
        if(IsKeyPressed(KEY_ESCAPE))
            *s=1;
        if(IsKeyPressed(KEY_ENTER)||IsKeyPressed(KEY_SPACE)){
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

    }}
    void drawoption(int s){

        DrawText("continua:", 300, 20, 20, (s==0)?RED:GREEN);
        DrawText("reinicia", 300, 40, 20, (s==1)?RED:GREEN);
        DrawText("configuracoes", 300, 60, 20, (s==2)?RED:GREEN);
        DrawText("retornar ao menu", 300, 80, 20, (s==3)?RED:GREEN);
        DrawText("sair", 300, 100, 20, (s==4)?RED:GREEN);

    }

    void drawplayer(player *p){
        DrawRectangle(p->position.x-0.5*p->tamanho.x,p->position.y+0.5*p->tamanho.x,p->tamanho.x,p->tamanho.y,RED);
    }

    int main(){
        int cur_window=0,sel_window=0;
        player p1;
        initplayer(&p1);
        InitWindow(SCREEN_WIDTH,SCREEN_HEIGHT,"jogo");
        SetExitKey(KEY_HOME);
        SetTargetFPS(60);
        while(!WindowShouldClose()){
            BeginDrawing();
            ClearBackground(PURPLE);
            switch (cur_window){
                case 0:
                    updatemenu(&sel_window,&cur_window);
                    drawmenu(sel_window);

                    break;
                case 1:
                    updateplayer(&p1,&cur_window);
                    drawplayer(&p1);
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
                    initplayer(&p1);
                    cur_window=1;
            }

            EndDrawing();
        }
        CloseWindow();
        return 0;
    }
