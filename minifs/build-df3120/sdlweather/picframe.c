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

	picframe_gpio_init();

	return 0;
}

int picframe_gpio_export(int ex, int pin) {
	char buff[50];
	FILE *fp;
	
	if (ex) fp = fopen("/sys/class/gpio/export", "ab");
	else fp = fopen("/sys/class/gpio/unexport", "ab");

	if (!fp) return -1;
	sprintf(buff, "%d", pin);
	rewind(fp);
	fwrite(&buff, sizeof(char), strlen(buff), fp);
	fclose(fp);
	return 0;
}

int picframe_gpio_setcfg(int gpio, int dir, int edge) {
	char buff[50];
	int fp;

        sprintf(buff, "/sys/class/gpio/gpio%d/edge", gpio);
        fp = open(buff, O_WRONLY);

        if (!fp) {
                debug_printf("failed to open %s\n", buff);
                return -1;
        }
        if (edge == 0) write(fp, "none", 4);
        else if (edge == 1) write(fp, "falling", 4);
        else if (edge == 2) write(fp, "rising", 4);
	else if (edge == 3) write(fp, "both", 4);
	close(fp);

	sprintf(buff, "/sys/class/gpio/gpio%d/direction", gpio);
	fp = open(buff, O_WRONLY);
	if (!fp) {
		debug_printf("failed to open %s\n", buff);
		return -1;
	}
	if (dir) write(fp, "out", 3);
	else write(fp, "in", 2);
	close(fp);

	return 0;
}

/* These are pretty DF3120 specific! */
void picframe_gpio_init() {
	picframe_gpio_export(1, 162);
	picframe_gpio_export(1, 163);
	picframe_gpio_export(1, 164);
	/* Settings: direction=in, edge=falling */
	picframe_gpio_setcfg(162, 0, 1);
	picframe_gpio_setcfg(163, 0, 1);
	picframe_gpio_setcfg(164, 0, 1);
	/* BUG: somehow the edge detect doesnt work first */
	system("echo falling > /sys/class/gpio/gpio162/edge");
	system("echo both > /sys/class/gpio/gpio162/edge");
        system("echo falling > /sys/class/gpio/gpio163/edge");
        system("echo both > /sys/class/gpio/gpio163/edge");
        system("echo falling > /sys/class/gpio/gpio164/edge");
        system("echo both > /sys/class/gpio/gpio164/edge");

	_max_fds = 0;
        // TODO: Failure handling here
        _gpio_fds[0] = open("/sys/class/gpio/gpio162/value", O_RDONLY | O_NONBLOCK);
        _gpio_fds[1] = open("/sys/class/gpio/gpio163/value", O_RDONLY | O_NONBLOCK);
        _gpio_fds[2] = open("/sys/class/gpio/gpio164/value", O_RDONLY | O_NONBLOCK);

	// TODO: currently blind handling of buttons, default is active
	_buttons[0].val = 1;
	_buttons[0].pval = 1;
	_buttons[1].val = 1;
	_buttons[1].pval = 1;
	_buttons[2].val = 1;
	_buttons[2].pval = 1;
}

void picframe_gpio_cleanup() {
	close(_gpio_fds[0]);
	close(_gpio_fds[1]);
	close(_gpio_fds[2]);

	picframe_gpio_export(0, 162);
	picframe_gpio_export(0, 163);
	picframe_gpio_export(0, 164);
}

