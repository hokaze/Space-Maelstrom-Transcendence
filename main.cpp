// Space Maelstrom Transcendence - because over-the-top names for the sake of an in-joke
// are always worth it! I swear I'll split this file someday...

// Visual Studio needs slightly different include statements
#ifdef _WIN32
	#include "SDL.h"
	#include "SDL_image.h"
	#include "SDL_mixer.h"
	#include "SDL_ttf.h"
#endif
// Normal SDL includes
#ifndef _WIN32
	#include "SDL/SDL.h"
	#include "SDL/SDL_image.h"
	#include "SDL/SDL_mixer.h"
	#include "SDL/SDL_ttf.h"
#endif
// Standard includes
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream> // DEBUG
using namespace std;


//////////////////////////////////////////////////////////////////////
// Constants, classes and prototypes                                //
//////////////////////////////////////////////////////////////////////

// Define our constants for the "game"
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;
bool DEBUG = true;
bool FULLSCREEN = false;
int MAX_FPS = 60;

// The dimensions of the ship and its weapons
const int SHIP_WIDTH = 32;
const int SHIP_HEIGHT = 32;
const int B1_WIDTH = 10;
const int B1_HEIGHT = 12;
const int B1_SHOTS = 30;
const int B1_COOLDOWN = 15;
const int B1_ATTACK = 10;
const int B2_WIDTH = 14;
const int B2_HEIGHT = 18;
const int B2_SHOTS = 80;
const int B2_COOLDOWN = 30;
const int B2_ATTACK = 5;

// Obstacle related stuff
const int AS1_HEIGHT = 50;
const int AS1_WIDTH = 50;
const int AS1_MAX = 20;
const int AS1_HP = 5;
const int AS1_ATTACK = 10;
const int D1_HEIGHT = 20;
const int D1_WIDTH = 20;
const int D1_MAX = AS1_MAX * 4;
const int D1_ATTACK = 2;

// Enemy related stuff
const int E1_HEIGHT = 30;
const int E1_WIDTH = 26;
const int E1_MAX = 10;
const int E1_HP = 10;
const int E1_ATTACK = 5;
const int E1_EXPLODE_TIME = MAX_FPS / 4;
const int B3_WIDTH = 8;
const int B3_HEIGHT = 12;
const int B3_SHOTS = 4;
const int B3_COOLDOWN = 60;
const int B3_ATTACK = 5;

// Setup surfaces, event system and BGM
SDL_Surface *title_bg = NULL;
SDL_Surface *title_selection = NULL;
SDL_Surface *help_popup = NULL;
SDL_Surface *highscore_popup = NULL;
SDL_Surface *ship = NULL;
SDL_Surface *bullet = NULL;
SDL_Surface *bullet2 = NULL;
SDL_Surface *bullet3 = NULL;
SDL_Surface *asteroid = NULL;
SDL_Surface *debris = NULL;
SDL_Surface *enemy1 = NULL;
SDL_Surface *boom1 = NULL;
SDL_Surface *background = NULL;
SDL_Surface *stars = NULL;
SDL_Surface *planets = NULL;
SDL_Surface *nebulae = NULL;
SDL_Surface *screen = NULL;

SDL_Event event;
const SDL_Color WHITE = {255,255,255};

Mix_Music *bgm1 = NULL;
Mix_Chunk *shot1 = NULL;
Mix_Chunk *shot2 = NULL;
Mix_Chunk *shot3 = NULL;
Mix_Chunk *break1 = NULL;
Mix_Chunk *break2 = NULL;
Mix_Chunk *menu1 = NULL;
Mix_Chunk *menu2 = NULL;

SDL_Surface *HUD_display = NULL;
TTF_Font *font1 = NULL;

//The area of the sprite sheet
SDL_Rect clips_ship[7];


struct Background
{
	int x, y, scroll_speed;
	SDL_Surface *image;
};

struct Highscore
{
	string name;
	int pos, score;
};

// The ship that will move around on the screen
// classes are by default private
class Ship
{
	// Private variables
	int x_vel, y_vel;
	int frame;
	bool frame_up;
	int hp; int score;
	
	// Initialise the class
	public:
	SDL_Rect box;
	int attack; int b1_cooldown; int b2_cooldown;
	Ship();
	
	// Prototype functions
	void set_clips();
	void handle_input(); // use keypresses to adjust velocity
	void move(); // actually move the ship (and check the bounds of the screen)
	void show(); // blit to screen
	void damage(int attack);
	void increase_score(int points);
	int return_hp();
	int return_score();
};

// The timer - used for capping fps and could be adapted to give certain objects
// a lifespan (e.g. bullets, powerups)
class Timer
{
	int time_elapsed;
	bool started; 
	
	// Initialise the class
	public:
	Timer();
	
	// Prototype functions
	void start();
	void stop();
	int get_time();
	bool is_started();
};

// Basic bullet, expires when it flies off-screen.
class Bullet
{
	int xv, yv;
	//bool alive;
	
	public:
	SDL_Rect box;
	SDL_Surface *sprite;
	bool alive;
	int attack;
	Bullet();
	void setup(int damage, SDL_Surface* bullet_surface);
	void update();
	void fire(int start_x, int start_y, int x_speed, int y_speed, int width, int height);
	void show();
	void die();
};

// Basic obstacle, respawns when it passes bottom of screen or dies
class Asteroid
{
	int hp;
	
	public:
	int xv, yv;
	int attack;
	SDL_Rect box;
	SDL_Surface *sprite;
	bool alive;
	Asteroid();
	void spawn();
	void update();
	void show();
	void damage(int attack);
	void die(bool respawn = true);
};

// Generated upon destruction of Asteroid object
class Debris
{
	int xv, yv;
	
	public:
	int attack;
	SDL_Rect box;
	SDL_Surface* sprite;
	bool alive;
	Debris();
	void spawn(int start_x, int start_y, int x_speed, int y_speed);
	void update();
	void show();
	void die();
};

// Base enemy that just moves down and fires, no AI
class Enemy
{
	int xv, yv;
	int hp;
	
	public:
	int b3_cooldown; int attack;
	int explosion_x; int explosion_y;
	int explosion_timer;
	SDL_Rect box;
	SDL_Surface *sprite;
	SDL_Surface *explosion;
	Bullet b3[B3_SHOTS];
	bool alive;
	Enemy();
	void spawn();
	void update();
	void show();
	void damage(int attack);
	void die();
};

