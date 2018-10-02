#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>

typedef struct vec2 {
    union {
        struct {
            float x;
            float y;
        };
        float v[2];
    };
} vec2_t;

typedef struct vec3 {
    union {
        struct {
            float x;
            float y;
            float z;
        };
        float v[3];
    };
} vec3_t;

typedef struct vec4 {
    union {
        struct {
            float x;
            float y;
            float z;
            float w;
        };
        float v[4];
    };
} vec4_t;

typedef vec4_t quat_t;

typedef struct mat3 {
    union {
        struct {
            float m11, m21, m31;
            float m12, m22, m32;
            float m13, m23, m33;
        };
        float v[9];
    };
} mat3_t;

typedef struct mat4 {
    union {
        struct {
            float m11, m21, m31, m41;
            float m12, m22, m32, m42;
            float m13, m23, m33, m43;
            float m14, m24, m34, m44;
        };
        float v[16];
    };
} mat4_t;

#define to_radians(degrees) (degrees * M_PI / 180.0f)
#define to_degrees(radians) (radians * 180.0f / M_PI)
#define signf(f) ((f < 0) ? -1 : ((f > 0) ? 1 : 0))
#define randomf() ((float) rand() / (float) RAND_MAX)

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// vec2
vec2_t vec2(float x, float y);

// vec3
vec3_t vec3(float x, float y, float z);
vec3_t vec3_from_array(float *v);
// bool vec3_is_zero(vec3_t v0);
// bool vec3_is_equal(vec3_t v0, vec3_t v1);
vec3_t vec3_zero();
vec3_t vec3_one();
vec3_t vec3_sign(vec3_t v0);
vec3_t vec3_add(vec3_t v0, vec3_t v1);
vec3_t vec3_add_f(vec3_t v0, float f);
vec3_t vec3_subtract(vec3_t v0, vec3_t v1);
vec3_t vec3_subtract_f(vec3_t v0, float f);
vec3_t vec3_multiply(vec3_t v0, vec3_t v1);
vec3_t vec3_multiply_f(vec3_t v0, float f);
vec3_t vec3_multiply_mat3(vec3_t v0, mat3_t m0);
vec3_t vec3_divide(vec3_t v0, vec3_t v1);
vec3_t vec3_divide_f(vec3_t v0, float f);
vec3_t vec3_snap(vec3_t v0, vec3_t v1);
vec3_t vec3_snap_f(vec3_t v0, float f);
vec3_t vec3_negative(vec3_t v0);
vec3_t vec3_abs(vec3_t v0);
vec3_t vec3_floor(vec3_t v0);
vec3_t vec3_ceil(vec3_t v0);
vec3_t vec3_round(vec3_t v0);
vec3_t vec3_max(vec3_t v0, vec3_t v1);
vec3_t vec3_min(vec3_t v0, vec3_t v1);
vec3_t vec3_clamp(vec3_t v0, vec3_t v1, vec3_t v2);
vec3_t vec3_cross(vec3_t v0, vec3_t v1);
vec3_t vec3_normalize(vec3_t v0);
float vec3_dot(vec3_t v0, vec3_t v1);
// vec3_t vec3_project(vec3_t v0, vec3_t v1);
// vec3_t vec3_slide(vec3_t v0, vec3_t normal);
// vec3_t vec3_reflect(vec3_t v0, vec3_t normal);
// vec3_t vec3_lerp(vec3_t v0, vec3_t v1, float f);
// vec3_t vec3_bezier3(vec3_t v0, vec3_t v1, vec3_t v2, float f);
// vec3_t vec3_bezier4(vec3_t v0, vec3_t v1, vec3_t v2, vec3_t v3, float f);
float vec3_length(vec3_t v0);
float vec3_length_squared(vec3_t v0);
float vec3_distance(vec3_t v0, vec3_t v1);
float vec3_distance_squared(vec3_t v0, vec3_t v1);

// vec4
vec4_t vec4(float x, float y, float z, float w);
vec4_t vec4_from_vec3(vec3_t v, float w);
vec4_t vec4_multiply_mat4(mat4_t m, vec4_t v);

// mat4
mat4_t mat4_from_array(float *v);
mat4_t mat4_identity();
mat4_t mat4_perspective(float fov_y, float aspect, float near, float far);
mat4_t mat4_ortho(float left, float right, float bottom, float top, float near, float far);
mat4_t mat4_translation(float x, float y, float z);
mat4_t mat4_rotation_x(float rad);
mat4_t mat4_rotation_y(float rad);
mat4_t mat4_rotation_z(float rad);
mat4_t mat4_rotation_axis(float rad, float x, float y, float z);
mat4_t mat4_rotation_quat(quat_t q);
mat4_t mat4_scaling(float x, float y, float z);
mat4_t mat4_multiply(mat4_t m1, mat4_t m2);
mat4_t mat4_translate(mat4_t m1, float x, float y, float z);
mat4_t mat4_rotate_x(mat4_t m1, float rad);
mat4_t mat4_rotate_y(mat4_t m1, float rad);
mat4_t mat4_rotate_z(mat4_t m1, float rad);
mat4_t mat4_rotate_axis(mat4_t m1, float rad, float x, float y, float z);
mat4_t mat4_rotate_quat(mat4_t m1, quat_t q);
mat4_t mat4_scale(mat4_t m1, float x, float y, float z);
mat4_t mat4_transpose(mat4_t m);
void mat4_dump(mat4_t m);

// quat
// bool quat_is_zero(quat_t q0);
// bool quat_is_equal(quat_t q0, quat_t q1);
quat_t quat(float x, float y, float z, float w);
quat_t quat_from_array(float *v);
quat_t quat_zero();
quat_t quat_unit();
quat_t quat_multiply(quat_t q0, quat_t q1);
quat_t quat_multiply_f(quat_t q0, float f);
quat_t quat_divide(quat_t q0, quat_t q1);
quat_t quat_divide_f(quat_t q0, float f);
quat_t quat_negative(quat_t q0);
quat_t quat_conjugate(quat_t q0);
quat_t quat_inverse(quat_t q0);
quat_t quat_normalize(quat_t q0);
float quat_dot(quat_t q0, quat_t q1);
// quat_t quat_power(quat_t q0, float exponent);
quat_t quat_from_axis_angle(vec3_t axis, float angle);
// quat_t quat_from_vec3(float *v0, float *v1);
quat_t quat_from_mat4(mat4_t m0);
quat_t quat_lerp(quat_t q0, quat_t q1, float f);
quat_t quat_slerp(quat_t q0, quat_t q1, float f);
float quat_length(quat_t q0);
float quat_length_squared(quat_t q0);
float quat_angle(quat_t q0, quat_t q1);

#ifdef __cplusplus
}
#endif
