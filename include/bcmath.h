#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdbool.h>
#include <float.h>

typedef struct vec2 {
    union {
        struct {
            float x;
            float y;
        };
        float v[2];
    };
} vec2_t;

typedef struct vec2i {
    union {
        struct {
            int x;
            int y;
        };
        int v[2];
    };
} vec2i_t;

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

typedef struct vec3i {
    union {
        struct {
            int x;
            int y;
            int z;
        };
        int v[3];
    };
} vec3i_t;

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
            float m00, m10, m20, m30;
            float m01, m11, m21, m31;
            float m02, m12, m22, m32;
            float m03, m13, m23, m33;
        };
        float v[16];
    };
} mat4_t;

typedef struct mat4_stack {
    mat4_t *array;
    int size;
    int current;
} * mat4_stack_t;

#define to_radians(degrees) ((degrees) * M_PI / 180.0f)
#define to_degrees(radians) ((radians) * 180.0f / M_PI)
#define signf(f) ((f < 0) ? -1 : ((f > 0) ? 1 : 0))
#define randomf() ((float) rand() / (float) RAND_MAX)
#define randomi(X) (rand() % (X))
#define clampf(x,l,h) (x < l ? l : (x > h ? h : x))

#ifndef M_PI
#define M_PI 3.1415926536f
#endif

#ifndef M_PI_2
#define M_PI_2 1.5707963268f
#endif

#ifndef M_PI_4
#define M_PI_4 0.7853981634f
#endif

// vec2
vec2_t vec2(float x, float y);
vec2_t vec2_from_array(float *v);
bool vec2_is_zero(vec2_t v0);
bool vec2_is_equal(vec2_t v0, vec2_t v1);
vec2_t vec2_zero();
vec2_t vec2_one();
// vec2_t vec2_sign(vec2_t v0);
vec2_t vec2_add(vec2_t v0, vec2_t v1);
vec2_t vec2_add_f(vec2_t v0, float f);
vec2_t vec2_subtract(vec2_t v0, vec2_t v1);
vec2_t vec2_subtract_f(vec2_t v0, float f);
vec2_t vec2_multiply(vec2_t v0, vec2_t v1);
vec2_t vec2_multiply_f(vec2_t v0, float f);
// vec2_t vec2_multiply_mat2(vec2_t v0, vec2_t m0);
vec2_t vec2_multiply_mat3(vec2_t v0, float z, mat3_t m0);
// vec2_t vec2_divide(vec2_t v0, vec2_t v1);
// vec2_t vec2_divide_f(vec2_t v0, float f);
// vec2_t vec2_snap(vec2_t v0, vec2_t v1);
// vec2_t vec2_snap_f(vec2_t v0, float f);
// vec2_t vec2_negative(vec2_t v0);
// vec2_t vec2_abs(vec2_t v0);
// vec2_t vec2_floor(vec2_t v0);
// vec2_t vec2_ceil(vec2_t v0);
// vec2_t vec2_round(vec2_t v0);
// vec2_t vec2_max(vec2_t v0, vec2_t v1);
// vec2_t vec2_min(vec2_t v0, vec2_t v1);
// vec2_t vec2_clamp(vec2_t v0, vec2_t v1, vec2_t v2);
vec2_t vec2_normalize(vec2_t v0);
// float vec2_dot(vec2_t v0, vec2_t v1);
// vec2_t vec2_project(vec2_t v0, vec2_t v1);
// vec2_t vec2_slide(vec2_t v0, vec2_t normal);
// vec2_t vec2_reflect(vec2_t v0, vec2_t normal);
// vec2_t vec2_tangent(vec2_t v0);
// vec2_t vec2_rotate(vec2_t v0, float f);
// vec2_t vec2_lerp(vec2_t v0, vec2_t v1, float f);
// vec2_t vec2_bezier3(vec2_t v0, vec2_t v1, vec2_t v2, float f);
// vec2_t vec2_bezier4(vec2_t v0, vec2_t v1, vec2_t v2, vec2_t v3, float f);
float vec2_angle(vec2_t v0);
float vec2_length(vec2_t v0);
float vec2_length_squared(vec2_t v0);
float vec2_distance(vec2_t v0, vec2_t v1);
float vec2_distance_squared(vec2_t v0, vec2_t v1);