// Prototype functions
SDL_Surface *load_image(string filename);
void apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL);
bool init();
bool load_files();
void clean_quit();
void update_bg(Background& bg, SDL_Surface* display);
bool box_collision(SDL_Rect A, SDL_Rect B);
void update_hud(Ship player, SDL_Surface* HUD, SDL_Surface* screen);
void load_highscores(Highscore table[]);
void show_highscores(Highscore table[], SDL_Surface* HUD, SDL_Surface* screen);


//////////////////////////////////////////////////////////////////////
// MAIN LOOP                                                        //
//////////////////////////////////////////////////////////////////////

int main(int argc, char* args[]) // standard SDL setup for main()
{	
	// Check command-line arguments
	for (int i = 0; i < argc; i++)
	{
		if ((string(args[i]) == "-fullscreen") || (string(args[i]) == "-f"))
		{
			FULLSCREEN = true;
		}
		else if ((string(args[i]) == "-debug") || (string(args[i]) == "-d"))
		{
			DEBUG = true;
		}
	}
	
	// Initialise everything
	if( init() == false )
	{
		return 1;
	}
	
	// Load the images and any audio
	if( load_files() == false )
	{
		return 1;
	}
	
	bool quit = false; // quit flag
	Highscore score_table[10]; // highscores has 10 entries
	bool menu = true; bool help = false; bool view_scores = false;
	int menu_item  = 0; int menu_action = 0;
	srand(time(NULL)); // initialise random seed
	Timer fps; // frame rate regulator
	fps.start(); // start timer
	SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE); // ignore mouse events
	load_highscores(score_table);
	
	// DEBUG
	/*for (int i = 0; i < 10; i++)
	{
		cout << score_table[i].pos << " " << score_table[i].name << " " << score_table[i].score << endl;
	}*/
	
	// Main menu
	while (menu)
	{
		if (SDL_PollEvent(&event))
		{
			if (event.type = SDL_KEYDOWN)
			{
				if (help)
				{
					help = false;
				}
				if (view_scores)
				{
					view_scores = false;
				}
				if (event.key.keysym.sym == SDLK_UP)
				{
					if (menu_item > 0)
					{
						menu_item--;
						menu_action = 1;
					}
				}
				if (event.key.keysym.sym == SDLK_DOWN)
				{
					if (menu_item < 6)
					{
						menu_item++;
						menu_action = 1;
					}
				}
				if ((event.key.keysym.sym == SDLK_z) || (event.key.keysym.sym == SDLK_RETURN))
				{
					if (menu_item == 0)
					{
						menu = false;
						menu_action = 2;
					}
					else if (menu_item == 2)
					{
						view_scores = true;
						menu_action = 2;
					}
					else if (menu_item == 4)
					{
						help = true;
						menu_action = 2;
					}
					else if (menu_item == 6)
					{
						menu = false;
						quit = true;
					}
				}
				if (event.key.keysym.sym == SDLK_F1)
				{
					help = true;
				}
				if (event.key.keysym.sym == SDLK_F2)
				{
					menu = false;
				}
				if (event.key.keysym.sym == SDLK_F3)
				{
					view_scores = true;
				}
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					quit = true;
					menu = false;
				}
			}
			// If the user has closed the program or hit ESC
			if (event.type == SDL_QUIT)
			{
				quit = true;
				menu = false;
			}
		}
		
		// Play menu sound effects
		if (menu_action == 1)
		{
			Mix_PlayChannel(-1, menu1, 0);
		}
		if (menu_action == 2)
		{
			Mix_PlayChannel(-1, menu2, 0);
		}
		
		menu_action = 0;
		
		// Draw to display
		apply_surface(0, 0, title_bg, screen);
		if (menu_item == 0)
		{
			apply_surface(0, 205, title_selection, screen);
		}
		else if (menu_item == 2)
		{
			apply_surface(0, 270, title_selection, screen);
		}
		else if (menu_item == 4)
		{
			apply_surface(0, 335, title_selection, screen);
		}
		else if (menu_item == 6)
		{
			apply_surface(0, 400, title_selection, screen);
		}

		if (help)
		{
			apply_surface(0, 0, help_popup, screen);
		}
		
		if (view_scores)
		{
			apply_surface(0, 0, highscore_popup, screen);
			show_highscores(score_table, HUD_display, screen);
		}

		SDL_Flip(screen); // update screen
		
		// Cap the frame rate to our specified maximum fps
		if(fps.get_time() < 1000 / MAX_FPS)
		{
			SDL_Delay((1000 / MAX_FPS) - fps.get_time());
		}
	}
	
	// Setup backgrounds
	Background distant = {0, 0, 1, background};
	Background starfield = {0, 0, 2, stars};
	Background planetoids = {0, 0, 3, planets};
	Background nebula = {0, 0, 5, nebulae};
	
	Ship player_ship; // setup the ship

	Bullet b1[B1_SHOTS]; // setup primitive bullet
	for (int i = 0; i < B1_SHOTS; i++)
	{
		b1[i].setup(B1_ATTACK, bullet);
	}
	int b1_cooldown = B1_COOLDOWN;
	bool b1_firing = false;

	Bullet b2[B2_SHOTS];
	for (int i = 0; i < B2_SHOTS; i++)
	{
		b2[i].setup(B2_ATTACK, bullet2);
	}
	int b2_cooldown = B2_COOLDOWN;
	bool b2_firing = false;

	Asteroid a1[AS1_MAX];
	Debris d1[D1_MAX];
	
	Enemy e1[E1_MAX];
	for (int i = 0; i < E1_MAX; i++)
	{
		for (int j = 0; j < B3_SHOTS; j++)
		{
			e1[i].b3[j].setup(B3_ATTACK, bullet3);
		}
	}
	
	for (int i = 0; i < AS1_MAX; i++)
	{
		a1[i].spawn();
	}
	
	for (int i = 0; i < E1_MAX; i++)
	{
		e1[i].spawn();
	}
	
	player_ship.set_clips();
	
	// Attempt to play the music on loop
	if (Mix_PlayMusic(bgm1, -1) == -1)
	{
		return 1;
	}
	
	// While the user hasn't quit, control fps and input events
	while(quit == false)
	{
		fps.start(); // start timer
		
		while (help)
		{
			if (SDL_PollEvent(&event))
			{
				//if ((event.key.keysym.sym == SDLK_z) || (event.key.keysym.sym == SDLK_RETURN) || (event.key.keysym.sym == SDLK_x))
				if (event.type == SDL_KEYDOWN)
				{
					help = false;
				}
			}
		}
		
		// While there's events...
		if (SDL_PollEvent(&event))
		{
			player_ship.handle_input(); // check input for the ship
			
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_z) // keydown for bullet 1
				{
					b1_firing = true;
				}
				if (event.key.keysym.sym == SDLK_x) // keydown for bullet 2
				{
					b2_firing = true;
				}
				// DEBUG MODE CODE - Control gamestate with keycodes
				if (DEBUG)
				{
					if (event.key.keysym.sym == SDLK_KP_PLUS) // increment framerate on keypad + press
					{
						if (MAX_FPS < 300)
						{
							MAX_FPS += 10;
						}
					}
					if (event.key.keysym.sym == SDLK_KP_MINUS) // decrement framerate on keypad - press
					{
						if (MAX_FPS > 10)
						{
							MAX_FPS -= 10;
						}
					}
					if (event.key.keysym.sym == SDLK_a) // enable bullet 1 autofire
					{
						b1_firing = true;
					}
					if (event.key.keysym.sym == SDLK_s) // enable bullet 2 autofire
					{
						b2_firing = true;
					}
					if (event.key.keysym.sym == SDLK_PAGEDOWN) // decrease bullet cooldown
					{
						if (player_ship.b1_cooldown > 1)
						{
							player_ship.b1_cooldown -= 2;
							player_ship.b2_cooldown -= 4;
						}
					}
					if (event.key.keysym.sym == SDLK_PAGEUP) // increase bullet cooldown
					{
						if (player_ship.b1_cooldown < 59)
						{
							player_ship.b1_cooldown += 2;
							player_ship.b2_cooldown += 4;
						}
					}
				}
				// Press F1 to get help
				if (event.key.keysym.sym == SDLK_F1)
				{
					help = true;
				}
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					quit = true;
				}
			}

			else if (event.type == SDL_KEYUP)
			{
				if (event.key.keysym.sym == SDLK_z) // keyup for bullet 1
				{
					b1_firing = false;
				}
				if (event.key.keysym.sym == SDLK_x) // keyup for bullet 2
				{
					b2_firing = false;
				}
			}
			
			// If the user has closed the program or hit ESC
			if(event.type == SDL_QUIT)
			{
				quit = true;
			}
		}
		
		player_ship.move(); // calculate ship's position

		// Firing for player bullet type 1
		if ((b1_firing == true) && (b1_cooldown < 1))
		{
			for (int i = 0; i < B1_SHOTS; i++)
			{
				if (b1[i].alive == false)
				{
					b1[i].fire(player_ship.box.x + (SHIP_WIDTH / 2) - (B1_WIDTH / 2), player_ship.box.y + (SHIP_HEIGHT / 2), 0, -10, B1_WIDTH, B1_HEIGHT);
					Mix_PlayChannel(-1, shot1, 0);
					break;
				}
			}
			b1_cooldown = player_ship.b1_cooldown;
		}

		// Firing for player bullet type 2
		if ((b2_firing == true) && (b2_cooldown < 1))
		{
			for (int i = 0; i < B2_SHOTS; i++)
			{
				if (b2[i].alive == false)
				{
					b2[i].fire(player_ship.box.x + (SHIP_WIDTH / 2) - (B2_WIDTH / 2), player_ship.box.y + (SHIP_HEIGHT / 2), 1, -10, B2_WIDTH, B2_HEIGHT);
					b2[i+1].fire(player_ship.box.x + (SHIP_WIDTH / 2) - (B2_WIDTH / 2), player_ship.box.y + (SHIP_HEIGHT / 2), -1, -10, B2_WIDTH, B2_HEIGHT);
					b2[i+2].fire(player_ship.box.x + (SHIP_WIDTH / 2) - (B2_WIDTH / 2), player_ship.box.y + (SHIP_HEIGHT / 2), 2, -10, B2_WIDTH, B2_HEIGHT);
					b2[i+3].fire(player_ship.box.x + (SHIP_WIDTH / 2) - (B2_WIDTH / 2), player_ship.box.y + (SHIP_HEIGHT / 2), -2, -10, B2_WIDTH, B2_HEIGHT);
					Mix_PlayChannel(-1, shot2, 0);
					break;
				}
			}
			b2_cooldown = player_ship.b2_cooldown;
		}

		// Update array for player bullet type 1
		for (int i = 0; i < B1_SHOTS; i++)
		{
			b1[i].update();

			for (int j = 0; j < AS1_MAX; j++)
			{
				if (box_collision(b1[i].box, a1[j].box)) // lazy box collision between bullet and asteroid
				{
					int debris_spawned = 0;
					// Generate debris exploding in an "X" formation
					for (int k = 0; k < D1_MAX; k++)
					{
						if (d1[k].alive == false)
						{
							if (debris_spawned == 0)
							{
								d1[k].spawn(a1[j].box.x + (AS1_WIDTH / 2) - (D1_WIDTH / 2), a1[j].box.y + (AS1_HEIGHT / 2), a1[j].xv+3, a1[j].yv-5);
							}
							else if (debris_spawned == 1)
							{
								d1[k+1].spawn(a1[j].box.x + (AS1_WIDTH / 2) - (D1_WIDTH / 2), a1[j].box.y + (AS1_HEIGHT / 2), a1[j].xv-3, a1[j].yv-5);
							}
							else if (debris_spawned == 2)
							{
								d1[k+2].spawn(a1[j].box.x + (AS1_WIDTH / 2) - (D1_WIDTH / 2), a1[j].box.y + (AS1_HEIGHT / 2), a1[j].xv+3, a1[j].yv-10);
							}
							else if (debris_spawned == 3)
							{
								d1[k+3].spawn(a1[j].box.x + (AS1_WIDTH / 2) - (D1_WIDTH / 2), a1[j].box.y + (AS1_HEIGHT / 2), a1[j].xv-3, a1[j].yv-10);
							}
							else
							{
								break;
							}
							debris_spawned++;
						}
					}
					Mix_PlayChannel(-1, break1, 0);
					// Destroy asteroid and bullet
					a1[j].damage(b1[i].attack);
					b1[i].die();
					player_ship.increase_score(10);
				}
			}

			// Collide with enemies
			for (int j = 0; j < E1_MAX; j++)
			{
				if (box_collision(b1[i].box, e1[j].box))
				{
					e1[j].damage(b1[i].attack);
					b1[i].die();
					player_ship.increase_score(100);
					Mix_PlayChannel(-1, break2, 0);
				}
			}

			// Collide with debris
			for (int j = 0; j < D1_MAX; j++)
			{
				if (box_collision(b1[i].box, d1[j].box))
				{
					b1[i].die();
					d1[j].die();
					player_ship.increase_score(5);
				}
			}
		}
		

		// Update array for player bullet type 2
		for (int i = 0; i < B2_SHOTS; i++)
		{
			b2[i].update();

			for (int j = 0; j < AS1_MAX; j++)
			{
				if (box_collision(b2[i].box, a1[j].box))
				{
					int debris_spawned = 0;
					// Generate debris exploding in an "X" formation
					for (int k = 0; k < D1_MAX; k++)
					{
						if (d1[k].alive == false)
						{
							if (debris_spawned == 0)
							{
								d1[k].spawn(a1[j].box.x + (AS1_WIDTH / 2) - (D1_WIDTH / 2), a1[j].box.y + (AS1_HEIGHT / 2), a1[j].xv+3, a1[j].yv-5);
							}
							else if (debris_spawned == 1)
							{
								d1[k+1].spawn(a1[j].box.x + (AS1_WIDTH / 2) - (D1_WIDTH / 2), a1[j].box.y + (AS1_HEIGHT / 2), a1[j].xv-3, a1[j].yv-5);
							}
							else if (debris_spawned == 2)
							{
								d1[k+2].spawn(a1[j].box.x + (AS1_WIDTH / 2) - (D1_WIDTH / 2), a1[j].box.y + (AS1_HEIGHT / 2), a1[j].xv+3, a1[j].yv-10);
							}
							else if (debris_spawned == 3)
							{
								d1[k+3].spawn(a1[j].box.x + (AS1_WIDTH / 2) - (D1_WIDTH / 2), a1[j].box.y + (AS1_HEIGHT / 2), a1[j].xv-3, a1[j].yv-10);
							}
							else
							{
								break;
							}
							debris_spawned++;
						}
					}
					Mix_PlayChannel(-1, break1, 0);
					// Destroy asteroid and bullet
					a1[j].damage(b2[i].attack);
					b2[i].die();
					player_ship.increase_score(10);
				}
			}

			// Collide with enemies
			for (int j = 0; j < E1_MAX; j++)
			{
				if (box_collision(b2[i].box, e1[j].box))
				{
					e1[j].damage(b2[i].attack);
					b2[i].die();
					player_ship.increase_score(100);
					Mix_PlayChannel(-1, break2, 0);
				}
			}

			// Collide with debris
			for (int j = 0; j < D1_MAX; j++)
			{
				if (box_collision(b2[i].box, d1[j].box))
				{
					b2[i].die();
					d1[j].die();
					player_ship.increase_score(5);
				}
			}
		}

		// Player weapons are cooling
		b1_cooldown--;
		b2_cooldown--;
		
		// Asteroid collisions
		for (int i = 0; i < AS1_MAX; i++)
		{
			if (box_collision(player_ship.box, a1[i].box)) // lazy box collision between ship and asteroid
			{
				int debris_spawned = 0;
				// Generate debris exploding in an "X" formation
				for (int j = 0; j < D1_MAX; j++)
				{
					if (d1[j].alive == false)
					{
						if (debris_spawned == 0)
						{
							d1[j].spawn(a1[i].box.x + (AS1_WIDTH / 2) - (D1_WIDTH / 2), a1[i].box.y + (AS1_HEIGHT / 2), a1[i].xv+3, a1[i].yv-5);
						}
						else if (debris_spawned == 1)
						{
							d1[j+1].spawn(a1[i].box.x + (AS1_WIDTH / 2) - (D1_WIDTH / 2), a1[i].box.y + (AS1_HEIGHT / 2), a1[i].xv-3, a1[i].yv-5);
						}
						else if (debris_spawned == 2)
						{
							d1[j+2].spawn(a1[i].box.x + (AS1_WIDTH / 2) - (D1_WIDTH / 2), a1[i].box.y + (AS1_HEIGHT / 2), a1[i].xv+3, a1[i].yv-10);
						}
						else if (debris_spawned == 3)
						{
							d1[j+3].spawn(a1[i].box.x + (AS1_WIDTH / 2) - (D1_WIDTH / 2), a1[i].box.y + (AS1_HEIGHT / 2), a1[i].xv-3, a1[i].yv-10);
						}
						else
						{
							break;
						}
						debris_spawned++;
					}
				}
				Mix_PlayChannel(-1, break1, 0);
				// Update ship health and the HUD string
				player_ship.damage(a1[i].attack);
				a1[i].die();
			}

			// Asteroids collide with debris
			for (int j = 0; j < D1_MAX; j++)
			{
				if (box_collision(a1[i].box, d1[j].box))
				{
					a1[i].damage(d1[j].attack);
					d1[j].die();
				}
			}
		}

		// Update scrolling backgrounds
		update_bg(distant, screen);
		update_bg(starfield, screen);
		update_bg(planetoids, screen);
		
		// Display player bullets
		for (int i = 0; i < B1_SHOTS; i++)
		{
			b1[i].show();
		}
		for (int i = 0; i < B2_SHOTS; i++)
		{
			b2[i].show();
		}
		
		player_ship.show(); // display the ship's current position
		
		// Update AND display asteroid debris
		for (int i = 0; i < D1_MAX; i++)
		{
			d1[i].update();
			if (box_collision(d1[i].box, player_ship.box))
			{
				player_ship.damage(d1[i].attack);
				d1[i].die();
			}
			d1[i].show();
		}
		
		// Update AND display asteroids
		for (int i = 0; i < AS1_MAX; i++)
		{
			a1[i].update();
			a1[i].show();
		}
		
		// Enemies move, fire, display
		for (int i = 0; i< E1_MAX; i++)
		{
			if (e1[i].b3_cooldown < 1)
			{
				// Enemy bullets fire downwards
				for (int j = 0; j < B3_SHOTS; j++)
				{
					if (e1[i].b3[j].alive == false)
					{
						e1[i].b3[j].fire(e1[i].box.x + (e1[i].box.w / 2) - (B3_WIDTH / 2), e1[i].box.y + (e1[i].box.h / 2), 0, 5, B3_WIDTH, B3_HEIGHT);
						Mix_PlayChannel(-1, shot3, 0);
						break;
					}
				}
				e1[i].b3_cooldown = B3_COOLDOWN;
			}
			// Update enemy bullets and display
			for (int j = 0; j < B3_SHOTS; j++)
			{
				e1[i].b3[j].update();
				if (box_collision(player_ship.box, e1[i].b3[j].box)) // lazy box collision between ship and enemy bullets
				{
					e1[i].b3[j].die();
					player_ship.damage(e1[i].b3[j].attack);
				}
				e1[i].b3[j].show();	
			}
			// Weapon cools down, enemy moves, check for collision with player, then display
			e1[i].b3_cooldown--;
			e1[i].update();
			if (box_collision(player_ship.box, e1[i].box))
			{
				player_ship.damage(e1[i].attack);
				e1[i].damage(player_ship.attack);
			}
			e1[i].show();
		}
		
		// Apply foreground layers
		update_bg(nebula, screen);
		update_hud(player_ship, HUD_display, screen);
		if (help)
		{
			apply_surface(0, 0, help_popup, screen);
		}
		

		// Update the screen, if unable to do so, return 1
		if( SDL_Flip( screen ) == -1 )
		{
			return 1;
		}
		
		// Cap the frame rate to our specified maximum fps
		if(fps.get_time() < 1000 / MAX_FPS)
		{
			SDL_Delay((1000 / MAX_FPS) - fps.get_time());
		}
	}
	
	clean_quit();
	
	return 0;
}



