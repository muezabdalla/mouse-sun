// g++ key_sun.cpp -o key_sun.cpp.o -lSDL2 -lSDL2_image && ./key_sun.cpp.o
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define MOUSE_FILE		"/dev/input/event4"
#define BUTTON_WIDTH	100
#define BUTTON_HIEGHT	60
#define SHOW_BORDERS	true
#define SHOW_MOUSE		true

using namespace std;

void imageToTexture(string image_path, SDL_Texture* &tex_temp, SDL_Renderer* renderer_temp)
{
	// this is to convert a string to a const char * to be able to use it in IMG_Load
	const char * image_path_converted = image_path.c_str();
	// making the surfaces from the images
	SDL_Surface* sur_temp =	IMG_Load(image_path_converted);
	// making the texture from the surfaces
	tex_temp = SDL_CreateTextureFromSurface(renderer_temp, sur_temp);
	// we do not need the surfaces from now so we freed them
	SDL_FreeSurface(sur_temp);
}

int main() {
	// the x,y positons and width and hieght on the window
	SDL_Rect rect_mouse;
	SDL_Rect rect_mouse_rightP;
	SDL_Rect rect_mouse_leftP;
	SDL_Rect rect_mouse_wheelP;
	SDL_Rect rect_mouse_wheelup;
	SDL_Rect rect_mouse_wheeldown;

	rect_mouse =	{0, 0, BUTTON_WIDTH, BUTTON_HIEGHT}; 

	SDL_Window* window = NULL;
	if (SHOW_BORDERS)
		window = SDL_CreateWindow( "keysun", 0, 0, BUTTON_WIDTH, BUTTON_HIEGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_UTILITY | SDL_WINDOW_ALWAYS_ON_TOP);
	else
		window = SDL_CreateWindow( "keysun", 0, 0, BUTTON_WIDTH, BUTTON_HIEGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_UTILITY | SDL_WINDOW_ALWAYS_ON_TOP);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_Texture* tex_mouse =			NULL; imageToTexture("resources/mouse.png", tex_mouse, renderer);
	SDL_Texture* tex_mouse_rightP =		NULL; imageToTexture("resources/mouse_rightP.png", tex_mouse_rightP, renderer);
	SDL_Texture* tex_mouse_leftP =		NULL; imageToTexture("resources/mouse_leftP.png", tex_mouse_leftP, renderer);
	SDL_Texture* tex_mouse_wheelP =		NULL; imageToTexture("resources/mouse_wheelP.png", tex_mouse_wheelP, renderer);
	SDL_Texture* tex_mouse_wheelup =	NULL; imageToTexture("resources/mouse_wheelup.png", tex_mouse_wheelup, renderer);
	SDL_Texture* tex_mouse_wheeldown =	NULL; imageToTexture("resources/mouse_wheeldown.png", tex_mouse_wheeldown, renderer);

	SDL_RenderCopy(renderer, tex_mouse,  NULL, &rect_mouse);

	int mouse_input = open(MOUSE_FILE, O_RDONLY);
	if (mouse_input == -1) {
		cout << "Cannot open " << MOUSE_FILE << endl;
		return 1;
	}
	cout << "files opened";

	SDL_RenderPresent(renderer);

	SDL_Event sdl_input; // the input from SDL2 (the input when the application is in focus)
	struct input_event global_mouse; // the global input from the mouse input file
	bool close = false; // if set to true the loop exit (to close the app)
	while (!close)
	{
		ssize_t mouse_bytesRead = read(mouse_input, &global_mouse, sizeof(global_mouse));
		if (mouse_bytesRead == (ssize_t)-1) {
			cout << "Failed to read from " << MOUSE_FILE << ". but it was opened successfuly" << endl;
			return 1;
		}

		// checkin if the type is pressed or released
		if (global_mouse.type == EV_KEY)
		{
			//checking if any of the three buttons release(right,left,wheel)
			if (global_mouse.value == 0 && global_mouse.code > 27 && global_mouse.code < 275)
			{
				SDL_RenderCopy(renderer, tex_mouse,  NULL, &rect_mouse);
			}
			if (global_mouse.value == 1)
			{
				switch (global_mouse.code)
				{
					case 272: SDL_RenderCopy(renderer, tex_mouse_leftP,  NULL, &rect_mouse); break;
					case 273: SDL_RenderCopy(renderer, tex_mouse_rightP,  NULL, &rect_mouse); break;
					case 274: SDL_RenderCopy(renderer, tex_mouse_wheelP,  NULL, &rect_mouse); break;
				}
			}
			SDL_RenderPresent(renderer);
		}
		// checking if the type is = 2 (relative)
		if (global_mouse.type == EV_REL && global_mouse.code == 11)
		{
			// checking if scrolled up
			if (global_mouse.value == 120)
				SDL_RenderCopy(renderer, tex_mouse_wheelup,  NULL, &rect_mouse);
			else 
				SDL_RenderCopy(renderer, tex_mouse_wheeldown,  NULL, &rect_mouse);

			SDL_RenderPresent(renderer);
			// to make the arrow show for a small period of time we should sleep then show the normal mouse image
			// other wise the arrows will stay until a button is released
			SDL_Delay(200);
			SDL_RenderCopy(renderer, tex_mouse,  NULL, &rect_mouse);
			SDL_RenderPresent(renderer);
		}
		while ( SDL_PollEvent( &sdl_input ) != 0 ) 
		{
			// checking if you want to close the app
			if (sdl_input.type == SDL_QUIT) close =true;
		}
	}
	return 0;
}
