#include <SDL2/SDL.h> 
//#include <SDL2/SDL_image.h> 
#include <SDL2/SDL_timer.h> 
#include <SDL2/SDL_ttf.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
const int SCREEN_WIDTH = 1366;
const int SCREEN_HEIGHT = 768;
const int FPS = 60;
void CSDL_DrawRectangle(SDL_Renderer* renderer, int x, int y, int w, int h){
	int i;
	//printf("Drawing square from %d,%d to %d,%d\n",x,y,x+w,y+h);
	
	for(i=y;i<=y+h;i++){
		SDL_RenderDrawLine(renderer, x,i,x+w,i);
	}
	/*SDL_SetRenderDrawColor(renderer,25,25,0,255);
	SDL_RenderDrawLine(renderer, x,y,x+w-1,y);
	SDL_RenderDrawLine(renderer, x,y,x,y+h-1);
	SDL_RenderDrawLine(renderer, x+w-1,y,x+w-1,y+h-1);
	SDL_RenderDrawLine(renderer, x,y+h-1,x+w-1,y+h-1);
	*/
}
void CSDL_DrawCircle(SDL_Renderer* renderer, int x, int y, int r){
	int i,j;
	j=0;
	i=r;
	int k;
	int f=0;
	for(j=0;i>=j;j++){
		f=j*j+i*i-r*r;
		if(f>0)i--;
		SDL_RenderDrawLine(renderer, x-j, y+i, x+j, y+i);
		SDL_RenderDrawLine(renderer, x-j, y-i, x+j, y-i);
		SDL_RenderDrawLine(renderer, x-i, y+j, x+i, y+j);
		SDL_RenderDrawLine(renderer, x-i, y-j, x+i, y-j);
	}
}

//int boardWidth=25;
//int boardHeight=14;

const int boardHeight=14;
const int boardWidth = boardHeight*((float)SCREEN_WIDTH/SCREEN_HEIGHT);
void printBoard(int * board){
	int i,j;
	printf("\033[0;0H");
	for(i=0;i<boardHeight;i++){
		//printf("p l %2d: ",i);
		for(j=0;j<boardWidth;j++){
			if(board[j+i*boardWidth]==1)printf("\033[40m  \033[m");
			else printf("\033[47m  \033[m");
			//if(board[j+i*boardHeight]==1)printf("[]");
			//else printf("  ");
		}
		printf("\n");
	}
	/*
	for(i=0;i<boardWidth*boardHeight;i++){
		//if(i%boardWidth==0)printf("printing line %2d: ",i/boardWidth);
		//if(board[i]==1)printf("[]");
		//else printf("  ");
		//if(i%boardWidth==boardWidth-1)printf("\n");;
		if(board[i]==1)printf("\033[40m  \033[m");
		else printf("\033[47m  \033[m");
		//if(i%boardWidth==boardWidth-1 && i/boardWidth<boardHeight-2)printf("newline\n");
		if(i%boardWidth==boardWidth-1)printf("\n");
	}*/
}
void updateBoard(int * board0,int * board1){
	int i;
	for(i=0;i<boardWidth*boardHeight;i++){
		int x = i%boardWidth;
		int y = i/boardWidth;
		int j,k;
		int sum=0;
		for(j=0;j<3;j++){
			for(k=0;k<3;k++){
				if(j!=1 || k!=1){
					int xt = ((j-1+x)+boardWidth)%boardWidth;
					int yt = ((k-1+y)+boardHeight)%boardHeight;
					//printf("(%d,%d) = %d\n",xt,yt,board0[xt+yt*boardWidth]);
					sum+=board0[xt+boardWidth*yt];
				}
			}
		}
		if(sum==2)board1[x+y*boardWidth]=board0[x+y*boardWidth];
		else if(sum==3)board1[x+y*boardWidth]=1;
		else board1[x+y*boardWidth]=0;
		//getchar();
		//printf("\n\n");
	}
}