//////////////////////////////////////////////////////////////////////
// Function Definitions                                             //
//////////////////////////////////////////////////////////////////////


// GENERAL FUNCTIONS //

// Image loading function - loads image specificed by filename
// and returns the version optimised for SDL's display
SDL_Surface *load_image(string filename)
{
	SDL_Surface* loaded_image = NULL; // loaded image's temporary storage
	SDL_Surface* optimised_image = NULL;  // the optimised surface, blitting from a surface with a different format causes horrible slowdown
	loaded_image = IMG_Load(filename.c_str()); // actually load the image, will return NULL if there was an error
	
	// If the image loaded successfully...
	if(loaded_image != NULL)
	{
		optimised_image = SDL_DisplayFormatAlpha(loaded_image); // convert from loaded format to one optimised for SDL, make sure to use the Alpha version of SDL_DisplayFormat, or else the alpha channel gets stripped!
		SDL_FreeSurface(loaded_image); // free up the old surface from memory, no need for it now
	}
	
	return optimised_image; // return the optimised surface
}

// Surface blitting function - takes x and y of WHERE to blit, the surface to blit
// from and the surface to, in that order.
void apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip)
{
	// Make a temporary rectangle to hold the offsets
	// SDL_BlitSurface only accepts offsets within an SDL_Rect
	SDL_Rect offset;
	
	// Give the offsets to the rectangle - the rect also has width
	// and height members, but we're not concerned with them here
	offset.x = x;
	offset.y = y;
	offset.w = SCREEN_WIDTH;
	offset.h = SCREEN_HEIGHT;
	
	//Blit the surface - specify the source surface, a rect stating how much
	//of the source surface to use (NULL means use all), the destination surface
	//and a rect stating where the offset for blitting is.
	SDL_BlitSurface(source, clip, destination, &offset);
}

