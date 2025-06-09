// In math3d.c

#include "math3d.h"

// --- vec3 functions (unchanged) ---
vec3_t vec3_from_cartesian(float x, float y, float z) {
    vec3_t v = { x, y, z };
    vec3_update_spherical(&v);
    return v;
}

vec3_t vec3_from_spherical(float r, float theta, float phi) {
    vec3_t v = { 0 };
    v.r = r;
    v.theta = theta;
    v.phi = phi;
    vec3_update_cartesian(&v);
    return v;
}

void vec3_update_spherical(vec3_t* v) {
    v->r = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
    v->theta = atan2f(v->y, v->x);
    v->phi = (v->r == 0.0f) ? 0.0f : acosf(v->z / v->r);
}

void vec3_update_cartesian(vec3_t* v) {
    float sin_phi = sinf(v->phi);
    v->x = v->r * sin_phi * cosf(v->theta);
    v->y = v->r * sin_phi * sinf(v->theta);
    v->z = v->r * cosf(v->phi);
}

vec3_t vec3_normalize_fast(vec3_t v) {
    float length_sq = v.x * v.x + v.y * v.y + v.z * v.z;
    float inv_sqrt = 1.0f / sqrtf(length_sq);

    v.x *= inv_sqrt;
    v.y *= inv_sqrt;
    v.z *= inv_sqrt;

    vec3_update_spherical(&v);
    return v;
}

vec3_t vec3_slerp(vec3_t a, vec3_t b, float t) {
    a = vec3_normalize_fast(a);
    b = vec3_normalize_fast(b);

    float dot = a.x * b.x + a.y * b.y + a.z * b.z;
    dot = fmaxf(fminf(dot, 1.0f), -1.0f);

    float theta = acosf(dot) * t;
    vec3_t relative = {
        b.x - a.x * dot,
        b.y - a.y * dot,
        b.z - a.z * dot
    };

    float len = sqrtf(relative.x * relative.x + relative.y * relative.y + relative.z * relative.z);
    if (len > 0.0001f) {
        relative.x /= len;
        relative.y /= len;
        relative.z /= len;
    }

    float cos_theta = cosf(theta);
    float sin_theta = sinf(theta);

    vec3_t result = {
        a.x * cos_theta + relative.x * sin_theta,
        a.y * cos_theta + relative.y * sin_theta,
        a.z * cos_theta + relative.z * sin_theta
    };

    vec3_update_spherical(&result);
    return result;
}

void vec3_print_cartesian(const vec3_t* v) {
    printf("Cartesian: (x=%.3f, y=%.3f, z=%.3f)\n", v->x, v->y, v->z);
}

void vec3_print_spherical(const vec3_t* v) {
    printf("Spherical: (r=%.3f, θ=%.3f rad, φ=%.3f rad)\n", v->r, v->theta, v->phi);
}

// --- mat4 functions ---

mat4_t mat4_identity(void) {
    mat4_t m = { 0 };
    m.m[0] = 1.0f; m.m[5] = 1.0f; m.m[10] = 1.0f; m.m[15] = 1.0f;
    return m;
}

mat4_t mat4_translate(float tx, float ty, float tz) {
    mat4_t m = mat4_identity();
    m.m[12] = tx;
    m.m[13] = ty;
    m.m[14] = tz;
    return m;
}

mat4_t mat4_scale(float sx, float sy, float sz) {
    mat4_t m = { 0 };
    m.m[0] = sx;
    m.m[5] = sy;
    m.m[10] = sz;
    m.m[15] = 1.0f;
    return m;
}

static mat4_t mat4_rotate_x(float angle) {
    mat4_t m = mat4_identity();
    float c = cosf(angle);
    float s = sinf(angle);
    m.m[5] = c;
    m.m[6] = s;
    m.m[9] = -s;
    m.m[10] = c;
    return m;
}

static mat4_t mat4_rotate_y(float angle) {
    mat4_t m = mat4_identity();
    float c = cosf(angle);
    float s = sinf(angle);
    m.m[0] = c;
    m.m[2] = -s;
    m.m[8] = s;
    m.m[10] = c;
    return m;
}

static mat4_t mat4_rotate_z(float angle) {
    mat4_t m = mat4_identity();
    float c = cosf(angle);
    float s = sinf(angle);
    m.m[0] = c;
    m.m[1] = s;
    m.m[4] = -s;
    m.m[5] = c;
    return m;
}


mat4_t mat4_multiply(mat4_t a, mat4_t b) {
    mat4_t result = { 0 };
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            float sum = 0.0f;
            for (int i = 0; i < 4; i++) {
                sum += a.m[i * 4 + row] * b.m[col * 4 + i];
            }
            result.m[col * 4 + row] = sum;
        }
    }
    return result;
}

mat4_t mat4_rotate_xyz(float rx, float ry, float rz) {
    mat4_t rxm = mat4_rotate_x(rx);
    mat4_t rym = mat4_rotate_y(ry);
    mat4_t rzm = mat4_rotate_z(rz);
    mat4_t r = mat4_multiply(rym, rxm); // Y * X
    r = mat4_multiply(rzm, r);          // Z * (Y * X)
    return r;
}

mat4_t mat4_frustum(float left, float right, float bottom, float top, float near, float far) {
    mat4_t m = { 0 };

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
    m.m[15] = 0.0f;

    return m;
}
