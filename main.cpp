#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <SDL.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <SDL_mixer.h>
#include <ctime>
#include <Windows.h>
#include <thread>

using namespace std;

class AnimationFrame {
	SDL_Texture *frame;
	int time, w, h; // ms
public:
	AnimationFrame(SDL_Texture *newFrame, int newTime = 100) {
		frame = newFrame;
		time = newTime;
	}
	AnimationFrame(SDL_Renderer *ren, const char *imagePath, int newTime = 100){
		SDL_Surface *bmp = SDL_LoadBMP(imagePath);
		if (bmp == NULL){
			std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
			SDL_Quit();
		}
		SDL_SetColorKey(bmp, SDL_TRUE, SDL_MapRGB(bmp->format, 0, 255, 0));
		w = bmp->w;
		cout << w << endl;
		h = bmp->h;
		frame = SDL_CreateTextureFromSurface(ren, bmp);
		SDL_FreeSurface(bmp);
		if (frame == NULL){
			std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() <<
				std::endl;
			SDL_Quit();
		}
		time = newTime;
	}
	void show(SDL_Renderer *ren, int x = 0, int y = 0){
		SDL_Rect src, dest;
		dest.x = x; dest.y = y; dest.w = w; dest.h = h;
		src.x = 0; src.y = 0; src.w = w; src.h = h;
		SDL_RenderCopy(ren, frame, &src, &dest);
	}
	int getTime() {
		return time;
	}
	void destroy() {
		SDL_DestroyTexture(frame);
	}
};

class Animation {
protected:
	vector<AnimationFrame *> frames;
	int totalTime;
	int x;
public:
	Animation() {
		totalTime = 0;
	}
	void addFrame(AnimationFrame *c) {
		frames.push_back(c);
		totalTime += c->getTime();
	}
	virtual void show(SDL_Renderer *ren, int time /*ms*/, int _x = 0, int y = 0) {
		x = _x;
		int aTime = time % totalTime;
		int tTime = 0;
		unsigned int i = 0;
		for (i = 0; i<frames.size(); i++) {
			tTime += frames[i]->getTime();
			if (aTime <= tTime) break;
		}
		frames[i]->show(ren, x, y);
	}
	int getX(){
		return x;
	}
	virtual void destroy() {
		for (unsigned int i = 0; i<frames.size(); i++)
			frames[i]->destroy();
	}
};
class Sprite : public Animation {
	float x, dx, ax, y, dy, ay;
public:
	Sprite() :Animation() {
		x = 0.0; dx = 0.0; ax = 0.0;
		y = 0.0; dy = 0.0; ay = 0.0;
	}
	void addFrames(SDL_Renderer *ren, const char imagePath, int count) {
	}

};

class Game {
protected:
	SDL_Window *win;
	SDL_Renderer *ren;
	int ticks;
	bool finished;
public:
	virtual void init(const char *gameName, int maxW = 1000, int maxH = 580, int
		startX = 100, int startY = 100) {
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
			std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
			return;
		}
		win = SDL_CreateWindow(gameName, startX, startY, maxW, maxH, SDL_WINDOW_SHOWN);
		if (win == NULL){
			std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
			SDL_Quit();
			return;
		}
		ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED |
			SDL_RENDERER_PRESENTVSYNC);

		if (ren == NULL){
			SDL_DestroyWindow(win);
			std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
			SDL_Quit();
			return;
		}
	}

	virtual void done() {
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
//		Mix_Quit();//audio
		SDL_Quit();
	}
	void run() {

		int start = SDL_GetTicks();
		finished = false;
		while (!finished) {
			SDL_Event event;
			if (SDL_PollEvent(&event)) {
				if (event.type == SDL_WINDOWEVENT) {
					if (event.window.event == SDL_WINDOWEVENT_CLOSE)
						finished = true;
				}
				if (event.type == SDL_KEYDOWN) {
					if (event.key.keysym.sym == SDLK_ESCAPE)
						finished = true;
				}

				if (!finished) handleEvent(event);
			}
			ticks = SDL_GetTicks();
			SDL_RenderClear(ren);
			show();
			SDL_RenderPresent(ren);
		}
		int end = SDL_GetTicks();
		cout << "FPS " << (300.0*1000.0 / float(end - start)) << endl;
	}
	virtual void show() = 0;
	virtual void handleEvent(SDL_Event &event) = 0;
};

class sign{
	volatile char target;
	bool ishit = false;

