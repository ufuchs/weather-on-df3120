#include "picframe.h"
#include <time.h>

#define START_WINDOW 1
#define NUM_WINDOWS  4

/* General elements for all windows */
Element_t leftArrow;
Element_t rightArrow;
Element_t window_label;

/* Clock window elements */
Element_t time_disp, info_disp;

//char *window_labels[NUM_WINDOWS] = { "Date and Time", "Media Center", "Wther", "Lights", "Security Cam" };

/* Weather window elements */
Element_t today_pic;
Element_t tomorrow_pic;
Element_t twodays_pic;
Element_t threedays_pic;

/* Home Automation window */
#define NUM_APPLIANCES 4
Element_t appls[NUM_APPLIANCES];
char *appl_names[NUM_APPLIANCES] = { "Living room", "Bed room", "Kitchen",
		"Bathroom" };

/*
 * Global vars
 */
struct LList_t *curr_window = NULL;
int curr_window_idx = START_WINDOW, prev_window_idx = START_WINDOW;
SDL_Event event;
SDL_Color fg = { 0, 0, 0, 0 };
SDL_Color bg = { 255, 255, 255, 0 };
int progress = 0;

/*
 * Generic Input handling
 */
int handle_input() {
	int i = 0;
	struct LList_t *tmp_window = NULL;

//	while (picframe_get_event(&event)) {
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			printf("Quitting\n");
			return 1;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_LEFT) {
				leftArrow.selected = 1;
				curr_window_idx--;
				if (curr_window_idx < 1)
					curr_window_idx = 1;
				tmp_window = picframe_get_window(curr_window_idx);
				if (tmp_window)
					curr_window = tmp_window;
				else
					curr_window_idx++;

				printf("Current window: %d\n", curr_window_idx);
			}

			if (event.key.keysym.sym == SDLK_RIGHT) {
				rightArrow.selected = 1;
				curr_window_idx++;
				if (curr_window_idx > NUM_WINDOWS)
					curr_window_idx = NUM_WINDOWS;
				tmp_window = picframe_get_window(curr_window_idx);
				if (tmp_window)
					curr_window = tmp_window;
				else
					curr_window_idx--;

				printf("Current window: %d\n", curr_window_idx);
			}

			if (event.key.keysym.sym == SDLK_RETURN) {
				i = picframe_get_lightsensor();
				printf("Light sensor: %d\n", i);
			}

			if (event.key.keysym.sym == '1') {
				appls[0].selected = 1;
			}
			if (event.key.keysym.sym == '2') {
				appls[1].selected = 1;
			}
			if (event.key.keysym.sym == '3') {
				appls[2].selected = 1;
			}
			if (event.key.keysym.sym == '4') {
				appls[3].selected = 1;
			}

			break;

		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_LEFT) {
				leftArrow.selected = 0;
			}

			if (event.key.keysym.sym == SDLK_RIGHT) {
				rightArrow.selected = 0;
			}

			if (event.key.keysym.sym == '1') {
				appls[0].selected = 0;
			}
			if (event.key.keysym.sym == '2') {
				appls[1].selected = 0;
			}
			if (event.key.keysym.sym == '3') {
				appls[2].selected = 0;
			}
			if (event.key.keysym.sym == '4') {
				appls[3].selected = 0;
			}

			break;
		default:
			break;
		}
		if (prev_window_idx != curr_window_idx) {
			prev_window_idx = curr_window_idx;
			return 2;
		}
		prev_window_idx = curr_window_idx;
	}
	return 0;
}

/*
 *
 */
void arrow_setup() {

	char *leftPath = "/usr/share/data/glyphicons_210_left_arrow.png";
	char *leftSelected = "/usr/share/data/glyphicons_210_left_arrow_selected.png";
	char *rightPath = "/usr/share/data/glyphicons_211_right_arrow.png";
	char *rightSelected = "/usr/share/data/glyphicons_211_right_arrow_selected.png";

	/* Left arrow */
	leftArrow.surface = IMG_Load(leftPath);
	leftArrow.surface_selected = IMG_Load(leftSelected);
	leftArrow.rect.x = 10;
	leftArrow.rect.y = 5;
	leftArrow.selected = 0;
	leftArrow.dynamic = 0;

	/* Right arrow */
	rightArrow.surface = IMG_Load(rightPath);
	rightArrow.surface_selected = IMG_Load(rightSelected);
	rightArrow.rect.x = 320 - 30;
	rightArrow.rect.y = 5;
	rightArrow.selected = 0;
	rightArrow.dynamic = 0;

}

