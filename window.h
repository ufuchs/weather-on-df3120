/*
 * window.h
 *
 *  Created on: Jul 11, 2013
 *      Author: ufuchs
 */

#ifndef WINDOW_H_
#define WINDOW_H_

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "datatypes.h"
#include "picframe.h"

/*
 * struct Params - holds the params for a single `Window`
 * @elements: linked list of 'Element_t'
 * @screen: application screen
 */
typedef struct {

	struct LList_t *elements;
	SDL_Surface *screen;

} Params;

/* function pointer to an event handler */
typedef  int (*eventHandler)(SDL_Event *event);

//
// This is the link between the windows param and and his functions
//
typedef struct {

	Params *params;

	/* non public window functions */
	void (*setup)(SDL_Surface *screen, Params *params);
	int (*wndProc)();
	void (*release)(Params *params);

} Window;

/*
 * Populate the `Window`s params
 * @screen: application screen
 * @params: holds the params for a single `Window`
 * @api: non public
 */
static inline void setup(SDL_Surface *screen, Params *params) {
	params->screen = screen;
};

/*
 * Frees the `Window`params
 * @params: holds the params for a single `Window`
 * @api: non public
 */
static inline void release(Params *params) {

	struct LList_t *curr = params->elements;

	/* free the linked list of 'Element_t' */
	while (curr) {

		struct LList_t *prev = curr;
		curr = curr->next;
		free(prev);

	}

	free(params);

};

/*
 * @api: non public
 */
static inline void initialize(Window *w, eventHandler wndProc) {

	w->params = (Params *) calloc(1, sizeof(Params));

	/* populate the function pointers */
	w->setup = setup;
	w->wndProc = wndProc;
	w->release = release;

}

/* public functions */
Window* windowFactory(SDL_Surface *, eventHandler event);
void window_free(Window *);
void window_update(Window* w);
struct LList_t * window_add_element(Window *w, Element_t *e);
Element_t * window_get_element_byName(Window *w, char *name);

#endif /* WINDOW_H_ */
