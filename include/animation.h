#ifndef ANIMATION_H
#define ANIMATION_H

#include "math3d.h"

// Animation keyframe structure
typedef struct {
    vec3_t position;
    vec3_t rotation;
    vec3_t scale;
    float time;
} animation_keyframe_t;

// Animation path structure for Bezier curves
typedef struct {
    vec3_t p0, p1, p2, p3;  // Control points
    float duration;         // Duration of the animation
} animation_path_t;

// Cubic Bezier curve evaluation
vec3_t bezier(vec3_t p0, vec3_t p1, vec3_t p2, vec3_t p3, float t);

// Create an animation keyframe
animation_keyframe_t keyframe_create(vec3_t position, vec3_t rotation, vec3_t scale, float time);

// Create an animation path
animation_path_t path_create(vec3_t p0, vec3_t p1, vec3_t p2, vec3_t p3, float duration);

// Evaluate animation path at given time
vec3_t path_evaluate(animation_path_t path, float time);

// Smooth step function for easing
float smooth_step(float t);

// Create a rotation animation around an axis
vec3_t rotate_around_axis(vec3_t axis, float angle, float time, float duration);

#endif // ANIMATION_H