// canvas.h
#ifndef CANVAS_H
#define CANVAS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>


typedef struct {
    int width;
    int height;
    float **pixels;  // 2D array of brightness values [0.0, 1.0]
} canvas_t;

// Function declarations
canvas_t* canvas_create(int width, int height);
void canvas_destroy(canvas_t* canvas);
void canvas_clear(canvas_t* canvas);
void set_pixel_f(canvas_t* canvas, float x, float y, float intensity);
void draw_line_f(canvas_t* canvas, float x0, float y0, float x1, float y1, float thickness);
void canvas_save_pgm(canvas_t* canvas, const char* filename);
void draw_circle(canvas_t* canvas, int center_x, int center_y, int radius, uint8_t intensity);

#endif