	public:
	sign( char _t){
		target = _t;
	}
	public:
		bool hit(char typed){
			if(typed == target)
			{
				ishit = true;
				return true;
			}
			else return false;
		}
	bool getIsHit(){
		return ishit;
	}
	void setIsHit(bool _h){
		ishit = _h;
	}
	void change(char c){
		target = c;
	}
};

class in
{
	sign* target;
	bool ishit;
	bool isdone;
	public:
	in(sign* _t){
		target = _t;
		isdone = false;
	}
	public:
		void run(){
			while(!isdone){
				if(!target->getIsHit()){
					SDL_Event event;
					if (SDL_PollEvent(&event))
					{
						if (event.type == SDL_KEYDOWN){
							cout << "t";
						if (event.key.keysym.sym == SDLK_a)
						{
							if(target->hit('a')) cout << "hit" << endl;
						}
						if (event.key.keysym.sym == SDLK_s)
						{
							if(target->hit('s')) cout << "hit" << endl;
						}
						if (event.key.keysym.sym == SDLK_d)
						{
							if(target->hit('d')) cout << "hit" << endl;
						}
						if (event.key.keysym.sym == SDLK_f)
						{
							if(target->hit('f')) cout << "hit" << endl;	
						}
						if (event.key.keysym.sym == SDLK_g)
						{
							if(target->hit('g')) cout << "hit" << endl;
						}
						if (event.key.keysym.sym == SDLK_q)
						{
							isdone = true;
						}
					}
				}
			}
		}
	}
};

class monorailGame :public Game {
	bool playingm;
	Animation startpage;
	Animation a;
	Animation train;
	Animation buttonA;//
	Animation buttonS;//
	Animation buttonD;//
	Animation buttonF;//
	Animation buttonG;//
	Animation smoke;
	Animation locomotive;
	Animation wagon1;
	Animation wagon2;
	Animation wagon3;
	Animation wagon4;
	Animation wagon5;
	Animation trainsign;
	sign *target;
	thread* thred;
	in *i;
	int x, y;
	int dx, dy;

public:
bool OMEGA=false;
const int maxtrains = 15;
int wagonarray[];

	void init(const char *gameName = "Color Tracks", int maxW = 640, int maxH = 480, int
		startX = 10, int startY = 100){
		playingm = false;
		Game::init(gameName);
		for (int i = 1; i <= 5; i++)
		{
			stringstream ss;
			ss << "tstart" << i << ".bmp";
			startpage.addFrame(new AnimationFrame(ren, ss.str().c_str(), 500));
		}
		
		a.addFrame(new AnimationFrame(ren, "desert.bmp"));
		buttonA.addFrame(new AnimationFrame(ren, "btnA.bmp"));
		buttonD.addFrame(new AnimationFrame(ren, "btnD.bmp"));
		buttonS.addFrame(new AnimationFrame(ren, "btnS.bmp"));
		buttonF.addFrame(new AnimationFrame(ren, "btnF.bmp"));
		buttonG.addFrame(new AnimationFrame(ren, "btnG.bmp"));
		locomotive.addFrame(new AnimationFrame(ren, "traindriver.bmp"));
		wagon1.addFrame(new AnimationFrame(ren, "wagon1.bmp"));
		wagon2.addFrame(new AnimationFrame(ren, "wagon2.bmp"));
		wagon3.addFrame(new AnimationFrame(ren, "wagon3.bmp"));
		wagon4.addFrame(new AnimationFrame(ren, "wagon4.bmp"));
		wagon5.addFrame(new AnimationFrame(ren, "wagon5.bmp"));
		trainsign.addFrame(new AnimationFrame(ren, "trainsign2.bmp"));
		
		dx = -1.5;
		//dy = 1;
		x = 900;
		y = 0;

		srand(time(NULL));
		for (int i = 0; i < maxtrains-1; i++)
		{
			int num = rand() % 5 + 1;
			wagonarray[i] = num;
		}

		 
		
		for (int i = 1; i <= 6; i++) {
			stringstream ss;
			ss << "smoke" << i << ".bmp";
			smoke.addFrame(new AnimationFrame(ren, ss.str().c_str(), 300));
		}
		target = new sign('p');
		i = new in(target);
		thred = new thread(&in::run, i);
	}

