#include "picframe.h"

void OnExit() {
	TTF_Quit();
	SDL_VideoQuit();
	SDL_Quit();
}

int picframe_init() {
	_font = NULL;
	/* SDL Setup */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
	exit(1);
	}
	atexit(OnExit);
	/* Initialize our screen */
	_screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32,
                               SDL_HWSURFACE );
	if ( _screen == NULL )
	{
		fprintf(stderr, "Could not setup screen to resolution %dx%d : %s\n",
		WIDTH, HEIGHT, SDL_GetError());
		exit(1);
	}
	/* Hide the mouse */
	SDL_ShowCursor(0); 
	/* Solid white background */
	picframe_clear_screen();
	/* Initialize the fonts */
	if ( TTF_Init() == -1 ) {
		fprintf(stderr, "Could not initalize SDL_ttf!\n");
		exit(1);
	}
	/* Load a default font */
//	picframe_load_font("/usr/share/fonts/Ubuntu-L.ttf", 20);

	/* Aight, lets make our first default window */
	_num_windows = 0;
//	struct LList_t *node = picframe_add_window();

	gpio_init();

	return 0;
}



void picframe_clear_screen() {
	SDL_FillRect(_screen, &_screen->clip_rect, SDL_MapRGB(_screen->format, 255, 255,255));
}

/* 
 * Very simple (hacky) [vertial or horizontal] line drawing
 * Better is possible by direct pixel manipulation or SDL_gfx
 */
void picframe_draw_line(SDL_Rect pos) {
	SDL_FillRect(_screen, &pos, SDL_MapRGB(_screen->format, 0,0,0));
}

struct LList_t *picframe_add_window() {
	struct LList_t *node = NULL;

	_num_windows++;
	node = llist_add_node(&_windows, _num_windows, NULL);

	return node;
}

struct LList_t *picframe_add_element_to_window(struct LList_t *window, Element_t *data) {
	struct LList_t *node = NULL;
	if (window == NULL) return NULL;
	
	node = llist_add_node((struct LList_t **)&window->data, 1, (void *)data);
	
	return node;
}


int picframe_get_event(SDL_Event *event) {
	int rc;
	rc = SDL_PollEvent(event);
	if (!rc) {
		rc = gpio_handle_events(event);
	}

	return rc;
}

void picframe_update(struct LList_t *window) {
	struct LList_t *curr = window->data;
	Element_t *element = NULL;
	SDL_Surface *ts;
	while (curr) {

		if (curr->data) {
			element = (Element_t *)curr->data;
			if (!element) {
				curr = curr->next;
				continue;
			}

			//printf("Disp surface %p and rect.x: %d rect.y: %d\n", element->surface, element->rect.x, element->rect.y);
			if (element->selected) ts = element->surface_selected;
			else ts = element->surface;

			if (ts) {
//				picframe_disp_surface(ts, &element->rect);
				SDL_BlitSurface(ts, NULL, _screen, &element->rect);
			
				if (element->dynamic) {
					SDL_FreeSurface(element->surface);
					element->surface = NULL;
				}
			}
		}
		curr = curr->next;
	}
	//printf("\n");
	SDL_Flip(_screen);
}

int picframe_get_lightsensor() {
	FILE *fp = NULL;
	int val = 0;

	fp = fopen("/sys/devices/platform/s3c24xx-adc/s3c-hwmon/adc0_raw", "r");
	if (!fp) return -1;

	fscanf(fp, "%d", &val);
	fclose(fp);
	
	return val;
}

int picframe_get_backlight() {
	FILE *fp = NULL;
	int val = 0;
	fp = fopen("/sys/class/backlight/pwm-backlight.0/brightness", "r");
	if (!fp) return -1;
	fscanf(fp, "%d", &val);
	fclose(fp);
	
	return val;
}

int picframe_set_backlight(int i) {
	FILE *fp = NULL;
	fp = fopen("/sys/class/backlight/pwm-backlight.0/brightness", "w");
	if (!fp) return -1;
	
	fprintf(fp, "%d", i);
	fclose(fp);

	return 1;
}

int picframe_load_font(char *path, int size) {
	if (_font) TTF_CloseFont(_font);

	if (!path) {
		fprintf(stderr, "No path given!\n");
		return -1;
	}

	_font = TTF_OpenFont(path, size);
	if (_font == NULL){
		fprintf(stderr, "Unable to load font: %s %s \n", path, TTF_GetError());
		// Handle the error here.
		return -1;
	}
    	return 0;
}

void picframe_cleanup() {
	gpio_cleanup();
}
