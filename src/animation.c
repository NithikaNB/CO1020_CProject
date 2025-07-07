#include "animation.h"
#include <math.h>
#include <stdio.h>

// Cubic Bezier curve evaluation
vec3_t bezier(vec3_t p0, vec3_t p1, vec3_t p2, vec3_t p3, float t) {
    // Clamp t to [0, 1]
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    
    // Precompute terms for efficiency
    float u = 1.0f - t;
    float t2 = t * t;
    float u2 = u * u;
    float u3 = u2 * u;
    float t3 = t2 * t;
    float c1 = 3.0f * u2 * t;
    float c2 = 3.0f * u * t2;
    
    // B(t) = u³P₀ + 3u²tP₁ + 3ut²P₂ + t³P₃
    vec3_t result = {
        u3 * p0.x + c1 * p1.x + c2 * p2.x + t3 * p3.x,
        u3 * p0.y + c1 * p1.y + c2 * p2.y + t3 * p3.y,
        u3 * p0.z + c1 * p1.z + c2 * p2.z + t3 * p3.z
    };
    
    #ifdef DEBUG
    printf("Bezier t=%.3f: (%.2f, %.2f, %.2f)\n", t, result.x, result.y, result.z);
    #endif
    
    return result;
}

// Create an animation keyframe
animation_keyframe_t keyframe_create(vec3_t position, vec3_t rotation, vec3_t scale, float time) {
    animation_keyframe_t keyframe;
    keyframe.position = position;
    keyframe.rotation = rotation;
    keyframe.scale = scale;
    keyframe.time = time;
    
    #ifdef DEBUG
    printf("Created keyframe at t=%.3f: pos=(%.2f, %.2f, %.2f), rot=(%.2f, %.2f, %.2f), scale=(%.2f, %.2f, %.2f)\n",
           time, position.x, position.y, position.z, rotation.x, rotation.y, rotation.z, scale.x, scale.y, scale.z);
    #endif
    
    return keyframe;
}

// Create an animation path
animation_path_t path_create(vec3_t p0, vec3_t p1, vec3_t p2, vec3_t p3, float duration) {
    animation_path_t path;
    
    // If p0 is at origin, assume user wants a default orbital path around (0,0,0)
    float dist = sqrtf(p0.x * p0.x + p0.y * p0.y + p0.z * p0.z);
    if (dist < 1e-6f) {
        #ifdef DEBUG
        printf("Origin detected, creating default orbital path\n");
        #endif
        // Create a circular orbit in the XY plane with radius 1
        float r = 1.0f; // Default radius
        path.p0 = vec3_from_cartesian(r, 0.0f, 0.0f);
        path.p1 = vec3_from_cartesian(r, r * 0.552f, 0.0f); // Bézier control point for circle
        path.p2 = vec3_from_cartesian(-r * 0.552f, r, 0.0f);
        path.p3 = vec3_from_cartesian(-r, 0.0f, 0.0f);
    } else {
        path.p0 = p0;
        path.p1 = p1;
        path.p2 = p2;
        path.p3 = p3;
    }
    
    path.duration = duration > 0.0f ? duration : 1.0f; // Prevent invalid duration
    
    #ifdef DEBUG
    printf("Created path: duration=%.3f, p0=(%.2f, %.2f, %.2f), p3=(%.2f, %.2f, %.2f)\n",
           path.duration, path.p0.x, path.p0.y, path.p0.z, path.p3.x, path.p3.y, path.p3.z);
    #endif
    
    return path;
}

// Evaluate animation path at given time
vec3_t path_evaluate(animation_path_t path, float time) {
    if (path.duration <= 0.0f) {
        #ifdef DEBUG
        printf("Warning: Invalid path duration (%.3f), returning p0\n", path.duration);
        #endif
        return path.p0;
    }
    
    float t = time / path.duration;
    t = fmodf(t, 1.0f);
    if (t < 0.0f) t += 1.0f;
    
    // Apply quintic easing for smoother orbital motion
    float t_eased = t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f); // t^5 easing
    
    vec3_t result = bezier(path.p0, path.p1, path.p2, path.p3, t_eased);
    
    #ifdef DEBUG
    printf("Path evaluated at t=%.3f (eased): pos=(%.2f, %.2f, %.2f)\n", t, result.x, result.y, result.z);
    #endif
    
    return result;
}


// Smooth step function for easing
float smooth_step(float t) {
    if (t <= 0.0f) return 0.0f;
    if (t >= 1.0f) return 1.0f;
    return t * t * (3.0f - 2.0f * t);
}

// Create a rotation animation around an axis
vec3_t rotate_around_axis(vec3_t axis, float angle, float time, float duration) {
    if (duration <= 0.0f) {
        #ifdef DEBUG
        printf("Warning: Invalid rotation duration (%.3f), returning zero rotation\n", duration);
        #endif
        return vec3_from_cartesian(0, 0, 0);
    }
    
    float t = time / duration;
    t = fmodf(t, 1.0f);
    if (t < 0.0f) t += 1.0f;
    
    // Apply quintic easing for smoother rotation
    t = t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f); // t^5 easing
    
    // Normalize axis
    float axis_len = sqrtf(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
    if (axis_len < 1e-6f) {
        #ifdef DEBUG
        printf("Warning: Zero-length axis, using default Z-axis for orbit\n");
        #endif
        // Default to Z-axis for orbiting around center
        axis = vec3_from_cartesian(0, 0, 1);
        axis_len = 1.0f;
    }
    
    float current_angle = t * angle;
    float inv_len = 1.0f / axis_len;
    
    // Orient toward center (0,0,0) by adjusting rotation
    vec3_t result = {
        axis.x * inv_len * current_angle,
        axis.y * inv_len * current_angle,
        axis.z * inv_len * current_angle
    };
    
    #ifdef DEBUG
    printf("Rotation at t=%.3f: angle=%.2f, axis=(%.2f, %.2f, %.2f), result=(%.2f, %.2f, %.2f)\n",
           t, current_angle, axis.x, axis.y, axis.z, result.x, result.y, result.z);
    #endif
    
    return result;
}