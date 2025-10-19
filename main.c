#include "raylib.h"

#define WINDOW_HEIGHT 900
#define WINDOW_WIDTH 1600


typedef struct {
    Vector2 position;
    Vector2 velocity;
    int vidas;
    int amuletos[6];
    Vector2 tamanho;
}player;

        void initplayer(player *p){
        p->position.x=50.0f;
        p->position.y=50.0f;
        p->amuletos[0]=4;
        p->tamanho.x=30;
        p->tamanho.y=30;

    }
    void updateplayer(player *p,int *s){
        if(IsKeyDown(KEY_D)){
            p->position.x += 10;
        }
        if(IsKeyDown(KEY_A)){
            p->position.x -= 10;

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
        if(IsKeyPressed(KEY_ENTER)){
            if(*c==0)
                *s=1; //continua
            if(*c==1)
                *s=6; //reinicia fase
            if(*c==2)
                *s=5; //opcoes?
            if(*c==3)
               *s=0; //menu inicial
            if(*c==4)
                *s=4; //quit

    }}
    void drawoption(int s){

        DrawText("continua:", 300, 20, 20, (s==0)?RED:GREEN);
        DrawText("reinicia", 300, 40, 20, (s==1)?RED:GREEN);
        DrawText("configuracoes", 300, 60, 20, (s==2)?RED:GREEN);
        DrawText("retornar ao menu", 300, 80, 20, (s==3)?RED:GREEN);
        DrawText("sair", 300, 100, 20, (s==4)?RED:GREEN);

    }

    void drawplayer(player *p){
        DrawRectangle(p->position.x,p->position.y,p->tamanho.x,p->tamanho.y,RED);
    }

    int main(){
        int cur_window=0,sel_window=0;
        player p1;
        initplayer(&p1);
        InitWindow(WINDOW_WIDTH,WINDOW_HEIGHT,"batata");
        SetExitKey(KEY_NULL);
        SetTargetFPS(60);
        while(!WindowShouldClose()){
            BeginDrawing();
            ClearBackground(PURPLE);
            switch (cur_window){
                case 0: // menu
                    updatemenu(&sel_window,&cur_window);
                    drawmenu(sel_window);

                    break;
                case 1: // gameplay
                    updateplayer(&p1,&cur_window);
                    drawplayer(&p1);
                    break;
                case 2: // inventario
                    DrawText("INVENTARIO",WINDOW_WIDTH/2,WINDOW_HEIGHT/2,40,GREEN);
                    break;
                case 3: //pausa
                    updateoption(&sel_window,&cur_window);
                    drawoption(sel_window);
                    break;

                    break;
                case 4: //quita
                    EndDrawing();
                    CloseWindow();

                case 5: //configs
                    DrawText("CONFIGS",WINDOW_WIDTH/2,WINDOW_HEIGHT/2,40,GREEN);
                    if(IsKeyPressed(KEY_ESCAPE))
                        cur_window=3;
                    break;
                case 6: //reinicia a fase
                    initplayer(&p1);
                    cur_window=1;
            }

            EndDrawing();
        }
        CloseWindow();
        return 0;
    }