// Initialise SDL and generally set everything up
bool init()
{
	// Setup SDL systems, return false if there was an issue
	if(SDL_Init( SDL_INIT_EVERYTHING ) == -1)
	{
		return false;
	}
	
	if (FULLSCREEN)
	{
		screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE | SDL_FULLSCREEN);
	}
	else
	{
		screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
	}
	
	if( screen == NULL )
	{
		return false;
	}
	
	// Initialise SDL_mixer
	//args: sound frequency, sound format, audio channels and sample size
	//for other formats besides wav (e.g. FLAC, MOD, MP3, OGG) use Mix_Init(flags)
	//to add support. Don't forget to then use Mix_Quit() when done with the decoders!
	if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
	{
		return false;
	}
	
	// Initialise SDL_ttf
	if( TTF_Init() == -1 )
	{
		return false;    
		
	}
	
	
	SDL_WM_SetCaption("Space Maelstrom Transcendence", NULL); // set the window title and icon
	
	return true; // everything initialised fine
}

// Load our required files. Not really needed as this will only ever be called once
// but it does make the mainloop a lot neater...
bool load_files()
{
	title_bg = load_image("img/title_bg.png");
	title_selection = load_image("img/title_selection.png");
	help_popup = load_image("img/help_popup.png");
	highscore_popup = load_image("img/highscore_popup.png");
	ship = load_image("img/shipsheet.png");
	bullet = load_image("img/pew1.png");
	bullet2 = load_image("img/pew2.png");
	bullet3 = load_image("img/pew3.png");
	asteroid = load_image("img/asteroid1.png");
	boom1 = load_image("img/boom1.png");
	debris = load_image("img/asteroid1_debris.png");
	enemy1 = load_image("img/enemy1.png");
	background = load_image("img/distant_bg.png");
	stars = load_image("img/stars.png");
	planets = load_image("img/planets.png");
	nebulae = load_image("img/nebulae.png");
	bgm1 = Mix_LoadMUS("snd/bgm1.ogg");
	shot1 = Mix_LoadWAV("snd/pew1.wav");
	shot2 = Mix_LoadWAV("snd/pew2.wav");
	shot3 = Mix_LoadWAV("snd/pew3.wav");
	break1 = Mix_LoadWAV("snd/break1.wav");
	break2 = Mix_LoadWAV("snd/break2.wav");
	menu1 = Mix_LoadWAV("snd/menu1.wav");
	menu2 = Mix_LoadWAV("snd/menu2.wav");
	font1 = TTF_OpenFont("terminus.ttf", 14);
	
	// If any file doesn't load, return false
	if((ship == NULL) || (background == NULL) || (bgm1 == NULL) || (stars == NULL) || (bullet == NULL) || (planets == NULL) || (nebulae == NULL))
	{
		return false;
	}
	
	return true; // everything loaded fine
}

