#include "picframe.h"
#include <time.h>

#define START_WINDOW 2
#define NUM_WINDOWS 5

/* General elements for all windows */
Element_t left_arrow_icon, right_arrow_icon, window_label;

/* Clock window elements */
Element_t time_disp, info_disp;

/* Media window elements */
Element_t progress_bar;
Element_t media_btns[5];

char *window_labels[NUM_WINDOWS] = { "Date and Time", "Media Center", "Wther",
		"Lights", "Security Cam" };
/* Weather window elements */
Element_t today_pic, tomorrow_pic, twodays_pic, threedays_pic;

#define WEATHER_ICON_WIDTH 50
#define NUM_WEATHER_VAR 12

#define WEATHER_SUNNY 0
#define WEATHER_SUNCLOUD 1
#define WEATHER_CLOUDY 2
#define WEATHER_RAINY_LOW 3
#define WEATHER_RAINY_SUNNY 4
#define WEATHER_RAINY_THUNDER_SUNNY 5
#define WEATHER_MIXED 6
#define WEATHER_RAINY 7
#define WEATHER_THUNDER 8
#define WEATHER_WINDY 9
#define WEATHER_FOGGY 10
#define WEATHER_SNOWY 11
#define DATA_DIR "/usr/share"

char *weather_pics[NUM_WEATHER_VAR] = { "data/weather/skc.png",
		"data/weather/sct.png", "data/weather/ovc.png", "data/weather/fzra.png",
		"data/weather/hi_shwrs.png", "data/weather/hi_tsra.png",
		"data/weather/mix.png", "data/weather/ra1.png", "data/weather/tsra.png",
		"data/weather/wind.png", "data/weather/fg.png", "data/weather/sn.png" };
/* Security window */
Element_t security_cam;

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