	void handleEvent(SDL_Event &event) {
		
//		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
//			std::cout << "Audio ERROR:" << Mix_GetError() << std::endl;
		//}
		//Mix_Chunk *backgroundM = Mix_LoadWAV("GameMoozik.wav");

		if (event.type == SDL_KEYDOWN&&playingm==false){
			if (event.key.keysym.sym == SDLK_9)
			{
			//	Mix_PlayChannel(-1, backgroundM, 3);
				playingm = true;
			}
		}
		

		if (event.type == SDL_KEYDOWN){
			if (event.key.keysym.sym == SDLK_RETURN)
			{
				startpage.destroy();
			}
			
		}

			if (event.type == SDL_KEYDOWN){
				if (event.key.keysym.sym == SDLK_a)
				{
					//image for pressed button
					//buttonA.addFrame(new AnimationFrame(ren, "btnAactive.bmp"));
				
					//set dx so train starts until one of the buttons is pressed
					//gains speed as the user inputs correct matches
					//game over screen if they loose
					//dx = -2;
				}
			}
			if (event.type == SDL_KEYUP){
				if (event.key.keysym.sym == SDLK_a)
				{
					
				}
			}
			if (event.type == SDL_KEYDOWN){
				if (event.key.keysym.sym == SDLK_s)
				{
					//image for pressed button
					//buttonA.addFrame(new AnimationFrame(ren, "btnSactive.bmp"));
					
				}
			}

			if (event.type == SDL_KEYDOWN){
				if (event.key.keysym.sym == SDLK_d)
				{
					//image for pressed button
					//buttonA.addFrame(new AnimationFrame(ren, "btnDactive.bmp"));
					
				}
			}
			if (event.type == SDL_KEYDOWN){
				if (event.key.keysym.sym == SDLK_f)
				{
					//image for pressed button
					//buttonA.addFrame(new AnimationFrame(ren, "btnFactive.bmp"));
					
				}
			}

			if (event.type == SDL_KEYDOWN){
				if (event.key.keysym.sym == SDLK_g)
				{
					//image for pressed button
					//buttonA.addFrame(new AnimationFrame(ren, "btnGactive.bmp"));
					
				}
			}
		
	}
	void show() {
		
		a.show(ren, ticks);
		
		
		for (int i = 0; i < maxtrains; i++)
		{
			int num;
			num = wagonarray[i];
				if(wagon1.getX() % 100 == 0) target->setIsHit(false);
		
				if (num == 1){
					wagon1.show(ren, ticks, x + (i * 101), 320);
					if(wagon1.getX() <= 500 && wagon1.getX() + 100 >= 500) target->change('g');}
				else if (num == 2){
					wagon2.show(ren, ticks, x + (i * 101), 320);
					if(wagon2.getX() <= 500 && wagon2.getX() + 100 >= 500) target->change('d');}

				else if (num == 3){
					wagon3.show(ren, ticks, x + (i * 101), 320);
					if(wagon3.getX() <= 500 && wagon3.getX() + 100 >= 500) target->change('a');}

				else if (num == 4){
					wagon4.show(ren, ticks, x + (i * 101), 320);
					if(wagon4.getX() <= 500 && wagon4.getX() + 100 >= 500) target->change('f');}

				else{
					wagon5.show(ren, ticks, x + (i * 101), 320);
					if(wagon5.getX() <= 500 && wagon5.getX() + 100 >= 500) target->change('s');}
				OMEGA = true;
			
		}

		locomotive.show(ren, ticks, x - 175, 293);
		buttonA.show(ren, ticks, 240, 470);
		buttonS.show(ren, ticks, 340, 470);
		buttonD.show(ren, ticks, 450, 470);
		buttonF.show(ren, ticks, 560, 470);
		buttonG.show(ren, ticks, 660, 470);
		smoke.show(ren, ticks, x-175, y);
		trainsign.show(ren, ticks, 10, 40);

		startpage.show(ren, ticks);
		x += dx;
		y += dy;
	}

	void done() {
		a.destroy();
		startpage.destroy();
		buttonA.destroy();
		buttonS.destroy();
		buttonD.destroy();//
		buttonF.destroy();//
		buttonG.destroy();//
		smoke.destroy();
		locomotive.destroy();
		wagon1.destroy();
		wagon2.destroy();
		wagon3.destroy();
		wagon4.destroy();
		wagon5.destroy();
		thred->join();
		Game::done();
	}
};

int main(int argc, char **argv) {
	monorailGame g;
	g.init();
	g.run();
	g.done();
	return 0;
}