// House-keeping when shutting the application
void clean_quit()
{
	// Free the surfaces
	SDL_FreeSurface(title_bg);
	SDL_FreeSurface(title_selection);
	SDL_FreeSurface(help_popup);
	SDL_FreeSurface(highscore_popup);
	SDL_FreeSurface(ship);
	SDL_FreeSurface(background);
	SDL_FreeSurface(stars);
	SDL_FreeSurface(planets);
	SDL_FreeSurface(nebulae);
	SDL_FreeSurface(bullet);
	SDL_FreeSurface(bullet2);
	SDL_FreeSurface(bullet3);
	SDL_FreeSurface(debris);
	SDL_FreeSurface(enemy1);
	SDL_FreeSurface(boom1);
	
	Mix_FreeMusic(bgm1); // free the audio stream
	Mix_FreeChunk(shot1);
	Mix_FreeChunk(shot2);
	Mix_FreeChunk(shot3);
	Mix_FreeChunk(break1);
	Mix_FreeChunk(break2);
	Mix_FreeChunk(menu1);
	Mix_FreeChunk(menu2);
	Mix_CloseAudio(); // close the audio mixer
	
	SDL_FreeSurface(HUD_display);
	TTF_CloseFont(font1);
	TTF_Quit();
	
	SDL_Quit(); // close SDL
}

