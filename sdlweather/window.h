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
#include <assert.h>

//
//
//
typedef struct {

	struct LList_t *elements;
	SDL_Surface *screen;

} Params;

typedef  int (*eventHandler)(SDL_Event *event);

//
//
//
typedef struct {

	Params *params;

	void (*setup)(SDL_Surface *screen, Params *params);
	int (*wndProc)();
	void (*free)(Params *params);

} Window;

//
//
//
static inline void setup(SDL_Surface *screen, Params *params) {
	params->screen = screen;
};

//
//
//
static inline void free_local(Params *params) {

	struct LList_t *curr = params->elements;

	while (curr) {

		struct LList_t *prev = curr;

		curr = curr->next;

		free(prev);

	}

};

Window* windowFactory(SDL_Surface *, eventHandler event);
void window_free(Window *);
void window_update(Window* w);
struct LList_t * window_add_element(Window *w, Element_t *e);
Element_t * window_get_element_byName(Window *w, char *name);

#endif /* WINDOW_H_ */