//	picframe_gpio_handle_events();

	while (picframe_get_event(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			printf("Quitting\n");
			return 1;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_LEFT) {
				left_arrow_icon.selected = 1;
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
				right_arrow_icon.selected = 1;
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
				left_arrow_icon.selected = 0;
			}

			if (event.key.keysym.sym == SDLK_RIGHT) {
				right_arrow_icon.selected = 0;
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
 * Setup routine for the clock window
 */
void clock_setup() {
	struct LList_t *first_window = NULL;
	first_window = picframe_get_window(1);
	SDL_Rect tmp;

	/* Left arrow */
	tmp.x = 20;
	tmp.y = 15;
	picframe_add_button(&left_arrow_icon, &tmp,
			"/usr/share/data/glyphicons_210_left_arrow.png",
			"/usr/share/data/glyphicons_210_left_arrow_selected.png");
	picframe_add_element_to_window(first_window, &left_arrow_icon);

	/* Right arrow */
	tmp.x = 320 - 40;
	tmp.y = 15;
	right_arrow_icon.selected = 1;
	picframe_add_button(&right_arrow_icon, &tmp,
			"/usr/share/data/glyphicons_211_right_arrow.png",
			"/usr/share/data/glyphicons_211_right_arrow_selected.png");
	picframe_add_element_to_window(first_window, &right_arrow_icon);

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

	picframe_gen_text(&window_label.surface, fg, bg, window_labels[0]);
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

void second_window_setup() {
	struct LList_t *second_window = NULL;
	SDL_Rect tmp;
	int i = 0;
	char *media_btn_array[] = { "/usr/share/data/glyphicons_172_rewind.png",
			"/usr/share/data/glyphicons_173_play.png",
			"/usr/share/data/glyphicons_174_pause.png",
			"/usr/share/data/glyphicons_175_stop.png",
			"/usr/share/data/glyphicons_176_forward.png" };
	second_window = picframe_add_window();
	tmp.x = 50;
	tmp.y = 160;
	for (i = 0; i < 5; i++) {
		picframe_add_button(&media_btns[i], &tmp, media_btn_array[i],
				media_btn_array[i]);
		picframe_add_element_to_window(second_window, &media_btns[i]);
		tmp.x += 50;
	}

	picframe_add_element_to_window(second_window, &left_arrow_icon);
	picframe_add_element_to_window(second_window, &right_arrow_icon);
	picframe_add_element_to_window(second_window, &window_label);

	tmp.x = 10;
	tmp.y = (HEIGHT - 20);
	tmp.w = (WIDTH - 20);
	tmp.h = 10;
	picframe_add_progress_bar(&progress_bar, &tmp, 0);
	picframe_add_element_to_window(second_window, &progress_bar);
}

int second_window_loop() {
	int ret, dir = 2;
	time_t now, prev = 0;

	SDL_FreeSurface(window_label.surface);
	picframe_load_font("/usr/share/fonts/Ubuntu-L.ttf", 30);
	picframe_gen_text(&window_label.surface, fg, bg, window_labels[1]);
	window_label.rect.x = (WIDTH / 2) - (window_label.surface->clip_rect.w / 2);

	while (1) {
		ret = handle_input();
		if (ret)
			return ret;

		now = time(0);
		if (now > prev) {
			progress += dir;
			if (progress > 100) {
				progress = 100;
				dir = -2;
			}
			if (progress < 0) {
				progress = 0;
				dir = 2;
			}
			prev = now;
			picframe_update_progress_bar(&progress_bar, progress);
		}

		picframe_update(curr_window);
		SDL_Delay(1);
	}
	return 0;
}

void third_window_setup() {
	struct LList_t *third_window = NULL;
	third_window = picframe_add_window();

	picframe_add_element_to_window(third_window, &left_arrow_icon);
	picframe_add_element_to_window(third_window, &right_arrow_icon);
	picframe_add_element_to_window(third_window, &window_label);

	today_pic.surface = NULL;
	today_pic.rect.x = (WIDTH / 6) - (WEATHER_ICON_WIDTH / 2);
	today_pic.rect.y = 50;
	today_pic.selected = 0;
	today_pic.dynamic = 0;
	picframe_add_element_to_window(third_window, &today_pic);

	/*
	tomorrow_pic.surface = NULL;
	tomorrow_pic.rect.x = (WIDTH / 6) - (WEATHER_ICON_WIDTH / 2);
	tomorrow_pic.rect.y = (HEIGHT * 0.5);
	tomorrow_pic.selected = 0;
	tomorrow_pic.dynamic = 0;
	picframe_add_element_to_window(third_window, &tomorrow_pic);

	twodays_pic.surface = NULL;
	twodays_pic.rect.x = ((WIDTH / 2)) - (WEATHER_ICON_WIDTH / 2);
	twodays_pic.rect.y = (HEIGHT * 0.5);
	twodays_pic.selected = 0;
	twodays_pic.dynamic = 0;
	picframe_add_element_to_window(third_window, &twodays_pic);

	threedays_pic.surface = NULL;
	threedays_pic.rect.x = (WIDTH - (WIDTH / 6)) - (WEATHER_ICON_WIDTH / 2);
	threedays_pic.rect.y = (HEIGHT * 0.5);
	threedays_pic.selected = 0;
	threedays_pic.dynamic = 0;
	picframe_add_element_to_window(third_window, &threedays_pic);
	*/
}

int today_loop() {
	int ret;


	SDL_FreeSurface(window_label.surface);
	picframe_load_font("/usr/share/fonts/Ubuntu-L.ttf", 30);
	picframe_gen_text(&window_label.surface, fg, bg, window_labels[2]);
	window_label.rect.x = (WIDTH / 2) - (window_label.surface->clip_rect.w / 2);

	picframe_load_image(&today_pic.surface, "/nfs/tmp/weather-0.png");
	/*
	picframe_load_image(&tomorrow_pic.surface, weather_pics[WEATHER_CLOUDY]);
	picframe_load_image(&twodays_pic.surface, weather_pics[WEATHER_MIXED]);
	picframe_load_image(&threedays_pic.surface, weather_pics[WEATHER_SNOWY]);
	*/

	/*
	SDL_Rect sep;
	sep.x = (WIDTH / 3);
	sep.y = (HEIGHT * 0.5);
	sep.w = 1;
	sep.h = (HEIGHT * 0.5);
	picframe_draw_line(sep);
	sep.x = (WIDTH / 3) * 2;
	picframe_draw_line(sep);
	*/

	while (1) {
		ret = handle_input();
		if (ret)
			return ret;

		picframe_update(curr_window);
		SDL_Delay(1);
	}
	return 0;
	// TODO: Free up all the images when the window is closed
	SDL_FreeSurface(today_pic.surface);
	SDL_FreeSurface(tomorrow_pic.surface);
	SDL_FreeSurface(twodays_pic.surface);
	SDL_FreeSurface(threedays_pic.surface);
}

void fourth_window_setup() {
	int i;
	struct LList_t *fourth_window = NULL;
	fourth_window = picframe_add_window();

	picframe_add_element_to_window(fourth_window, &left_arrow_icon);
	picframe_add_element_to_window(fourth_window, &right_arrow_icon);
	picframe_add_element_to_window(fourth_window, &window_label);

	SDL_Rect tmp;
	tmp.x = (WIDTH - 90);
	tmp.y = 60;
	tmp.w = 80;
	tmp.h = 40;

	for (i = 0; i < NUM_APPLIANCES; i++) {
		picframe_add_button_text(&appls[i], &tmp, 26, "ON");
		picframe_add_element_to_window(fourth_window, &appls[i]);
		tmp.y += tmp.h + 5;
	}
}

int fourth_window_loop() {
	int ret;

	SDL_FreeSurface(window_label.surface);
	picframe_load_font("/usr/share/fonts/Ubuntu-L.ttf", 30);
	picframe_gen_text(&window_label.surface, fg, bg, window_labels[3]);
	window_label.rect.x = (WIDTH / 2) - (window_label.surface->clip_rect.w / 2);

	while (1) {
		ret = handle_input();
		if (ret)
			return ret;

		picframe_update(curr_window);
		SDL_Delay(1);
	}
	return 0;
}
void fifth_window_setup() {
	struct LList_t *fifth_window = NULL;
	fifth_window = picframe_add_window();

	picframe_add_element_to_window(fifth_window, &left_arrow_icon);
	picframe_add_element_to_window(fifth_window, &right_arrow_icon);
	picframe_add_element_to_window(fifth_window, &window_label);

	security_cam.surface = NULL;
	security_cam.rect.x = 0;
	security_cam.rect.y = 45;
	security_cam.selected = 0;
	security_cam.dynamic = 0;
	picframe_add_element_to_window(fifth_window, &security_cam);
}

int fifth_window_loop() {
	int ret;

	SDL_FreeSurface(window_label.surface);
	picframe_load_font("/usr/share/fonts/Ubuntu-L.ttf", 30);
	picframe_gen_text(&window_label.surface, fg, bg, window_labels[4]);
	window_label.rect.x = (WIDTH / 2) - (window_label.surface->clip_rect.w / 2);

	picframe_load_image(&security_cam.surface, "/nfs/tmp/weather-0.png");

	while (1) {
		ret = handle_input();
		if (ret)
			return ret;

		picframe_update(curr_window);
		SDL_Delay(1);
	}
	return 0;
	// TODO: Free up all the images when the window is closed
	SDL_FreeSurface(security_cam.surface);
}

/*
 * Generic main loop to keep things running...
 */

int main() {
	int ret = 0;

	picframe_init();

	clock_setup();
	second_window_setup();
	third_window_setup();
	fourth_window_setup();
	fifth_window_setup();

	curr_window = picframe_get_window(START_WINDOW);

	picframe_set_backlight(500);

	while (1) {
		picframe_clear_screen();
		switch (curr_window_idx) {
		case 1:
			ret = clock_loop();
			break;
		case 2:
			ret = second_window_loop();
			break;
		case 3:
			ret = today_loop();
			break;
		case 4:
			ret = fourth_window_loop();
			break;
		case 5:
			ret = fifth_window_loop();
			break;
		default:
			break;
		}
		if (ret == 1)
			goto cleanup;

		picframe_update(curr_window);
		SDL_Delay(1);
	}

	cleanup: picframe_cleanup();

	return 0;
}