void update_bg(Background& bg, SDL_Surface* display)
{
	// scroll backgrounds down
	bg.y += bg.scroll_speed;
			
	// If the background has gone too far down...
	if (bg.y >= bg.image->h )
	{
		bg.y = 0; // reset offset
	}
	
	// Apply to screen
	apply_surface(bg.x, bg.y, bg.image, display); // show background at current coordinates
	apply_surface(bg.x, bg.y - bg.image->h, bg.image, display); // display a copy slightly offset to the original to create the illusion of scrolling
}

// Generic rectangular collision detection - inaccurate but FAST
bool box_collision(SDL_Rect A, SDL_Rect B)
{
    // The sides of the rectangles
    int left_a, left_b;
    int right_a, right_b;
    int top_a, top_b;
    int bottom_a, bottom_b;

    // Calculate the sides of rect A
    left_a = A.x;
    right_a = A.x + A.w;
    top_a = A.y;
    bottom_a = A.y + A.h;
        
    // Calculate the sides of rect B
    left_b = B.x;
    right_b = B.x + B.w;
    top_b = B.y;
    bottom_b = B.y + B.h;

    // If any of the sides from A are outside of B
    if(bottom_a <= top_b)
    {
        return false;
    }
    
    if(top_a >= bottom_b)
    {
        return false;
    }
    
    if(right_a <= left_b)
    {
        return false;
    }
    
    if(left_a >= right_b)
    {
        return false;
    }
    
    // If none of the sides from A are outside B
    return true;
}

// Updates the HUD with Health, Score and Debug information
void update_hud(Ship player, SDL_Surface* HUD, SDL_Surface* screen)
{
	stringstream ss;
	string HUD_health_txt;
	string HUD_score_txt;
	
	ss << player.return_hp();
	HUD_health_txt = "Health = " + ss.str();
	ss.str(string());
	ss << player.return_score();
	HUD_score_txt = "Score = " + ss.str();
	ss.str(string());
	
	HUD = TTF_RenderText_Solid(font1, HUD_health_txt.c_str(), WHITE);
	apply_surface(5, 5, HUD, screen);
	SDL_FreeSurface(HUD);
	HUD = TTF_RenderText_Solid(font1, HUD_score_txt.c_str(), WHITE);
	apply_surface(5, 20, HUD, screen);
	SDL_FreeSurface(HUD);
	
	if (DEBUG)
	{
		string HUD_debug_txt;
		ss << MAX_FPS;
		HUD_debug_txt = "MAX_FPS : " + ss.str();
		ss.str(string());
		HUD = TTF_RenderText_Solid(font1, HUD_debug_txt.c_str(), WHITE);
		apply_surface(5, 45, HUD, screen);
		SDL_FreeSurface(HUD);
		ss << player.b1_cooldown << ", " << player.b2_cooldown;
		HUD_debug_txt = "COOLDOWN: " + ss.str();
		ss.str(string());
		HUD = TTF_RenderText_Solid(font1, HUD_debug_txt.c_str(), WHITE);
		apply_surface(5, 60, HUD, screen);
		SDL_FreeSurface(HUD);
	}
}

void load_highscores(Highscore table[])
{
	ifstream score_file("highscore.txt");
	string entry;
	int i = 0, j = 0;
	
	while (score_file.good())
	{
		getline(score_file, entry, ','); // read in lines, using commas as delimiters
		if (j == 0) {table[i].pos = atoi(entry.c_str());}
		else if (j == 1) {table[i].name = entry;}
		else if (j == 2) {table[i].score = atoi(entry.c_str());}
		j++;
		if (j > 2) {j = 0; i++;}
	}
}

void show_highscores(Highscore table[], SDL_Surface* HUD, SDL_Surface* screen)
{
	stringstream ss;
	int x = 70, y = 150;
	
	for (int i = 0; i < 10; i++)
	{
		ss << table[i].pos;
		HUD = TTF_RenderText_Solid(font1, ss.str().c_str(), WHITE);
		apply_surface(x, y, HUD, screen);
		x += 110;
		SDL_FreeSurface(HUD);
		ss.str(string());
		
		HUD = TTF_RenderText_Solid(font1, table[i].name.c_str(), WHITE);
		apply_surface(x, y, HUD, screen);
		x += 220;
		SDL_FreeSurface(HUD);
		
		ss << table[i].score;
		HUD = TTF_RenderText_Solid(font1, ss.str().c_str(), WHITE);
		apply_surface(x, y, HUD, screen);
		x = 70; y += 25;
		SDL_FreeSurface(HUD);
		ss.str(string());
	}
}