int picframe_gpio_handle_events(SDL_Event *event) {
	char buff[10];
	int rs, i;

	memset((void*)_fd_set, 0, sizeof(_fd_set));
	_fd_set[0].fd = _gpio_fds[0];
	_fd_set[0].events = POLLPRI;
	_fd_set[1].fd = _gpio_fds[1];
	_fd_set[1].events = POLLPRI;
	_fd_set[2].fd = _gpio_fds[2];
	_fd_set[2].events = POLLPRI;

	rs = poll(_fd_set, NUM_GPIO_KEYS, 50);

	if (rs < 0) {
		debug_printf("Error in poll(): %d,%d\n", rs, errno);
		return -1;
	} else if (rs == 0) {
	} else {
		rs = 1;
		for(i=0;i<NUM_GPIO_KEYS;i++) {
			if (_fd_set[i].revents & POLLPRI) {
				(void)read(_gpio_fds[i], buff, 1);
				debug_printf("Button %d value change: %d\n", i, buff[0]);
			
				_buttons[i].pval = _buttons[i].val;
				if (_buttons[i].val) {
					event->type = SDL_KEYUP;
					_buttons[i].val = 0;
				} else {
					event->type = SDL_KEYDOWN;
					_buttons[i].val = 1;
				}

				if (i==0)
					event->key.keysym.sym = SDLK_RIGHT;
				else if (i==1)
					event->key.keysym.sym = SDLK_LEFT;
				else if (i==2)
					event->key.keysym.sym = SDLK_RETURN;
				return rs;	
			}
		}
	}
	return rs;
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
		rc = picframe_gpio_handle_events(event);
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
				//printf("Disp surface %p and rect.x: %d rect.y: %d\n", ts, element->rect.x, element->rect.y);
				picframe_disp_surface(ts, &element->rect);		
			
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

int picframe_gen_text(SDL_Surface **target, SDL_Color fgcolor, SDL_Color bgcolor, char *text) {
	*target = TTF_RenderText_Shaded(_font, text, fgcolor, bgcolor);
	return 0;
}

int picframe_load_image(SDL_Surface **target, char *path) {
	SDL_Surface *tmp = NULL;
	debug_printf("Loading image: %s\n", path);
	tmp = IMG_Load(path);
	if (!tmp) {
		fprintf(stderr, "Failed to load image %s\n", path);
		return -1;
	}
	*target = tmp; //SDL_DisplayFormat(tmp);
	//SDL_FreeSurface(tmp);
	return 0;
}

int picframe_disp_surface(SDL_Surface *src, SDL_Rect *rect) {
	SDL_BlitSurface(src, NULL, _screen, rect);
	return 0;
}

int picframe_add_button(Element_t *b, SDL_Rect *rect, char *path, char *selected) {

	b->surface = IMG_Load(path);
	b->surface_selected = IMG_Load(selected);
	memcpy(&(b->rect), rect, sizeof(SDL_Rect));
	b->selected = 0;
	b->dynamic = 0;
	return 0;	
}

int picframe_add_button_text(Element_t *b, SDL_Rect *rect, int textsize, char *text) {
	SDL_Color fg = {0,0,0,0};
	SDL_Color bg = {255,255,255,0};

	SDL_Surface *tmp = NULL;
	SDL_Rect nr;
	tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, rect->w, rect->h, _screen->format->BitsPerPixel,
				   _screen->format->Rmask, _screen->format->Gmask, _screen->format->Bmask, _screen->format->Amask);
	
	SDL_FillRect(tmp, &tmp->clip_rect, SDL_MapRGB(tmp->format, 0, 0,0));
	nr.x = 2;
	nr.y = 2;
	nr.w = rect->w - 4;
	nr.h = rect->h - 4;
	SDL_FillRect(tmp, &nr, SDL_MapRGB(tmp->format, 255, 255, 255));


	SDL_Surface *tmp_sel = NULL;
	tmp_sel = SDL_CreateRGBSurface(SDL_SWSURFACE, rect->w, rect->h, _screen->format->BitsPerPixel,
                                   _screen->format->Rmask, _screen->format->Gmask, _screen->format->Bmask, _screen->format->Amask);
	SDL_FillRect(tmp_sel, &tmp_sel->clip_rect, SDL_MapRGB(tmp_sel->format, 0, 0,0));
        nr.x = 4;
        nr.y = 4;
        nr.w = rect->w - 8;
        nr.h = rect->h - 8;
        SDL_FillRect(tmp_sel, &nr, SDL_MapRGB(tmp_sel->format, 255, 255, 255));

	SDL_Surface *tmp_text = NULL;

        picframe_load_font("/usr/share/fonts/Ubuntu-L.ttf", textsize);
        picframe_gen_text(&tmp_text, fg, bg, text);
	
	nr.x = (rect->w/2) - (tmp_text->clip_rect.w/2);
	nr.y = (rect->h/2) - (tmp_text->clip_rect.h/2);
	nr.w = rect->w;
	nr.h = rect->h;
	
        SDL_BlitSurface(tmp_text, NULL, tmp, &nr);
	SDL_BlitSurface(tmp_text, NULL, tmp_sel, &nr);
	
	SDL_FreeSurface(tmp_text);	

	b->surface = tmp;
	b->surface_selected = tmp_sel;
	memcpy(&(b->rect), rect, sizeof(SDL_Rect));
	b->selected = 0;
	b->dynamic = 0;
	return 0;
}

int picframe_add_progress_bar(Element_t *b, SDL_Rect *rect, int val) {

        SDL_Surface *tmp = NULL;
        SDL_Rect nr;
        tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, rect->w, rect->h, _screen->format->BitsPerPixel,
                                   _screen->format->Rmask, _screen->format->Gmask, _screen->format->Bmask, _screen->format->Amask);

        SDL_FillRect(tmp, &tmp->clip_rect, SDL_MapRGB(tmp->format, 0, 0,0));
        nr.x = 2;
        nr.y = 2;
        nr.w = rect->w - 4;
        nr.h = rect->h - 4;
        SDL_FillRect(tmp, &nr, SDL_MapRGB(tmp->format, 255, 255, 255));

	b->surface = tmp;
	b->surface_selected = NULL;
	memcpy(&(b->rect), rect, sizeof(SDL_Rect));
	b->selected =0;
	b->dynamic = 0;

	picframe_update_progress_bar(b, val);

	return 0;
}

int picframe_update_progress_bar(Element_t *b, int val) {
	debug_printf("Updating progress bar: %d%\n", val);
	/* Clean the progress bar */
	SDL_Rect nr;
	nr.x = 2;
        nr.y = 2;
        nr.w = b->rect.w - 4;
        nr.h = b->rect.h - 4;

        SDL_FillRect(b->surface, &nr, SDL_MapRGB(b->surface->format, 255, 255, 255));
	/* Fill to the percentage */
	
	nr.w = (int)((val/100.0)*(b->rect.w-4));
	SDL_FillRect(b->surface, &nr, SDL_MapRGB(b->surface->format, 0,0,0));
	return 0;
}

void picframe_cleanup() {
	picframe_gpio_cleanup();
}