void drawBoard(SDL_Renderer * renderer, int * board){
	int i,j;
	float square_side = (float)SCREEN_HEIGHT/boardHeight;
	for(i=0;i<boardHeight;i++){
		for(j=0;j<boardWidth;j++){
			if(board[j+i*boardWidth]==1)SDL_SetRenderDrawColor(renderer,0.1*255,0.1*255,0.9*255,255);
			else SDL_SetRenderDrawColor(renderer,0,0,0,255);
			CSDL_DrawRectangle(renderer,j*square_side,i*square_side,square_side,square_side);
		}
	}
}
int main(int argc, char ** argv) 
{ 
	srand(time(0));
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) { 
		printf("error initializing SDL: %s\n", SDL_GetError()); 
	} 
	SDL_Window* window = SDL_CreateWindow("GAME", // creates a window 
									SDL_WINDOWPOS_CENTERED, 
									SDL_WINDOWPOS_CENTERED, 
									SCREEN_WIDTH, SCREEN_HEIGHT, 0); 
	Uint32 render_flags = SDL_RENDERER_ACCELERATED; 
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, render_flags); 
	TTF_Init();
	int textDisplay(SDL_Renderer * renderer, char * textInput, int textSize, int x, int y, int r, int g, int b, int outline_size)
	{
		TTF_Font *Buran = TTF_OpenFont("Buran.ttf", textSize); //this opens a font style and sets a size
		TTF_Font *font_outline = TTF_OpenFont("Buran.ttf", textSize); 
		TTF_SetFontOutline(font_outline, outline_size); 
		SDL_Color textColor = {r, g, b, 0};
		SDL_Color inverseTextColor = {255 - r, 255 - g, 255 - b, 0};
		SDL_Surface* surfaceMessage = TTF_RenderText_Blended(Buran, textInput, textColor); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
		SDL_Surface* surfaceOutline = TTF_RenderText_Blended(font_outline, textInput, inverseTextColor); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
		TTF_CloseFont(Buran);
		TTF_CloseFont(font_outline);
		SDL_Rect rect = { outline_size, outline_size, surfaceMessage->w, surfaceMessage->h}; 
		SDL_SetSurfaceBlendMode(surfaceMessage, SDL_BLENDMODE_BLEND);
		SDL_BlitSurface(surfaceMessage, NULL, surfaceOutline, &rect);
		//SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage); //now you can convert it into a texture
		SDL_Texture* Outline = SDL_CreateTextureFromSurface(renderer, surfaceOutline); //now you can convert it into a texture
		SDL_FreeSurface(surfaceMessage); 
		SDL_FreeSurface(surfaceOutline); 
		SDL_Rect Outline_rect; //create a rect
		SDL_QueryTexture(Outline, NULL, NULL, &Outline_rect.w, &Outline_rect.h); 
		Outline_rect.x = x-(Outline_rect.w/2);  //controls the rect's x coordinate 
		Outline_rect.y = y-(Outline_rect.h/2); // controls the rect's y coordinte
		SDL_RenderCopy(renderer, Outline, NULL, &Outline_rect); 
		SDL_DestroyTexture(Outline); 	
	}
	// controls annimation loop 
	int close = 0; 
	int pause = 1;
	// annimation loop 
	int keyboard[1000];
	int keyboardprev[1000];
	int keyboardonce[1000];
	int keyboarddown[1000];
	int keyboardup[1000];
	int mouse[10];
	int mouseprev[10];
	int mouseonce[10];
	int mousedown[10];
	int mouseup[10];
	unsigned int counter;
	int lastTime = SDL_GetTicks();
	memset(keyboard,0,sizeof keyboard);



	
	
	int mouseX;
	int mouseY;

	int i;
	int board0[boardWidth*boardHeight];
	int board1[boardWidth*boardHeight];
	int * board0p = board0;
	int * board1p = board1;
	float square_side = (float)SCREEN_HEIGHT/boardHeight;
	int generation=0;
	float updateSpeed=1;
	for(i=0;i<boardWidth*boardHeight;i++){
		//board0[i]=rand()&1;
		board0[i]=0;
	}
	//board0[ 0 + 0 * boardWidth] = 1;
	//board0[ 1 + 0 * boardWidth] = 1;
	//board0[ 2 + 0 * boardWidth] = 1;
	//board0[ 2 + 1 * boardWidth] = 1;
	//board0[ 1 + 2 * boardWidth] = 1;
	
	//board0[ 0 + 0 * boardWidth] = 1;
	//board0[ 1 + 0 * boardWidth] = 1;
	//board0[ 0 + 1 * boardWidth] = 1;
	//board0[ 1 + 1 * boardWidth] = 1;
	while (!close) {
		SDL_Event event; 
		// Events mangement 
		while (SDL_PollEvent(&event)) {
			switch (event.type) { 

			case SDL_QUIT: 
				// handling of close button 
				close = 1; 
				break; 
			case SDL_KEYDOWN:
				keyboard[event.key.keysym.scancode] = 1;
				keyboarddown[event.key.keysym.scancode] = 1;
				break;
			case SDL_KEYUP:
				keyboard[event.key.keysym.scancode] = 0;
				keyboardup[event.key.keysym.scancode] = 1;
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouse[event.button.button]=1;
				mousedown[event.button.button]=1;
				break;
			case SDL_MOUSEBUTTONUP:
				mouse[event.button.button]=0;
				mouseup[event.button.button]=1;
				break;
			}
		} 
		for(i=0;i<1000;i++)
		{
			keyboardonce[i] = 0;
			if(!keyboardprev[i] && keyboard[i])keyboardonce[i] = 1;
			keyboardprev[i] = keyboard[i];
		}
		for(i=0;i<10;i++){
			mouseonce[i] = 0;
			if(!mouseprev[i] && mouse[i])mouseonce[i]=1;
			mouseprev[i]=mouse[i];
		}
		if(keyboardonce[SDL_SCANCODE_RETURN])
		{
		}

		if(keyboardonce[SDL_SCANCODE_K])
		{
		}

		if(keyboardonce[SDL_SCANCODE_L])
		{
		}
		if(keyboardonce[SDL_SCANCODE_SPACE])
		{
			pause = !pause;
		}
		if(keyboard[SDL_SCANCODE_DOWN])
		{
		}
		if(keyboard[SDL_SCANCODE_UP])
		{
		}

		SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
		SDL_RenderClear(renderer); 
		// clears the screen 
		
		
		
//MAIN2
drawBoard(renderer,board0p);
if((updateSpeed>=1 && !pause && counter%(int)updateSpeed==0) || keyboardonce[SDL_SCANCODE_N]){
		updateBoard(board0p,board1p);
		int * temp;
		temp = board0p;
		board0p=board1p;
		board1p=temp;
		//usleep(100000);
		generation++;

}
else if(updateSpeed <1 && !pause){
	int k;
	for(k=0;k*updateSpeed<1;k++){
		updateBoard(board0p,board1p);
		int * temp;
		temp = board0p;
		board0p=board1p;
		board1p=temp;
		//usleep(100000);
		generation++;

	}
}
		

		if(keyboardonce[SDL_SCANCODE_S]){
			printf("Saving:\n");
			for(i=0;i<boardWidth*boardHeight;i++){
				printf("%d ",board0p[i]);
                if(i%boardWidth==boardWidth-1)printf("99 \n");
			}
			printf("100 \n");
		}
		if(keyboardonce[SDL_SCANCODE_L]){
			printf("Loading:\n");
            int j=0;
			for(i=0;i<boardHeight;i++){
			    for(j=0;i<boardWidth;j++){
                    scanf("%d",&board0p[j+i*boardWidth]);
                    if(board0p[j+i*boardWidth]==99)break;
                    if(board0p[j+i*boardWidth]==100)break;
                }
                if(board0p[j+i*boardWidth]==100)break;
			}
			printf("\n");
		}
		if(keyboardonce[SDL_SCANCODE_R]){
			generation=0;
			for(i=0;i<boardWidth*boardHeight;i++){
				board0p[i]=rand()&1;
				//board0[i]=0;
			}
		}
		if(keyboard[SDL_SCANCODE_C]){
			generation=0;
			for(i=0;i<boardWidth*boardHeight;i++){
				//board0[i]=rand()&1;
				board0p[i]=0;
			}
		}
		if(keyboardonce[SDL_SCANCODE_PERIOD]){
			if(updateSpeed<=1)updateSpeed=1/(round(1/updateSpeed)+1);
			else updateSpeed--;
		}
		if(keyboardonce[SDL_SCANCODE_COMMA]){
			if(updateSpeed<1)updateSpeed=1/(round(1/updateSpeed)-1);
			else updateSpeed++;
		}
		int j;
		if(mouse[SDL_BUTTON_LEFT]){
			SDL_GetMouseState(&mouseX,&mouseY);
			//SDL_SetRenderDrawColor(renderer,0,0,255,255);
			//CSDL_DrawRectangle(renderer,(mouseX/square_side)*square_side,square_side*(mouseY/square_side),square_side,square_side);
			SDL_SetRenderDrawColor(renderer,255,0,0,255);
			//CSDL_DrawRectangle(renderer,mouseX-5,mouseY-5,10,10);
			CSDL_DrawCircle(renderer,mouseX,mouseY,5);
		}
		if(mouseonce[SDL_BUTTON_LEFT]){
			SDL_GetMouseState(&mouseX,&mouseY);
			board0p[(int)(mouseX/square_side) + (int)(mouseY/square_side)*boardWidth]^=1;
		}
		char str[100];
		char buffer[100]="generation: ";
		sprintf(str,"%5d",generation);
		strcat(buffer,str);
		textDisplay(renderer,buffer, SCREEN_WIDTH/30, SCREEN_WIDTH/2, SCREEN_HEIGHT/15, 255, 255, 255, 3);
		sprintf(buffer, "speed: %.4f",1/updateSpeed);
		textDisplay(renderer,buffer, SCREEN_WIDTH/30, SCREEN_WIDTH/2, 2*SCREEN_HEIGHT/15, 255, 255, 255, 3);
		
		// triggers the double buffers 
		// for multiple rendering 
		SDL_RenderPresent(renderer); 

		/*//ANIMATED WALLPAPER
		//SDL_Delay(1000);
    		SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_PIXELFORMAT_ARGB8888);
    		SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, surface->pixels, surface->pitch);
    		//SDL_SaveBMP(surface, "screenshot.bmp");
    		//IMG_SavePNG(surface, "screenshot.png");
    		IMG_SavePNG(surface, "screenshot.png");
		//system("/usr/bin/feh --bg-scale screenshot.bmp");
		system("/usr/bin/feh --bg-scale screenshot.png");
		//system("xwallpaper --maximize screenshot.jpg");
		SDL_FreeSurface(surface);
		*/



		// calculates to 60 fps 
		counter++;
		SDL_Delay(1000 / FPS);
	} 

	//SDL CLEANUP
	
	// destroy renderer 
	SDL_DestroyRenderer(renderer); 

	// destroy window 
	SDL_DestroyWindow(window);	
	//END OF SDL CLEANUP
	return 0; 
} 

