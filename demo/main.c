// main.c - Demo implementation
#include "canvas.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main() {
    // Create a 400x400 canvas
    canvas_t* canvas = canvas_create(400, 400);
    if (!canvas) {
        printf("Failed to create canvas\n");
        return 1;
    }
    
    canvas_clear(canvas);
    
    // Center of the canvas
    float center_x = canvas->width / 2.0f;
    float center_y = canvas->height / 2.0f;
    float radius = 150.0f;
    
    // Draw lines at 15-degree steps (like a clock face)
    for (int angle_deg = 0; angle_deg < 360; angle_deg += 15) {
        float angle_rad = angle_deg * M_PI / 180.0f;
        
        // Calculate end point
        float end_x = center_x + radius * cos(angle_rad);
        float end_y = center_y + radius * sin(angle_rad);
        
        // Draw line from center to end point
        draw_line_f(canvas, center_x, center_y, end_x, end_y, 1.5f);
    }
    
    // Save the result
    canvas_save_pgm(canvas, "clock_demo.pgm");
    printf("Clock demo saved to clock_demo.pgm\n");
    printf("You can view this file with image viewers that support PGM format\n");
    
    canvas_destroy(canvas);
    return 0;
}