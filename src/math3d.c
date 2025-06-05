#include "math3d.h"
#include <string.h>

// Fast inverse square root (Quake III algorithm)
float fast_inv_sqrt(float x) {
    float xhalf = 0.5f * x;
    
    // Use union to avoid strict aliasing violations
    union {
        float f;
        int i;
    } u;
    
    u.f = x;
    u.i = 0x5f3759df - (u.i >> 1);
    x = u.f;
    x = x * (1.5f - xhalf * x * x);
    return x;
}

// Utility functions
float deg_to_rad(float degrees) {
    return degrees * PI / 180.0f;
}

float rad_to_deg(float radians) {
    return radians * 180.0f / PI;
}

// Vector creation and conversion
vec3_t vec3_create(float x, float y, float z) {
    vec3_t v;
    v.x = x;
    v.y = y;
    v.z = z;
    v._cartesian_dirty = 0;
    v._spherical_dirty = 1;
    vec3_update_spherical(&v);
    return v;
}

vec3_t vec3_from_spherical(float r, float theta, float phi) {
    vec3_t v;
    v.r = r;
    v.theta = theta;
    v.phi = phi;
    v._spherical_dirty = 0;
    v._cartesian_dirty = 1;
    vec3_update_cartesian(&v);
    return v;
}

void vec3_update_spherical(vec3_t* v) {
    if (!v->_spherical_dirty) return;
    
    v->r = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
    
    if (v->r < 1e-6f) {
        v->theta = 0.0f;
        v->phi = 0.0f;
    } else {
        v->theta = atan2f(v->y, v->x);
        v->phi = acosf(v->z / v->r);
    }
    
    v->_spherical_dirty = 0;
}

void vec3_update_cartesian(vec3_t* v) {
    if (!v->_cartesian_dirty) return;
    
    float sin_phi = sinf(v->phi);
    v->x = v->r * sin_phi * cosf(v->theta);
    v->y = v->r * sin_phi * sinf(v->theta);
    v->z = v->r * cosf(v->phi);
    
    v->_cartesian_dirty = 0;
}

