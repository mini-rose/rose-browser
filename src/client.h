#pragma once

#include "window.h"

typedef struct
{
	RoseWindow *window;
	int id;
} RoseClient;

RoseClient *rose_client_new(void);

RoseClient *rose_client_get_by_window(RoseWindow *window);
RoseClient *rose_client_get_by_id(int);

void rose_client_destroy_by_window(RoseWindow *window);
void rose_client_destroy_by_id(int);
void rose_client_destroy_all(void);
