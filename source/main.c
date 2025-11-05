#include "gba.h"

#define  GBA_SW 160                                        //actual gba screen width
#define  SW     120                                       //game screen width
#define  SH     80                                     //game screen height
#define RGB(r,g,b) ((r)+((g)<<5)+((b)<<10))                //15 bit, 0-31, 5bit=r, 5bit=g, 5bit=b

#define PADDLE_OFFSET 20
#define PADDLE_WIDTH 4
#define PADDLE_HEIGHT 30

int lastFr = 0;
int FPS = 0;

u16 bgColor = RGB(8,12,16);
u16 lineColor = RGB(4,6,8);
u16 paddleColor = RGB(31, 31, 31);

typedef struct {
    int x;
    int y;
    int width;
    int height;
    u16 color;
} Object;    
Object player;
Object enemy;

void clearBackground()
{
    for(int x=0;x<SW;x++) {
        for(int y=0;y<SH;y++){
            VRAM[y * GBA_SW + x] = bgColor;
        }
    }


    for (int y=0;y<SH;y++) {
        VRAM[y * GBA_SW + SW / 2 - 1] = lineColor;
        VRAM[y * GBA_SW + SW / 2] = lineColor;
    }
}

void clearObject(Object* o) {
    for (int i = 0; i < o -> height; i++) {
        for (int j = 0; j < o -> width; j++) {
            VRAM[(o -> x + j) + GBA_SW * (o -> y + i)] = bgColor;
        }
    }
}

void drawObject(Object* o) {
    for (int i = 0; i < o -> height; i++) {
        for (int j = 0; j < o -> width; j++) {
            VRAM[(o -> x + j) + GBA_SW * (o -> y + i)] = paddleColor;
        }
    }
}

//this is useless when using pageflipping
//void moveObject(Object* o, int dx, int dy) {
//    clearObject(o);
//    o -> x += dx;
//    o -> y += dy;
//    drawObject(o);
//}

void buttons()                                            
{
    if(KEY_R ){ } 
    if(KEY_L ){ }
    
    if(KEY_U ){
        player.y -= 3;
        drawObject(&player);
        if(player.y < 0){ 
            player.y = 0;
            drawObject(&player);
        }
    }
    
    if(KEY_D ){
        player.y += 3;
        drawObject(&player);
        if(player.y > SH-1 - player.height){ 
            player.y = SH - player.height;
            drawObject(&player);
        }
    }
    
    if(KEY_A ){ } 
    if(KEY_B ){ } 
    if(KEY_LS){ } 
    if(KEY_RS){ } 
    if(KEY_ST){ } 
    if(KEY_SL){ } 
}

void init() {

    player.width = PADDLE_WIDTH;
    player.height = PADDLE_HEIGHT;
    player.x = PADDLE_OFFSET;
    player.y = SH / 2 - PADDLE_HEIGHT / 2;
    player.color = paddleColor;

    enemy.width = PADDLE_WIDTH;
    enemy.height = PADDLE_HEIGHT;
    enemy.x = SW - PADDLE_WIDTH - PADDLE_OFFSET;
    enemy.y = SH / 2 - PADDLE_HEIGHT / 2;
    enemy.color = paddleColor;
    
    drawObject(&player);
}

int main() {
    *(u16*)0x4000000 = 0x405;  
    *(u16*)0x400010A = 0x82;   
    *(u16*)0x400010E = 0x84;

    //scale small mode 5 screen to full screen-------------------------------------
    REG_BG2PA=128;                                         //256=normal 128=scale 
    REG_BG2PD=128;                                         //256=normal 128=scale     
    
    init();
    
    while (1) {
        
        
        if(REG_TM2D>>12!=lastFr) {
            clearBackground();
            buttons();
            drawObject(&player);
            drawObject(&enemy);
            

            //frames per second---------------------------------------------------------- 
            VRAM[15] = 0; 
            VRAM[FPS] = RGB(31, 31, 0);                               //draw fps 
            
            FPS+=1;                                                 //increase frame
            if(lastFr>REG_TM2D>>12){ 
                FPS = 0;                                            //reset counter?
            }                          
            
            lastFr=REG_TM2D>>12;                                    //reset counter?
            
            //swap buffers---------------------------------------------------------------
            while(*Scanline<160){}	                                         //wait all scanlines
            if  ( DISPCNT&BACKB){ DISPCNT &= ~BACKB; VRAM=(u16*)VRAM_B;}      //back  buffer
            else{                 DISPCNT |=  BACKB; VRAM=(u16*)VRAM_F;}      //front buffer  
        }
    }
}