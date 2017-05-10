#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <SDL.h>
#include <stdio.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <ctime>
#include <Windows.h>
#include <thread>
#include <SDL_ttf.h>
Uint32 white;
using namespace std;

int scoreNum;
double dx;
int attempt;
//TTF_Font *font = NULL;
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
		//cout << w << endl;
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
	double x, dx, ax, y, dy, ay;
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
	//double dx;
public:
	virtual void init(const char *gameName, int maxW = 1000, int maxH = 580, int
		startX = 100, int startY = 100) {
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
			std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
			return;
		}
		if (TTF_Init() == -1)
		{
			std::cout << "TTF_Init Error: " << SDL_GetError() << std::endl;
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

		TTF_Quit();
		Mix_Quit();
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
	sign(char _t){
		target = _t;
	}
public:
	bool hit(char typed){
		if (typed == target)
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
void numattempt(){}
class in
{
	sign* target;
	bool ishit;
public:
	bool isdone;
	bool aButton;
	bool sButton;
	bool dButton;
	bool fButton;
	bool gButton;
	//double dx;
	Mix_Chunk *correctPress;
	Mix_Chunk *wrongPress;
	SDL_Renderer* ren;
	
	Animation buttonASprite;
	in(sign* _t, Animation a, SDL_Renderer* r){
		target = _t;
		isdone = false;
		buttonASprite = a;
		ren = r;
	}

	void run(){
		dx = 0.0;
		attempt = 0;
		while (!isdone){
			correctPress = Mix_LoadWAV("ding1.wav");
			wrongPress = Mix_LoadWAV("negativebeep.wav");
			if (!target->getIsHit()){
				if (aButton){
					if (target->hit('a')){
						cout << "hit" <<endl;
						Mix_PlayChannel(-1, correctPress, 0);
						scoreNum+=1;
						dx -= .2;
						cout << "from run "<< dx << endl;
						//buttonASprite.addFrame(new AnimationFrame(ren, "btnAactive.bmp"));
					}
					else {
						Mix_PlayChannel(-1, wrongPress, 0);
						attempt += 1;
					}
					aButton = false;
					
				}
				
				if (sButton){
					if (target->hit('s')){
						cout << "hit" << endl;
						Mix_PlayChannel(-1, correctPress, 0);
						
						scoreNum += 1;
						dx -= .2;
						cout << "from run " << dx << endl;
					}
					else{
						Mix_PlayChannel(-1, wrongPress, 0);
						attempt += 1;
					}
					sButton = false;
					
				}
				
				if (dButton){
					if (target->hit('d')){
						cout << "hit" << endl;
						Mix_PlayChannel(-1, correctPress, 0);
						scoreNum += 1;
						dx -= .2;
						cout << "from run " << dx << endl;
					}
					else {
						Mix_PlayChannel(-1, wrongPress, 0);
						attempt += 1;
					}
					dButton = false;
					
				}
				
				if (fButton){
					if (target->hit('f')){
						cout << "hit" << endl;
						Mix_PlayChannel(-1, correctPress, 0);
					
						scoreNum += 1;
						dx -= .2;
						cout << "from run " << dx << endl;
					}
					else{
						Mix_PlayChannel(-1, wrongPress, 0);
						attempt += 1;
					}
					fButton = false;
					
				}
				
				if (gButton){
					if (target->hit('g')){
						cout << "hit" << endl;
						Mix_PlayChannel(-1, correctPress, 0);
						
						scoreNum += 1;
						dx -= .2;
						cout << "from run " << dx << endl;
					}
					else {
						Mix_PlayChannel(-1, wrongPress, 0);
						attempt += 1; 
					}
					gButton = false;
					
				}
				
			}
			cout << attempt << endl;
		
			
			
		}
		
		Mix_FreeChunk(correctPress);
		Mix_FreeChunk(wrongPress);
		
		
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
	Animation score;
	Animation tracks;
	Animation attempt1;
	Animation attempt2;
	Animation attempt3;
	Animation attempt4;
	Animation gameover;
	sign *target;
	thread* thred;
	in *i;
	int x, y;
	//double dx; 
	double dy;
	SDL_Surface *message;
	SDL_Color textColor;
	SDL_Texture *text;
	SDL_Rect textRect;
	TTF_Font *font;
public:
	bool OMEGA = false;
	bool pause = false;
	const int maxtrains = 15;
	int wagonarray[15];
	int w, h;
    //double dx;
	public:
	void init(const char *gameName = "Color Tracks", int maxW = 640, int maxH = 480, int
		startX = 10, int startY = 100){
		playingm = false;
		Game::init(gameName);

		font = TTF_OpenFont("arial.ttf", 30);
		if (font == NULL){

			std::cout << "Font Error: " << SDL_GetError() <<
				std::endl;
		}

		textColor.r = 255, textColor.g = 255, textColor.b = 255;

		for (int i = 1; i <= 5; i++)
		{
			stringstream ss;
			ss << "tstart" << i << ".bmp";
			startpage.addFrame(new AnimationFrame(ren, ss.str().c_str(), 200));
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
		score.addFrame(new AnimationFrame(ren, "score.bmp"));
		tracks.addFrame(new AnimationFrame(ren, "bltracks.bmp"));
		gameover.addFrame(new AnimationFrame(ren, "gameover1.bmp"));
		attempt1.addFrame(new AnimationFrame(ren, "attempt1.bmp"));
		attempt2.addFrame(new AnimationFrame(ren, "attempt2.bmp"));
		attempt3.addFrame(new AnimationFrame(ren, "attempt3.bmp"));
		attempt4.addFrame(new AnimationFrame(ren, "attempt4.bmp"));
		//dx = -1.0;
		//dy = 1;
		x = 900;
		y = 0;
		
		
		
		srand(time(NULL));
		for (int i = 0; i < maxtrains - 1; i++)
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
		i = new in(target, buttonA, ren);
		thred = new thread(&in::run, i);
	}
	
	void handleEvent(SDL_Event &event) {

		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
			std::cout << "Audio ERROR:" << Mix_GetError() << std::endl;
		}
		/*Mix_Chunk *backgroundM = Mix_LoadWAV("funnysong.wav");

		if (event.type == SDL_KEYDOWN&&playingm == false){
			if (event.key.keysym.sym == SDLK_9)
			{
				Mix_PlayChannel(-1, backgroundM, -1);
				playingm = true;
			}
				
		}
			Mix_FreeChunk(backgroundM);*/
		if (event.type == SDL_KEYDOWN){
			if (event.key.keysym.sym == SDLK_p)
			{
				//image for pressed button
				if (pause == false){
					dx = 0;
					pause = true;
				}
				else{
					dx = -1.5;
					pause = false;
				}
			}
		}
		
		if (event.type == SDL_KEYDOWN){
			if (event.key.keysym.sym == SDLK_RETURN)
			{
				startpage.destroy();
			}
			if (event.key.keysym.sym == SDLK_a)
			{
				i->aButton = true;

			}
			if (event.key.keysym.sym == SDLK_s)
			{
				i->sButton = true;
				
			}
			if (event.key.keysym.sym == SDLK_d)
			{
				i->dButton = true;
				
			}
			if (event.key.keysym.sym == SDLK_f)
			{
				i->fButton = true;
				
			}
			if (event.key.keysym.sym == SDLK_g)
			{
				i->gButton = true;
				
			}
			if (event.key.keysym.sym == SDLK_q)
			{
				i->isdone = true;
				
			}
		}
	}

	void show() {
		gameover.show(ren,ticks);
		a.show(ren, ticks);
		tracks.show(ren, ticks, 0, 400);
		attempt1.show(ren, ticks, 925, 10);
		attempt2.show(ren, ticks, 855, 10);
		attempt3.show(ren, ticks, 785, 10);
		attempt4.show(ren, ticks, 712, 10);
		for (int i = 0; i < maxtrains; i++)
		{
			
			int num;
			num = wagonarray[i];
			if (wagon1.getX() % 100 == 1) target->setIsHit(false);
				
			if (num == 1){
				wagon1.show(ren, ticks, x + (i * 101), 320);
				if (wagon1.getX() <= 500 && wagon1.getX() + 100 >= 500) target->change('g');
				
			}
			else if (num == 2){
				wagon2.show(ren, ticks, x + (i * 101), 320);
				if (wagon2.getX() <= 500 && wagon2.getX() + 100 >= 500) target->change('d');
				
			}
			
			else if (num == 3){
				wagon3.show(ren, ticks, x + (i * 101), 320);
				if (wagon3.getX() <= 500 && wagon3.getX() + 100 >= 500) target->change('a');
				
			}

			else if (num == 4){
				wagon4.show(ren, ticks, x + (i * 101), 320);
				if (wagon4.getX() <= 500 && wagon4.getX() + 100 >= 500) target->change('f');
				
			}

			else{
				wagon5.show(ren, ticks, x + (i * 101), 320);
				if (wagon5.getX() <= 500 && wagon5.getX() + 100 >= 500) target->change('s');
				
			}

			OMEGA = true;

		}

		locomotive.show(ren, ticks, x - 175, 293);
		buttonA.show(ren, ticks, 240, 470);
		buttonS.show(ren, ticks, 340, 470);
		buttonD.show(ren, ticks, 450, 470);
		buttonF.show(ren, ticks, 560, 470);
		buttonG.show(ren, ticks, 660, 470);
		smoke.show(ren, ticks, x - 175, y);
		
		trainsign.show(ren, ticks, 10, 40);
		score.show(ren, ticks, 20, 25);
		startpage.show(ren, ticks);
		stringstream scoreN;
		scoreN << scoreNum;
		message = TTF_RenderText_Solid(font, scoreN.str().c_str(), textColor);
		//cout <<scoreNum <<endl;
		text = SDL_CreateTextureFromSurface(ren, message);
		if (text == NULL){
			std::cout << "text SDL_CreateTextureFromSurface Error: " << SDL_GetError() <<
				std::endl;
			SDL_Quit(); 
		}
		
		SDL_QueryTexture(text, NULL, NULL, &w, &h);
		textRect.x = 135; textRect.y = 19; textRect.w = w; textRect.h = h;
		SDL_RenderCopy(ren, text, NULL, &textRect);
		////SDL_RenderPresent(ren);
		SDL_DestroyTexture(text);
		SDL_FreeSurface(message);

		x += dx;
		y += dy;
		
		if (attempt == 1)
		{
			attempt4.destroy();
		}
		if (attempt == 2)
		{
			attempt3.destroy();
		}
		if (attempt == 3)
		{
			attempt2.destroy();
		}
		if (attempt == 4)
		{
			attempt1.destroy();
		}
		if (attempt == 4)
		{
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
			trainsign.destroy();
			score.destroy();
			tracks.destroy();
			SDL_DestroyTexture(text);
			//SDL_FreeSurface(message);*/
		}
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
		trainsign.destroy();
		score.destroy();
		tracks.destroy();
		gameover.destroy();
		attempt1.destroy();
		attempt2.destroy();
		attempt3.destroy();
		attempt4.destroy();
		TTF_CloseFont(font);
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