/*
 *
 */
void app_free() {

	SDL_FreeSurface(leftArrow.surface);
	SDL_FreeSurface(leftArrow.surface_selected);

	SDL_FreeSurface(rightArrow.surface);
	SDL_FreeSurface(rightArrow.surface_selected);
}

/*
 * Setup routine for the clock window
 */
void clock_setup() {
	struct LList_t *first_window = NULL;
	first_window = picframe_get_window(1);
	SDL_Rect tmp;

	picframe_add_element_to_window(first_window, &leftArrow);

	picframe_add_element_to_window(first_window, &rightArrow);

	/////// Arrows are HERE //////

	window_label.surface = NULL;
	window_label.rect.x = 110;
	window_label.rect.y = 5;
	window_label.selected = 0;
	window_label.dynamic = 0;
	picframe_add_element_to_window(first_window, &window_label);

	picframe_load_font("/usr/share/fonts/Ubuntu-L.ttf", 80);

	/* Dynamic watch label */
	time_disp.surface = NULL; //sometext;
	time_disp.rect.x = 5;
	time_disp.rect.y = 50;
	time_disp.selected = 0;
	time_disp.dynamic = 1; /* Free up the surface after use, otherwise memory leak! */
	picframe_add_element_to_window(first_window, &time_disp);

	/* Dynamic info label */
	info_disp.surface = NULL;
	info_disp.rect.x = 10;
	info_disp.rect.y = 200;
	info_disp.selected = 0;
	info_disp.dynamic = 1;
	picframe_add_element_to_window(first_window, &info_disp);
}

/*
 *  Execution of the clock window
 */
int clock_loop() {
	time_t now, prev = 0;
	struct tm* tm;
	char buff[50];
	int ret, lightlevel, brightness;

	SDL_FreeSurface(window_label.surface);
	picframe_load_font("/usr/share/fonts/Ubuntu-L.ttf", 30);

//	picframe_gen_text(&window_label.surface, fg, bg, window_labels[0]);
	window_label.rect.x = (WIDTH / 2) - (window_label.surface->clip_rect.w / 2);

	while (1) {
		ret = handle_input();
		if (ret)
			return ret;

		now = time(0);
		if (now > prev) {
			tm = localtime(&now);
			sprintf(buff, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min,
					tm->tm_sec);
			picframe_load_font("/usr/share/fonts/Ubuntu-L.ttf", 80);

			picframe_gen_text(&time_disp.surface, fg, bg, buff);

			// FREE_FONT
			/*
			debug_printf("Setting time_disp surface to: %p\n",
					time_disp.surface);
			*/
			prev = now;

			lightlevel = picframe_get_lightsensor();
			brightness = picframe_get_backlight();

			sprintf(buff, "Light level: %d\tBrightness: %d", lightlevel,
					brightness);
			picframe_load_font("/usr/share/fonts/Ubuntu-L.ttf", 20);
			picframe_gen_text(&info_disp.surface, fg, bg, buff);
			/*
			debug_printf("Setting info_disp surface to: %p\n",
					info_disp.surface);
			*/
		}

		picframe_update(curr_window);
		SDL_Delay(1);
	}
	return 0;
}

//
// weather_today_setup
//
void weather_today_setup() {

	struct LList_t *weather_window = picframe_add_window();

	today_pic.surface = NULL;
    today_pic.rect.x = 0;
    today_pic.rect.y = 0;
	today_pic.selected = 0;
	today_pic.dynamic = 0;

	picframe_add_element_to_window(weather_window, &today_pic);

	picframe_add_element_to_window(weather_window, &leftArrow);
	picframe_add_element_to_window(weather_window, &rightArrow);

}

//
// weather_tomorrow_setup
//
void weather_tomorrow_setup() {

	struct LList_t *weather_window = picframe_add_window();

	tomorrow_pic.surface = NULL;
	tomorrow_pic.rect.x = 0;
	tomorrow_pic.rect.y = 0;
	tomorrow_pic.selected = 0;
	tomorrow_pic.dynamic = 0;


	picframe_add_element_to_window(weather_window, &tomorrow_pic);

	picframe_add_element_to_window(weather_window, &leftArrow);
	picframe_add_element_to_window(weather_window, &rightArrow);


}

