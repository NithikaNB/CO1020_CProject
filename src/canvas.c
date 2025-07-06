// canvas.c
#include "canvas.h"
#include <stdint.h> // in case it's not included already

canvas_t* canvas_create(int width, int height) {
    canvas_t* canvas = malloc(sizeof(canvas_t));
    if (!canvas) return NULL;
    
    canvas->width = width;
    canvas->height = height;
    
    // Allocate 2D array for pixels
    canvas->pixels = malloc(height * sizeof(float*));
    if (!canvas->pixels) {
        free(canvas);
        return NULL;
    }
    
    for (int i = 0; i < height; i++) {
        canvas->pixels[i] = calloc(width, sizeof(float));
        if (!canvas->pixels[i]) {
            // Clean up on failure
            for (int j = 0; j < i; j++) {
                free(canvas->pixels[j]);
            }
            free(canvas->pixels);
            free(canvas);
            return NULL;
        }
    }
    
    return canvas;
}

void canvas_destroy(canvas_t* canvas) {
    if (!canvas) return;
    
    for (int i = 0; i < canvas->height; i++) {
        free(canvas->pixels[i]);
    }
    free(canvas->pixels);
    free(canvas);
}

void canvas_clear(canvas_t* canvas) {
    if (!canvas) return;
    
    for (int y = 0; y < canvas->height; y++) {
        for (int x = 0; x < canvas->width; x++) {
            canvas->pixels[y][x] = 0.0f;
        }
    }
}

// Bilinear filtering for sub-pixel precision
void set_pixel_f(canvas_t* canvas, float x, float y, float intensity) {
    if (!canvas || intensity < 0.0f) return;
    
    // Clamp intensity to [0.0, 1.0]
    if (intensity > 1.0f) intensity = 1.0f;
    
    // Get the four surrounding integer pixel coordinates
    int x0 = (int)floor(x);
    int y0 = (int)floor(y);
    int x1 = x0 + 1;
    int y1 = y0 + 1;
    
    // Calculate fractional parts
    float fx = x - x0;
    float fy = y - y0;
    
    // Bilinear weights
    float w00 = (1.0f - fx) * (1.0f - fy);  // top-left
    float w10 = fx * (1.0f - fy);           // top-right
    float w01 = (1.0f - fx) * fy;           // bottom-left
    float w11 = fx * fy;                    // bottom-right
    
    // Apply weights to the four pixels (with bounds checking)
    if (x0 >= 0 && x0 < canvas->width && y0 >= 0 && y0 < canvas->height) {
        canvas->pixels[y0][x0] += w00 * intensity;
        if (canvas->pixels[y0][x0] > 1.0f) canvas->pixels[y0][x0] = 1.0f;
    }
    if (x1 >= 0 && x1 < canvas->width && y0 >= 0 && y0 < canvas->height) {
        canvas->pixels[y0][x1] += w10 * intensity;
        if (canvas->pixels[y0][x1] > 1.0f) canvas->pixels[y0][x1] = 1.0f;
    }
    if (x0 >= 0 && x0 < canvas->width && y1 >= 0 && y1 < canvas->height) {
        canvas->pixels[y1][x0] += w01 * intensity;
        if (canvas->pixels[y1][x0] > 1.0f) canvas->pixels[y1][x0] = 1.0f;
    }
    if (x1 >= 0 && x1 < canvas->width && y1 >= 0 && y1 < canvas->height) {
        canvas->pixels[y1][x1] += w11 * intensity;
        if (canvas->pixels[y1][x1] > 1.0f) canvas->pixels[y1][x1] = 1.0f;
    }
}

// DDA (Digital Differential Analyzer) line drawing with thickness
void draw_line_f(canvas_t* canvas, float x0, float y0, float x1, float y1, float thickness) {
    if (!canvas || thickness <= 0.0f) return;
    
    float dx = x1 - x0;
    float dy = y1 - y0;
    
    // Determine the number of steps
    int steps = (int)(fmax(fabs(dx), fabs(dy)) * 2); // Multiply by 2 for smoother lines
    if (steps == 0) {
        set_pixel_f(canvas, x0, y0, 1.0f);
        return;
    }
    
    float x_step = dx / steps;
    float y_step = dy / steps;
    
    // Calculate perpendicular direction for thickness
    float length = sqrt(dx * dx + dy * dy);
    if (length == 0) return;
    
    float perp_x = -dy / length * thickness / 2.0f;
    float perp_y = dx / length * thickness / 2.0f;
    
    // Draw the line with thickness
    for (int i = 0; i <= steps; i++) {
        float x = x0 + i * x_step;
        float y = y0 + i * y_step;
        
        // Draw thickness by drawing multiple pixels perpendicular to line direction
        int thickness_steps = (int)(thickness * 2) + 1;
        for (int t = 0; t < thickness_steps; t++) {
            float t_ratio = (thickness_steps == 1) ? 0 : (float)t / (thickness_steps - 1) - 0.5f;
            float px = x + perp_x * t_ratio * 2.0f;
            float py = y + perp_y * t_ratio * 2.0f;
            
            // Use gaussian-like falloff for smoother thickness
            float falloff = exp(-2.0f * t_ratio * t_ratio);
            set_pixel_f(canvas, px, py, falloff);
        }
    }
}

// Save canvas as PGM (Portable GrayMap) format for visualization
void canvas_save_pgm(canvas_t* canvas, const char* filename) {
    if (!canvas || !filename) return;
    
    FILE* file = fopen(filename, "w");
    if (!file) return;
    
    fprintf(file, "P2\n");
    fprintf(file, "%d %d\n", canvas->width, canvas->height);
    fprintf(file, "255\n");
    
    for (int y = 0; y < canvas->height; y++) {
        for (int x = 0; x < canvas->width; x++) {
            int gray_value = (int)(canvas->pixels[y][x] * 255);
            fprintf(file, "%d ", gray_value);
        }
        fprintf(file, "\n");
    }
    
    fclose(file);
}

void draw_circle(canvas_t* canvas, int center_x, int center_y, int radius, uint8_t intensity) {
    // Validate inputs
    if (!canvas || !canvas->pixels || canvas->width <= 0 || canvas->height <= 0) {
        fprintf(stderr, "Error: Invalid canvas or pixel buffer\n");
        return;
    }

    // Iterate over a bounding box around the circle
    int x_min = center_x - radius;
    int x_max = center_x + radius;
    int y_min = center_y - radius;
    int y_max = center_y + radius;

    // Clip to canvas boundaries
    x_min = x_min < 0 ? 0 : x_min;
    x_max = x_max >= canvas->width ? canvas->width - 1 : x_max;
    y_min = y_min < 0 ? 0 : y_min;
    y_max = y_max >= canvas->height ? canvas->height - 1 : y_max;

    // Draw filled circle
    for (int y = y_min; y <= y_max; y++) {
        for (int x = x_min; x <= x_max; x++) {
            // Check if the pixel is within the circle's radius
            int dx = x - center_x;
            int dy = y - center_y;
            if (dx * dx + dy * dy <= radius * radius) {
                // Ensure index is within bounds
                size_t index = (size_t)y * canvas->width + x;
                if (index < (size_t)(canvas->width * canvas->height)) {
                    canvas->pixels[index] = intensity;
                } else {
                    fprintf(stderr, "Error: Pixel index out of bounds (%zu, max: %zu)\n",
                            index, (size_t)(canvas->width * canvas->height));
                }
            }
        }
    }
}