// Puts the ship in the bottom-centre
Ship::Ship()
{
	box.x = SCREEN_WIDTH / 2 - SHIP_WIDTH;
	box.y = SCREEN_HEIGHT / 2 - SHIP_HEIGHT;
	box.w = SHIP_WIDTH; box.h = SHIP_HEIGHT;
	x_vel = 0; y_vel = 0;
	frame = 0;
	frame_up = true;
	hp = 100; score = 0; attack = 10;
	b1_cooldown = B1_COOLDOWN; b2_cooldown = B2_COOLDOWN;
}

// Setup the clips of the spritesheet for the ship
void Ship::set_clips()
{
    //Clip the sprites
    clips_ship[ 0 ].x = 0;
    clips_ship[ 0 ].y = 0;
    clips_ship[ 0 ].w = SHIP_WIDTH;
    clips_ship[ 0 ].h = SHIP_HEIGHT;

    clips_ship[ 1 ].x = SHIP_WIDTH;
    clips_ship[ 1 ].y = 0;
    clips_ship[ 1 ].w = SHIP_WIDTH;
    clips_ship[ 1 ].h = SHIP_HEIGHT;

    clips_ship[ 2 ].x = SHIP_WIDTH * 2;
    clips_ship[ 2 ].y = 0;
    clips_ship[ 2 ].w = SHIP_WIDTH;
    clips_ship[ 2 ].h = SHIP_HEIGHT;

    clips_ship[ 3 ].x = SHIP_WIDTH * 3;
    clips_ship[ 3 ].y = 0;
    clips_ship[ 3 ].w = SHIP_WIDTH;
    clips_ship[ 3 ].h = SHIP_HEIGHT;

    clips_ship[ 4 ].x = SHIP_WIDTH * 4;
    clips_ship[ 4 ].y = 0;
    clips_ship[ 4 ].w = SHIP_WIDTH;
    clips_ship[ 4 ].h = SHIP_HEIGHT;

    clips_ship[ 5 ].x = SHIP_WIDTH * 5;
    clips_ship[ 5 ].y = 0;
    clips_ship[ 5 ].w = SHIP_WIDTH;
    clips_ship[ 5 ].h = SHIP_HEIGHT;

    clips_ship[ 6 ].x = SHIP_WIDTH * 6;
    clips_ship[ 6 ].y = 0;
    clips_ship[ 6 ].w = SHIP_WIDTH;
    clips_ship[ 6 ].h = SHIP_HEIGHT;
}

// Our keyboard input handler, uses switch case to make things a bit tidier than if/else.
// Note that instead of simply altering the x/y position, we change the velocity instead.
// Problem with x++/y++ is that it'll only move on keydown, so you'll have to press
// multiple times in order to actually keep moving. We move with a velocity equal to half
// the ship's width/height.
void Ship::handle_input()
{
	// If a key was pressed...
	// note: up/down and left/right cancel each other out, but diagonal directions work
	// and provide a faster means of movement
	if(event.type == SDL_KEYDOWN)
	{
		// Increment velocity
		switch( event.key.keysym.sym )
		{
			case SDLK_UP: y_vel -= SHIP_HEIGHT / 4; break;
			case SDLK_DOWN: y_vel += SHIP_HEIGHT / 4; break;
			case SDLK_LEFT: x_vel -= SHIP_WIDTH / 4; break;
			case SDLK_RIGHT: x_vel += SHIP_WIDTH / 4; break;
		}
	}
	// If a key was released...
	// we undo the earlier velocities set by the keydown event, resetting velocity to 0
	else if(event.type == SDL_KEYUP)
	{
		// Negate velocity
		switch(event.key.keysym.sym)
		{
			case SDLK_UP: y_vel = 0; break;
			case SDLK_DOWN: y_vel = 0; break;
			case SDLK_LEFT: x_vel = 0; break;
			case SDLK_RIGHT: x_vel = 0; break;
		}
	}
}

// This function actually handles the movement of the ship, with an included check to prevent it
// going off-screen...although this is flawed as it just pushes it back and if the velocity
// doesn't cleanly go into the screen's dimensions, the ship will get stuck. A better way would be
// to set the ship's offset to the screen's dimension minus the ship's dimension when you go off the
// screen.
void Ship::move()
{
	// If we are moving in both directions (i.e. diagonally) halves the speed to balance
	// the bug/feature of diagonal movement being faster.
	if ((x_vel != 0) && (y_vel != 0))
	{
		box.x += x_vel / 2;
		box.y += y_vel / 2;
	}
	
	else
	{
		box.x += x_vel; // x coordinate movement
		box.y += y_vel; // y coordinate movement
	}
	
	// If the ship went too far to the left or right
	// we wrap it around to the other side
	if (box.x < (0 - SHIP_WIDTH / 4))
	{
		box.x = SCREEN_WIDTH - (SHIP_WIDTH / 4);
	}
	else if (box.x > (SCREEN_WIDTH - SHIP_WIDTH / 4))
	{
		box.x = 0 - (SHIP_WIDTH / 4);
	}
	
	// If the ship went too far up or down
	// we shift it back into place
	if(( box.y < 0 ) || ( box.y + SHIP_HEIGHT > SCREEN_HEIGHT ))
	{
		box.y -= y_vel;
	}
}

void Ship::damage(int attack)
{
	hp -= attack;
}

void Ship::increase_score(int points)
{
	score += points;
}

int Ship::return_hp()
{
	return hp;
}

int Ship::return_score()
{
	return score;
}

// Just blit the ship to the screen. Simple. Animates through 7 frames
void Ship::show()
{
	apply_surface(box.x, box.y, ship, screen, &clips_ship[frame]);
	
	if (frame >= 6)
	{
		frame_up = false;
	}
	else if (frame <= 0)
	{
		frame_up = true;
	}
	
	if (frame_up == true)
	{
		frame++;
	}
	else
	{
		frame--;
	}
}


// TIMER CLASS FUNCTIONS //

// Time initialiser, just sets the variables up
Timer::Timer()
{
	time_elapsed = 0;
	started = false;
}

// Starts the timer, using SDL_GetTicks() to track elapsed time
void Timer::start()
{
	started = true; // start the timer
	time_elapsed = SDL_GetTicks(); // get the current clock time
}