float vec3_magnitude(vec3_t v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3_t vec3_normalize_fast(vec3_t v) {
    float inv_mag = fast_inv_sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    vec3_t result;
    result.x = v.x * inv_mag;
    result.y = v.y * inv_mag;
    result.z = v.z * inv_mag;
    result._cartesian_dirty = 0;
    result._spherical_dirty = 1;
    vec3_update_spherical(&result);
    return result;
}

float vec3_dot(vec3_t a, vec3_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3_t vec3_cross(vec3_t a, vec3_t b) {
    vec3_t result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    result._cartesian_dirty = 0;
    result._spherical_dirty = 1;
    vec3_update_spherical(&result);
    return result;
}

vec3_t vec3_slerp(vec3_t a, vec3_t b, float t) {
    // Normalize input vectors
    vec3_t na = vec3_normalize_fast(a);
    vec3_t nb = vec3_normalize_fast(b);
    
    // Calculate angle between vectors
    float dot = vec3_dot(na, nb);
    
    // Clamp dot product to avoid numerical errors
    if (dot > 1.0f) dot = 1.0f;
    if (dot < -1.0f) dot = -1.0f;
    
    float theta = acosf(fabsf(dot));
    
    // If vectors are nearly parallel, use linear interpolation
    if (theta < 1e-6f) {
        vec3_t result;
        result.x = na.x * (1.0f - t) + nb.x * t;
        result.y = na.y * (1.0f - t) + nb.y * t;
        result.z = na.z * (1.0f - t) + nb.z * t;
        result._cartesian_dirty = 0;
        result._spherical_dirty = 1;
        vec3_update_spherical(&result);
        return result;
    }
    
    // Spherical linear interpolation
    float sin_theta = sinf(theta);
    float w1 = sinf((1.0f - t) * theta) / sin_theta;
    float w2 = sinf(t * theta) / sin_theta;
    
    vec3_t result;
    result.x = na.x * w1 + nb.x * w2;
    result.y = na.y * w1 + nb.y * w2;
    result.z = na.z * w1 + nb.z * w2;
    result._cartesian_dirty = 0;
    result._spherical_dirty = 1;
    vec3_update_spherical(&result);
    return result;
}

vec3_t vec3_add(vec3_t a, vec3_t b) {
    return vec3_create(a.x + b.x, a.y + b.y, a.z + b.z);
}

vec3_t vec3_subtract(vec3_t a, vec3_t b) {
    return vec3_create(a.x - b.x, a.y - b.y, a.z - b.z);
}

vec3_t vec3_scale(vec3_t v, float s) {
    return vec3_create(v.x * s, v.y * s, v.z * s);
}

void vec3_print(vec3_t v) {
    vec3_update_spherical((vec3_t*)&v);
    printf("Vec3: Cart(%.3f, %.3f, %.3f) Sph(r=%.3f, θ=%.3f°, φ=%.3f°)\n",
           v.x, v.y, v.z, v.r, rad_to_deg(v.theta), rad_to_deg(v.phi));
}

// Matrix operations
mat4_t mat4_identity(void) {
    mat4_t m;
    memset(m.m, 0, sizeof(m.m));
    m.m[0] = 1.0f;   // m[0][0]
    m.m[5] = 1.0f;   // m[1][1]
    m.m[10] = 1.0f;  // m[2][2]
    m.m[15] = 1.0f;  // m[3][3]
    return m;
}

mat4_t mat4_translate(float tx, float ty, float tz) {
    mat4_t m = mat4_identity();
    m.m[12] = tx;  // m[3][0]
    m.m[13] = ty;  // m[3][1]
    m.m[14] = tz;  // m[3][2]
    return m;
}

mat4_t mat4_scale(float sx, float sy, float sz) {
    mat4_t m = mat4_identity();
    m.m[0] = sx;   // m[0][0]
    m.m[5] = sy;   // m[1][1]
    m.m[10] = sz;  // m[2][2]
    return m;
}

mat4_t mat4_rotate_xyz(float rx, float ry, float rz) {
    // Convert to radians
    rx = deg_to_rad(rx);
    ry = deg_to_rad(ry);
    rz = deg_to_rad(rz);
    
    // Rotation matrices
    float cx = cosf(rx), sx = sinf(rx);
    float cy = cosf(ry), sy = sinf(ry);
    float cz = cosf(rz), sz = sinf(rz);
    
    // Combined rotation matrix (ZYX order)
    mat4_t m = mat4_identity();
    
    m.m[0] = cy * cz;
    m.m[1] = cx * sz + sx * sy * cz;
    m.m[2] = sx * sz - cx * sy * cz;
    m.m[3] = 0.0f;
    
    m.m[4] = -cy * sz;
    m.m[5] = cx * cz - sx * sy * sz;
    m.m[6] = sx * cz + cx * sy * sz;
    m.m[7] = 0.0f;
    
    m.m[8] = sy;
    m.m[9] = -sx * cy;
    m.m[10] = cx * cy;
    m.m[11] = 0.0f;
    
    m.m[12] = 0.0f;
    m.m[13] = 0.0f;
    m.m[14] = 0.0f;
    m.m[15] = 1.0f;
    
    return m;
}

mat4_t mat4_frustum_asymmetric(float left, float right, float bottom, float top, float near, float far) {
    mat4_t m;
    memset(m.m, 0, sizeof(m.m));
    
    float rl = right - left;
    float tb = top - bottom;
    float fn = far - near;
    
    m.m[0] = (2.0f * near) / rl;
    m.m[5] = (2.0f * near) / tb;
    m.m[8] = (right + left) / rl;
    m.m[9] = (top + bottom) / tb;
    m.m[10] = -(far + near) / fn;
    m.m[11] = -1.0f;
    m.m[14] = -(2.0f * far * near) / fn;
    
    return m;
}

mat4_t mat4_multiply(mat4_t a, mat4_t b) {
    mat4_t result;
    
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            result.m[col * 4 + row] = 0.0f;
            for (int k = 0; k < 4; k++) {
                result.m[col * 4 + row] += a.m[k * 4 + row] * b.m[col * 4 + k];
            }
        }
    }
    
    return result;
}

vec3_t mat4_transform_point(mat4_t m, vec3_t v) {
    float x = m.m[0] * v.x + m.m[4] * v.y + m.m[8] * v.z + m.m[12];
    float y = m.m[1] * v.x + m.m[5] * v.y + m.m[9] * v.z + m.m[13];
    float z = m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14];
    float w = m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15];
    
    // Perspective divide
    if (fabsf(w) > 1e-6f) {
        x /= w;
        y /= w;
        z /= w;
    }
    
    return vec3_create(x, y, z);
}

void mat4_print(mat4_t m) {
    printf("Matrix 4x4:\n");
    for (int row = 0; row < 4; row++) {
        printf("[");
        for (int col = 0; col < 4; col++) {
            printf("%8.3f", m.m[col * 4 + row]);
            if (col < 3) printf(", ");
        }
        printf("]\n");
    }
    printf("\n");
}