// vec2i
vec2i_t vec2i(int x, int y);

// vec3
vec3_t vec3(float x, float y, float z);
vec3_t vec3_from_array(float *v);
vec3_t vec3_from_mat4(mat4_t m);
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
vec3_t vec3_multiply_mat4(vec3_t v, float w, mat4_t m);
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
vec3_t vec3_lerp(vec3_t v0, vec3_t v1, float f);
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
vec4_t vec4_divide(vec4_t v0, vec4_t v1);
vec4_t vec4_divide_f(vec4_t v0, float f);

// mat3
mat3_t mat3(float m00, float m10, float m20,
            float m01, float m11, float m21,
            float m02, float m12, float m22);
mat3_t mat3_from_array(float *v);
mat3_t mat3_identity();
mat3_t mat3_translation(float x, float y);
mat3_t mat3_rotation(float rad);
mat3_t mat3_scaling(float x, float y);
mat3_t mat3_multiply(mat3_t m1, mat3_t m2);
mat3_t mat3_translate(mat3_t m1, float x, float y);
mat3_t mat3_rotate(mat3_t m1, float rad);
mat3_t mat3_scale(mat3_t m1, float x, float y);
mat3_t mat3_transpose(mat3_t m);

// mat4
mat4_t mat4(float m00, float m10, float m20, float m30,
            float m01, float m11, float m21, float m31,
            float m02, float m12, float m22, float m32,
            float m03, float m13, float m23, float m33);
mat4_t mat4_from_array(float *v);
mat4_t mat4_identity();
mat4_t mat4_perspective(float fov_y, float aspect, float near, float far);
mat4_t mat4_ortho(float left, float right, float bottom, float top, float near, float far);
mat4_t mat4_translation(float x, float y, float z);
mat4_t mat4_rotation_x(float rad);
mat4_t mat4_rotation_y(float rad);
mat4_t mat4_rotation_z(float rad);
mat4_t mat4_rotation_axis(float rad, float x, float y, float z);
mat4_t mat4_rotation_quat(quat_t q0);
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
mat4_t mat4_inverse(mat4_t m);
vec4_t mat4_project(mat4_t m, float x, float y, float z, int viewport[4]);
vec4_t mat4_unproject(mat4_t m, float x, float y, float z, int viewport[4]);
vec4_t mat4_unproject_inv(mat4_t m, float x, float y, float z, int viewport[4]);
float mat4_determinant(mat4_t m);
void mat4_dump(mat4_t m);
bool mat4_is_zero(mat4_t m);

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

// mat4_stack
mat4_stack_t mat4_stack_init(int size);
void mat4_stack_free(mat4_stack_t ms);
bool mat4_stack_push(mat4_stack_t ms);
bool mat4_stack_pop(mat4_stack_t ms);
void mat4_stack_set(mat4_stack_t ms, mat4_t m);
mat4_t mat4_stack_get(mat4_stack_t ms);
float * mat4_stack_getp(mat4_stack_t ms);
void mat4_stack_identity(mat4_stack_t ms);
void mat4_stack_translate(mat4_stack_t ms, float x, float y, float z);
void mat4_stack_rotate_x(mat4_stack_t ms, float rad);
void mat4_stack_rotate_y(mat4_stack_t ms, float rad);
void mat4_stack_rotate_z(mat4_stack_t ms, float rad);
void mat4_stack_rotate_axis(mat4_stack_t ms, float rad, float x, float y, float z);
void mat4_stack_scale(mat4_stack_t ms, float x, float y, float z);
void mat4_stack_multiply(mat4_stack_t ms, mat4_t m);

#ifdef __cplusplus
}
#endif