// Stops the timer. Duh?
void Timer::stop()
{
	started = false;
}

// How much time has elasped since the start time?
int Timer::get_time()
{
	// If the timer is running
	if(started == true)
	{
		return (SDL_GetTicks() - time_elapsed); // Return the current time minus the start time
	}
	
	// If the timer isn't running
	else
	{
		return 0;
	}
}

// Check whether the timer is running or not
bool Timer::is_started()
{
	return started;
}

Bullet::Bullet()
{
	alive = false;
	box.x = -400; box.y = 400; xv = 0; yv = 0, box.w = 0, box.h = 0;
	attack = 0;
	sprite = NULL;
}

void Bullet::setup(int damage, SDL_Surface* bullet_surface)
{
	attack = damage;
	sprite = bullet_surface;
}

void Bullet::update()
{
	if (alive)
	{
		box.x += xv;
		box.y += yv;
	}
	
	if ((box.y < 0) || (box.y + box.h > SCREEN_HEIGHT))
	{
		die();
	}
	
	if (box.x < (0 - box.w / 4))
	{
		box.x = SCREEN_WIDTH - (box.w / 4);
	}
	else if (box.x > (SCREEN_WIDTH - box.w / 4))
	{
		box.x = 0 - (box.w / 4);
	}
}

void Bullet::fire(int start_x, int start_y, int x_speed, int y_speed, int width, int height)
{
	box.x = start_x;
	box.y = start_y;
	xv = x_speed;
	yv = y_speed;
	alive = true;
	box.w = width;
	box.h = height;
}

void Bullet::show()
{
	if (alive)
	{
		apply_surface(box.x, box.y, sprite, screen);
	}
}

void Bullet::die()
{
	alive = false;
	box.x = -400; box.y = 400;
	xv = 0; yv = 0;
}

Asteroid::Asteroid()
{
	alive = false;
	box.x = -200; box.y = 200; xv = 0; yv = 0;
	box.w= AS1_WIDTH; box.h = AS1_HEIGHT;
	hp = AS1_HP; attack = AS1_ATTACK;
	sprite = asteroid;
}

void Asteroid::spawn()
{
	alive = true;
	box.x = rand() % SCREEN_WIDTH; 
	box.y = rand() % 100 - SCREEN_HEIGHT; 
	xv = rand() % 6 - 3;
	yv = rand() % 4 + 1;
}

void Asteroid::update()
{
	if (alive)
	{
		box.x += xv;
		box.y += yv;
	}
	
	if ((box.y > SCREEN_HEIGHT))
	{
		die();
	}
	
	if (box.x < (0 - box.w / 2))
	{
		box.x = SCREEN_WIDTH - (box.w / 2);
	}
	else if (box.x > (SCREEN_WIDTH - box.w / 2))
	{
		box.x = 0 - (box.w / 2);
	}

}

void Asteroid::show()
{
	if (alive)
	{
		apply_surface(box.x, box.y, sprite, screen);
	}
}

void Asteroid::damage(int attack)
{
	hp -= attack;
	if (hp < 1)
	{
		die();
	}
}

void Asteroid::die(bool respawn)
{
	alive = false;
	box.x = -200; box.y = 200;
	xv = 0; yv = 0;

	if (respawn)
	{
		spawn(); // spawn new asteroids upon death
	}
}

Debris::Debris()
{
	alive = false;
	box.x = -500; box.y = 500; xv = 0; yv = 0;
	box.w = D1_WIDTH; box.h = D1_HEIGHT;
	attack = D1_ATTACK;
	sprite = debris;
}

void Debris::spawn(int start_x, int start_y, int x_speed, int y_speed)
{
	box.x = start_x;
	box.y = start_y;
	xv = x_speed;
	yv = y_speed;
	alive = true;
}

void Debris::update()
{
	if (alive)
	{
		box.x += xv;
		box.y += yv;
	}
	
	if ((box.y > SCREEN_HEIGHT) || (box.y < 0))
	{
		die();
	}
	
	if ((box.x > SCREEN_WIDTH) || (box.x < 0))
	{
		die();
	}
	
	// Alternative horizontal screen wrapping for debris
	/*if (box.x < (0 - box.w / 2))
	{
		box.x = SCREEN_WIDTH - (box.w / 2);
	}
	else if (box.x > (SCREEN_WIDTH - box.w / 2))
	{
		box.x = 0 - (box.w / 2);
	}*/

}

void Debris::show()
{
	if (alive)
	{
		apply_surface(box.x, box.y, sprite, screen);
	}
}

void Debris::die()
{
	alive = false;
	box.x = -500; box.y = +500;
	xv = 0; yv = 0;
}

Enemy::Enemy()
{
	alive = false;
	box.x = -1000; box.y = 1000; xv = 0; yv = 0;
	box.w= E1_WIDTH; box.h = E1_HEIGHT;
	b3_cooldown = B3_COOLDOWN + rand() % 300;
	hp = E1_HP; attack = E1_ATTACK;
	sprite = enemy1; explosion = boom1;
	explosion_x = box.x; explosion_y = box.y;
	explosion_timer = 0;
}

void Enemy::spawn()
{
	alive = true;
	box.x = rand() % SCREEN_WIDTH; 
	box.y = rand() % 200 - SCREEN_HEIGHT; 
	yv = rand() % 4 + 2;
}

void Enemy::update()
{
	if (alive)
	{
		box.x += xv;
		box.y += yv;
	}
	
	if ((box.y > SCREEN_HEIGHT))
	{
		die();
	}

	if (explosion_timer > 0)
	{
		explosion_timer--;
	}
}

void Enemy::show()
{
	if (alive)
	{
		apply_surface(box.x, box.y, sprite, screen);
	}

	if (explosion_timer > 0)
	{
		apply_surface(explosion_x, explosion_y, explosion, screen);
	}
}

void Enemy::damage(int attack)
{
	hp -= attack;
	if (hp < 1)
	{
		die();
	}
}

void Enemy::die()
{
	alive = false;
	explosion_x = box.x; explosion_y = box.y;
	box.x = -1000; box.y = 1000;
	xv = 0; yv = 0;
	spawn(); // spawn new enemies
	explosion_timer = E1_EXPLODE_TIME;
}