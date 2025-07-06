#include "animation.h"
#include <math.h>

// Cubic Bezier curve evaluation
vec3_t bezier(vec3_t p0, vec3_t p1, vec3_t p2, vec3_t p3, float t) {
    // Clamp t to [0, 1]
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    
    // Calculate Bezier curve using De Casteljau's algorithm
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;
    
    // B(t) = (1-t)³P₀ + 3(1-t)²tP₁ + 3(1-t)t²P₂ + t³P₃
    vec3_t result = vec3_from_cartesian(
        uuu * p0.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * p3.x,
        uuu * p0.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * p3.y,
        uuu * p0.z + 3 * uu * t * p1.z + 3 * u * tt * p2.z + ttt * p3.z
    );
    
    return result;
}

// Create an animation keyframe
animation_keyframe_t keyframe_create(vec3_t position, vec3_t rotation, vec3_t scale, float time) {
    animation_keyframe_t keyframe;
    keyframe.position = position;
    keyframe.rotation = rotation;
    keyframe.scale = scale;
    keyframe.time = time;
    return keyframe;
}

// Create an animation path
animation_path_t path_create(vec3_t p0, vec3_t p1, vec3_t p2, vec3_t p3, float duration) {
    animation_path_t path;
    path.p0 = p0;
    path.p1 = p1;
    path.p2 = p2;
    path.p3 = p3;
    path.duration = duration;
    return path;
}

// Evaluate animation path at given time
vec3_t path_evaluate(animation_path_t path, float time) {
    float t = time / path.duration;
    
    // Loop the animation
    t = fmodf(t, 1.0f);
    if (t < 0.0f) t += 1.0f;
    
    return bezier(path.p0, path.p1, path.p2, path.p3, t);
}

// Smooth step function for easing
float smooth_step(float t) {
    if (t <= 0.0f) return 0.0f;
    if (t >= 1.0f) return 1.0f;
    return t * t * (3.0f - 2.0f * t);
}

// Create a rotation animation around an axis
vec3_t rotate_around_axis(vec3_t axis, float angle, float time, float duration) {
    float t = time / duration;
    t = fmodf(t, 1.0f);
    if (t < 0.0f) t += 1.0f;
    
    float current_angle = t * angle;
    
    // Normalize axis
    float axis_len = sqrtf(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
    if (axis_len < 1e-6f) return vec3_from_cartesian(0, 0, 0);
    
    return vec3_from_cartesian(
        axis.x * current_angle / axis_len,
        axis.y * current_angle / axis_len,
        axis.z * current_angle / axis_len
    );
}