//
// weather_dayAfterTomorrow_setup
//
void weather_dayAfterTomorrow_setup() {

	struct LList_t *weather_window = picframe_add_window();

	twodays_pic.surface = NULL;
	twodays_pic.rect.x = 0;
	twodays_pic.rect.y = 0;
	twodays_pic.selected = 0;
	twodays_pic.dynamic = 0;

	picframe_add_element_to_window(weather_window, &twodays_pic);

	picframe_add_element_to_window(weather_window, &leftArrow);
	picframe_add_element_to_window(weather_window, &rightArrow);

}

//
// weather_dayAfterTomorrowPlus1_setup
//
void weather_dayAfterTomorrowPlus1_setup() {

	struct LList_t *weather_window = picframe_add_window();

	threedays_pic.surface = NULL;
	threedays_pic.rect.x = 0;
	threedays_pic.rect.y = 0;
	threedays_pic.selected = 0;
	threedays_pic.dynamic = 0;

	picframe_add_element_to_window(weather_window, &threedays_pic);

	picframe_add_element_to_window(weather_window, &leftArrow);
	picframe_add_element_to_window(weather_window, &rightArrow);

}

///////////////////////////////////////////////////////////////////////////////

//
// weather_today_loop
//
int weather_today_loop() {
	int ret;

	today_pic.surface = IMG_Load("/exports/df3120-rootfs/tmp/weather-0.png");

	picframe_update(curr_window);

	SDL_FreeSurface(today_pic.surface);

	while (1) {
		ret = handle_input();
		if (ret) {
			return ret;
		}
		SDL_Delay(10);
	}

}

//
// weather_tomorrow_loop
//
int weather_tomorrow_loop() {
	int ret;

	tomorrow_pic.surface = IMG_Load("/exports/df3120-rootfs/tmp/weather-1.png");

	picframe_update(curr_window);

	SDL_FreeSurface(tomorrow_pic.surface);

	while (1) {
		ret = handle_input();
		if (ret)
			return ret;
		SDL_Delay(10);
	}

}

//
// weather_dayAfterTomorrow_loop
//
int weather_dayAfterTomorrow_loop() {
	int ret;

	twodays_pic.surface = IMG_Load("/exports/df3120-rootfs/tmp/weather-2.png");

	picframe_update(curr_window);

	SDL_FreeSurface(twodays_pic.surface);

	while (1) {
		ret = handle_input();
		if (ret)
			return ret;
		SDL_Delay(10);
	}

}

//
// weather_dayAfterTomorrowPlus1_loop
//
int weather_dayAfterTomorrowPlus1_loop() {
	int ret;

	threedays_pic.surface = IMG_Load("/exports/df3120-rootfs/tmp/weather-2.png");

	picframe_update(curr_window);

	SDL_FreeSurface(threedays_pic.surface);

	while (1) {
		ret = handle_input();
		if (ret)
			return ret;
		SDL_Delay(10);
	}

}

/*
 * Generic main loop to keep things running...
 */

int main() {
	int ret = 0;

	picframe_init();


	arrow_setup();
//	clock_setup();

	weather_today_setup();
	weather_tomorrow_setup();
	weather_dayAfterTomorrow_setup();
	weather_dayAfterTomorrowPlus1_setup();

	curr_window = picframe_get_window(START_WINDOW);

	picframe_set_backlight(500);

	while (1) {

		picframe_clear_screen();

		switch (curr_window_idx) {
		/*
		case 1:
			ret = clock_loop();
			break;
		*/
		case 1:
			ret = weather_today_loop();
			break;
		case 2:
			ret = weather_tomorrow_loop();
			break;
		case 3:
			ret = weather_dayAfterTomorrow_loop();
			break;
		case 4:
			ret = weather_dayAfterTomorrowPlus1_loop();
			break;
		default:
			break;
		}
		if (ret == 1)
			goto cleanup;

	}

	// Try a friendly shotdown to prevent memory leaks...
	cleanup: picframe_cleanup();

	app_free();

	return 0;
}
