#include <bcmath.h>

//
// vec2
//

vec2_t vec2(float x, float y)
{
    vec2_t result = { x, y };
    return result;
}

vec2_t vec2_from_array(float *v)
{
    vec2_t result = { v[0], v[1] };
    return result;
}

bool vec2_is_zero(vec2_t v0)
{
    return fabsf(v0.v[0]) < FLT_EPSILON && fabsf(v0.v[1]) < FLT_EPSILON;
}

bool vec2_is_equal(vec2_t v0, vec2_t v1)
{
    return fabsf(v0.v[0] - v1.v[0]) < FLT_EPSILON && fabsf(v0.v[1] - v1.v[1]) < FLT_EPSILON;
}

vec2_t vec2_zero()
{
    vec2_t result = { 0, 0 };
    return result;
}
vec2_t vec2_one()
{
    vec2_t result = { 1, 1 };
    return result;
}

// vec2_t vec2_sign(vec2_t v0);

vec2_t vec2_add(vec2_t v0, vec2_t v1)
{
    vec2_t result = { v0.x + v1.x, v0.y + v1.y };
    return result;
}

vec2_t vec2_add_f(vec2_t v0, float f)
{
    vec2_t result = { v0.x + f, v0.y + f };
    return result;
}

vec2_t vec2_subtract(vec2_t v0, vec2_t v1)
{
    vec2_t result = { v0.x - v1.x, v0.y - v1.y };
    return result;
}

vec2_t vec2_subtract_f(vec2_t v0, float f)
{
    vec2_t result = { v0.x - f, v0.y - f };
    return result;
}

vec2_t vec2_multiply(vec2_t v0, vec2_t v1)
{
    vec2_t result = { v0.x * v1.x, v0.y * v1.y };
    return result;
}

vec2_t vec2_multiply_f(vec2_t v0, float f)
{
    vec2_t result = { v0.x * f, v0.y * f };
    return result;
}

// vec2_t vec2_multiply_mat2(vec2_t v0, vec2_t m0);

vec2_t vec2_divide(vec2_t v0, vec2_t v1)
{
    vec2_t result = { v0.x / v1.x, v0.y / v1.y };
    return result;
}

vec2_t vec2_divide_f(vec2_t v0, float f)
{
    vec2_t result = { v0.x / f, v0.y / f };
    return result;
}

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

vec2_t vec2_normalize(vec2_t v0)
{
    float l = sqrtf(v0.v[0] * v0.v[0] + v0.v[1] * v0.v[1]);
    vec2_t result = {
        v0.v[0] / l,
        v0.v[1] / l,
    };
    return result;
}

// float vec2_dot(vec2_t v0, vec2_t v1);
// vec2_t vec2_project(vec2_t v0, vec2_t v1);
// vec2_t vec2_slide(vec2_t v0, vec2_t normal);
// vec2_t vec2_reflect(vec2_t v0, vec2_t normal);
// vec2_t vec2_tangent(vec2_t v0);
// vec2_t vec2_rotate(vec2_t v0, float f);
// vec2_t vec2_lerp(vec2_t v0, vec2_t v1, float f);
// vec2_t vec2_bezier3(vec2_t v0, vec2_t v1, vec2_t v2, float f);
// vec2_t vec2_bezier4(vec2_t v0, vec2_t v1, vec2_t v2, vec2_t v3, float f);

float vec2_angle(vec2_t v0)
{
    return atan2f(v0.v[1], v0.v[0]);
}

float vec2_length(vec2_t v0)
{
    return sqrtf(v0.v[0] * v0.v[0] + v0.v[1] * v0.v[1]);
}

float vec2_length_squared(vec2_t v0)
{
    return v0.v[0] * v0.v[0] + v0.v[1] * v0.v[1];
}

float vec2_distance(vec2_t v0, vec2_t v1)
{
    return sqrtf((v0.v[0] - v1.v[0]) * (v0.v[0] - v1.v[0]) + (v0.v[1] - v1.v[1]) * (v0.v[1] - v1.v[1]));
}

float vec2_distance_squared(vec2_t v0, vec2_t v1)
{
    return (v0.v[0] - v1.v[0]) * (v0.v[0] - v1.v[0]) + (v0.v[1] - v1.v[1]) * (v0.v[1] - v1.v[1]);
}

//
// vec3
//

vec3_t vec3(float x, float y, float z)
{
    vec3_t result = { x, y, z };
    return result;
}

vec3_t vec3_from_array(float *v)
{
    vec3_t result = { v[0], v[1], v[2] };
    return result;
}

vec3_t vec3_from_mat4(mat4_t m)
{
    vec3_t result = { m.v[12], m.v[13], m.v[14] };
    return result;
}

vec3_t vec3_zero()
{
    vec3_t result = { 0, 0, 0 };
    return result;
}

vec3_t vec3_one()
{
    vec3_t result = { 1, 1, 1 };
    return result;
}

vec3_t vec3_sign(vec3_t v)
{
    vec3_t result = {
        signf(v.x),
        signf(v.y),
        signf(v.z),
    };
    return result;
}

vec3_t vec3_add(vec3_t v1, vec3_t v2)
{
    vec3_t result = {
        v1.x + v2.x,
        v1.y + v2.y,
        v1.z + v2.z,
    };
    return result;
}

vec3_t vec3_add_f(vec3_t v1, float f)
{
    vec3_t result = {
        v1.x + f,
        v1.y + f,
        v1.z + f,
    };
    return result;
}

vec3_t vec3_subtract(vec3_t v1, vec3_t v2)
{
    vec3_t result = {
        v1.x - v2.x,
        v1.y - v2.y,
        v1.z - v2.z,
    };
    return result;
}

vec3_t vec3_subtract_f(vec3_t v1, float f)
{
    vec3_t result = {
        v1.x - f,
        v1.y - f,
        v1.z - f,
    };
    return result;
}

vec3_t vec3_multiply(vec3_t v1, vec3_t v2)
{
    vec3_t result = {
        v1.x * v2.x,
        v1.y * v2.y,
        v1.z * v2.z,
    };
    return result;
}

vec3_t vec3_multiply_f(vec3_t v1, float f)
{
    vec3_t result = {
        v1.x * f,
        v1.y * f,
        v1.z * f,
    };
    return result;
}

vec3_t vec3_multiply_mat3(vec3_t v, mat3_t m)
{
    vec3_t result = {
        m.v[0] * v.x + m.v[3] * v.y + m.v[6] * v.z,
        m.v[1] * v.x + m.v[4] * v.y + m.v[7] * v.z,
        m.v[2] * v.x + m.v[5] * v.y + m.v[8] * v.z,
    };
    return result;
}

vec3_t vec3_divide(vec3_t v1, vec3_t v2)
{
    vec3_t result = {
        v1.x / v2.x,
        v1.y / v2.y,
        v1.z / v2.z,
    };
    return result;
}

vec3_t vec3_divide_f(vec3_t v1, float f)
{
    vec3_t result = {
        v1.x / f,
        v1.y / f,
        v1.z / f,
    };
    return result;
}

vec3_t vec3_snap(vec3_t v1, vec3_t v2)
{
    vec3_t result = {
        floorf(v1.x / v2.x) * v2.x,
        floorf(v1.y / v2.y) * v2.y,
        floorf(v1.z / v2.z) * v2.z,
    };
    return result;
}

vec3_t vec3_snap_f(vec3_t v1, float f)
{
    vec3_t result = {
        floorf(v1.x / f) * f,
        floorf(v1.y / f) * f,
        floorf(v1.z / f) * f,
    };
    return result;
}

vec3_t vec3_negative(vec3_t v1)
{
    vec3_t result = {
        -v1.x,
        -v1.y,
        -v1.z,
    };
    return result;
}

vec3_t vec3_abs(vec3_t v1)
{
    vec3_t result = {
        fabsf(v1.x),
        fabsf(v1.y),
        fabsf(v1.z),
    };
    return result;
}

vec3_t vec3_floor(vec3_t v1)
{
    vec3_t result = {
        floorf(v1.x),
        floorf(v1.y),
        floorf(v1.z),
    };
    return result;
}

vec3_t vec3_ceil(vec3_t v1)
{
    vec3_t result = {
        ceilf(v1.x),
        ceilf(v1.y),
        ceilf(v1.z),
    };
    return result;
}

vec3_t vec3_round(vec3_t v1)
{
    vec3_t result = {
        roundf(v1.x),
        roundf(v1.y),
        roundf(v1.z),
    };
    return result;
}

vec3_t vec3_max(vec3_t v1, vec3_t v2)
{
    vec3_t result = {
        fmaxf(v1.x, v2.x),
        fmaxf(v1.y, v2.y),
        fmaxf(v1.z, v2.z),
    };
    return result;
}

vec3_t vec3_min(vec3_t v1, vec3_t v2)
{
    vec3_t result = {
        fminf(v1.x, v2.x),
        fminf(v1.y, v2.y),
        fminf(v1.z, v2.z),
    };
    return result;
}

vec3_t vec3_clamp(vec3_t v1, vec3_t v_min, vec3_t v_max)
{
    return vec3_min(v_min, vec3_max(v_max, v1));
}

vec3_t vec3_cross(vec3_t v1, vec3_t v2)
{
    vec3_t result = {
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x,
    };
    return result;
}

vec3_t vec3_normalize(vec3_t v)
{
    float l = vec3_length(v);
    vec3_t result = {
        v.x / l,
        v.y / l,
        v.z / l,
    };
    return result;
}

float vec3_dot(vec3_t v1, vec3_t v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// vec3_t vec3_project(vec3_t result, vec3_t v0, vec3_t v1)
// {
//     float d = vec2_dot(v1, v1);
//     float s = vec2_dot(v0, v1) / d;
//     result.v[0] = v1[0] * s;
//     result.v[1] = v1[1] * s;
//     result.v[2] = v1[2] * s;
//     return result;
// }

// vec3_t vec3_slide(vec3_t result, vec3_t v0, vec3_t normal)
// {
//     float d = vec3_dot(v0, normal);
//     result.v[0] = v0[0] - normal[0] * d;
//     result.v[1] = v0[1] - normal[1] * d;
//     result.v[2] = v0[2] - normal[2] * d;
//     return result;
// }

// vec3_t vec3_reflect(vec3_t result, vec3_t v0, vec3_t normal)
// {
//     float d = 2.0f * vec3_dot(v0, normal);
//     result.v[0] = normal[0] * d - v0[0];
//     result.v[1] = normal[1] * d - v0[1];
//     result.v[2] = normal[2] * d - v0[2];
//     return result;
// }

vec3_t vec3_lerp(vec3_t v0, vec3_t v1, float f)
{
    vec3_t result = {
        v0.v[0] + (v1.v[0] - v0.v[0]) * f,
        v0.v[1] + (v1.v[1] - v0.v[1]) * f,
        v0.v[2] + (v1.v[2] - v0.v[2]) * f,
    };
    return result;
}

// vec3_t vec3_bezier3(vec3_t result, vec3_t v0, vec3_t v1, vec3_t v2, float f)
// {
//     float tmp0[VEC3_SIZE];
//     float tmp1[VEC3_SIZE];
//     vec3_lerp(tmp0, v0, v1, f);
//     vec3_lerp(tmp1, v1, v2, f);
//     vec3_lerp(result, tmp0, tmp1, f);
//     return result;
// }

// vec3_t vec3_bezier4(vec3_t result, vec3_t v0, vec3_t v1, vec3_t v2, vec3_t v3, float f)
// {
//     float tmp0[VEC3_SIZE];
//     float tmp1[VEC3_SIZE];
//     float tmp2[VEC3_SIZE];
//     float tmp3[VEC3_SIZE];
//     float tmp4[VEC3_SIZE];
//     vec3_lerp(tmp0, v0, v1, f);
//     vec3_lerp(tmp1, v1, v2, f);
//     vec3_lerp(tmp2, v2, v3, f);
//     vec3_lerp(tmp3, tmp0, tmp1, f);
//     vec3_lerp(tmp4, tmp1, tmp2, f);
//     vec3_lerp(result, tmp3, tmp4, f);
//     return result;
// }

float vec3_length(vec3_t v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

float vec3_length_squared(vec3_t v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

float vec3_distance(vec3_t v1, vec3_t v2)
{
    vec3_t d = vec3_subtract(v1, v2);
    return vec3_length(d);
}

float vec3_distance_squared(vec3_t v1, vec3_t v2)
{
    vec3_t d = vec3_subtract(v1, v2);
    return vec3_length_squared(d);
}

//
// vec4
//

vec4_t vec4(float x, float y, float z, float w)
{
    vec4_t result = { x, y, z, w };
    return result;
}

vec4_t vec4_from_vec3(vec3_t v, float w)
{
    vec4_t result = { v.x, v.y, v.z, w };
    return result;
}

vec4_t vec4_multiply_mat4(mat4_t m, vec4_t v)
{
    vec4_t result = {
        m.v[0] * v.x + m.v[4] * v.y + m.v[8] * v.z + m.v[12] * v.w,
        m.v[1] * v.x + m.v[5] * v.y + m.v[9] * v.z + m.v[13] * v.w,
        m.v[2] * v.x + m.v[6] * v.y + m.v[10] *v.z + m.v[14] * v.w,
        m.v[3] * v.x + m.v[7] * v.y + m.v[11] *v.z + m.v[15] * v.w,
    };
    return result;
}

vec4_t vec4_divide(vec4_t v0, vec4_t v1)
{
    vec4_t result = {
        v0.x / v1.x,
        v0.y / v1.y,
        v0.z / v1.z,
        v0.w / v1.w,
    };
    return result;
}

vec4_t vec4_divide_f(vec4_t v0, float f)
{
    vec4_t result = {
        v0.x / f,
        v0.y / f,
        v0.z / f,
        v0.w / f,
    };
    return result;
}

//
// mat4
//

mat4_t mat4_from_array(float *v)
{
    mat4_t result;
    for (int i = 0; i < 16; i++)
        result.v[i] = v[i];
    return result;
}

mat4_t mat4_identity()
{
    mat4_t result = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };
    return result;
}

mat4_t mat4_perspective(float fov_y, float aspect, float n, float f)
{
    float tan_half_fov_y = 1.0f / tanf(fov_y * 0.5f);
    mat4_t result = {
        aspect * tan_half_fov_y,
        0,
        0,
        0,
        0,
        tan_half_fov_y,
        0,
        0,
        0,
        0,
        f / (n - f),
        -1,
        0,
        0,
        -(f * n) / (f - n),
        0,
    };
    return result;
}

mat4_t mat4_ortho(float l, float r, float b, float t, float n, float f)
{
    mat4_t result = {
        2 / (r - l),
        0,
        0,
        0,
        0,
        2 / (t - b),
        0,
        0,
        0,
        0,
        -2 / (f - n),
        0,
        -((r + l) / (r - l)),
        -((t + b) / (t - b)),
        -((f + n) / (f - n)),
        1,
    };
    return result;
}

mat4_t mat4_translation(float x, float y, float z)
{
    mat4_t result = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1,
    };
    return result;
}

mat4_t mat4_rotation_x(float rad)
{
    float c = cosf(rad);
    float s = sinf(rad);
    mat4_t result = {
        1, 0, 0, 0,
        0, c, s, 0,
        0,-s, c, 0,
        0, 0, 0, 1,
    };
    return result;
}

mat4_t mat4_rotation_y(float rad)
{
    float c = cosf(rad);
    float s = sinf(rad);
    mat4_t result = {
        c, 0,-s, 0,
        0, 1, 0, 0,
        s, 0, c, 0,
        0, 0, 0, 1,
    };
    return result;
}

mat4_t mat4_rotation_z(float rad)
{
    float c = cosf(rad);
    float s = sinf(rad);
    mat4_t result = {
        c, s, 0, 0,
       -s, c, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };
    return result;
}

mat4_t mat4_rotation_axis(float rad, float x, float y, float z)
{
    float c = cosf(rad);
    float s = sinf(rad);
    float one_c = 1.0f - c;
    float xx = x * x;
    float xy = x * y;
    float xz = x * z;
    float yy = y * y;
    float yz = y * z;
    float zz = z * z;
    float l = xx + yy + zz;
    float sqrt_l = sqrtf(l);
    mat4_t result = {
        (xx + (yy + zz) * c) / l,
        (xy * one_c + z * sqrt_l * s) / l,
        (xz * one_c - y * sqrt_l * s) / l,
        0,
        (xy * one_c - z * sqrt_l * s) / l,
        (yy + (xx + zz) * c) / l,
        (yz * one_c + x * sqrt_l * s) / l,
        0,
        (xz * one_c + y * sqrt_l * s) / l,
        (yz * one_c - x * sqrt_l * s) / l,
        (zz + (xx + yy) * c) / l,
        0,
        0,
        0,
        0,
        1,
    };
    return result;
}

mat4_t mat4_rotation_quat(quat_t q0)
{
    // q0 = quat_normalize(q0);
    float xx = q0.v[0] * q0.v[0];
    float yy = q0.v[1] * q0.v[1];
    float zz = q0.v[2] * q0.v[2];
    float xy = q0.v[0] * q0.v[1];
    float zw = q0.v[2] * q0.v[3];
    float xz = q0.v[0] * q0.v[2];
    float yw = q0.v[1] * q0.v[3];
    float yz = q0.v[1] * q0.v[2];
    float xw = q0.v[0] * q0.v[3];
    mat4_t result = {
        1.0f - 2.0f * (yy + zz),
        2.0f * (xy - zw),
        2.0f * (xz + yw),
        0.0f,
        2.0f * (xy + zw),
        1.0f - 2.0f * (xx + zz),
        2.0f * (yz - xw),
        0.0f,
        2.0f * (xz - yw),
        2.0f * (yz + xw),
        1.0f - 2.0f * (xx + yy),
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        1.0f,
    };
    return result;
}

mat4_t mat4_scaling(float x, float y, float z)
{
    mat4_t result = {
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1,
    };
    return result;
}

mat4_t mat4_multiply(mat4_t m1, mat4_t m2)
{
    mat4_t result = {
        m1.v[0] * m2.v[0] + m1.v[4] * m2.v[1] + m1.v[8] * m2.v[2] + m1.v[12] * m2.v[3],
        m1.v[1] * m2.v[0] + m1.v[5] * m2.v[1] + m1.v[9] * m2.v[2] + m1.v[13] * m2.v[3],
        m1.v[2] * m2.v[0] + m1.v[6] * m2.v[1] + m1.v[10] * m2.v[2] + m1.v[14] * m2.v[3],
        m1.v[3] * m2.v[0] + m1.v[7] * m2.v[1] + m1.v[11] * m2.v[2] + m1.v[15] * m2.v[3],
        m1.v[0] * m2.v[4] + m1.v[4] * m2.v[5] + m1.v[8] * m2.v[6] + m1.v[12] * m2.v[7],
        m1.v[1] * m2.v[4] + m1.v[5] * m2.v[5] + m1.v[9] * m2.v[6] + m1.v[13] * m2.v[7],
        m1.v[2] * m2.v[4] + m1.v[6] * m2.v[5] + m1.v[10] * m2.v[6] + m1.v[14] * m2.v[7],
        m1.v[3] * m2.v[4] + m1.v[7] * m2.v[5] + m1.v[11] * m2.v[6] + m1.v[15] * m2.v[7],
        m1.v[0] * m2.v[8] + m1.v[4] * m2.v[9] + m1.v[8] * m2.v[10] + m1.v[12] * m2.v[11],
        m1.v[1] * m2.v[8] + m1.v[5] * m2.v[9] + m1.v[9] * m2.v[10] + m1.v[13] * m2.v[11],
        m1.v[2] * m2.v[8] + m1.v[6] * m2.v[9] + m1.v[10] * m2.v[10] + m1.v[14] * m2.v[11],
        m1.v[3] * m2.v[8] + m1.v[7] * m2.v[9] + m1.v[11] * m2.v[10] + m1.v[15] * m2.v[11],
        m1.v[0] * m2.v[12] + m1.v[4] * m2.v[13] + m1.v[8] * m2.v[14] + m1.v[12] * m2.v[15],
        m1.v[1] * m2.v[12] + m1.v[5] * m2.v[13] + m1.v[9] * m2.v[14] + m1.v[13] * m2.v[15],
        m1.v[2] * m2.v[12] + m1.v[6] * m2.v[13] + m1.v[10] * m2.v[14] + m1.v[14] * m2.v[15],
        m1.v[3] * m2.v[12] + m1.v[7] * m2.v[13] + m1.v[11] * m2.v[14] + m1.v[15] * m2.v[15],
    };
    return result;
}

mat4_t mat4_translate(mat4_t m1, float x, float y, float z)
{
    mat4_t m2 = mat4_translation(x, y, z);
    return mat4_multiply(m1, m2);
}

mat4_t mat4_rotate_x(mat4_t m1, float rad)
{
    mat4_t m2 = mat4_rotation_x(rad);
    return mat4_multiply(m1, m2);
}

mat4_t mat4_rotate_y(mat4_t m1, float rad)
{
    mat4_t m2 = mat4_rotation_y(rad);
    return mat4_multiply(m1, m2);
}

mat4_t mat4_rotate_z(mat4_t m1, float rad)
{
    mat4_t m2 = mat4_rotation_z(rad);
    return mat4_multiply(m1, m2);
}

mat4_t mat4_rotate_axis(mat4_t m1, float rad, float x, float y, float z)
{
    mat4_t m2 = mat4_rotation_axis(rad, x, y, z);
    return mat4_multiply(m1, m2);
}

mat4_t mat4_rotate_quat(mat4_t m1, quat_t q)
{
    mat4_t m2 = mat4_rotation_quat(q);
    return mat4_multiply(m1, m2);
}

mat4_t mat4_scale(mat4_t m1, float x, float y, float z)
{
    mat4_t m2 = mat4_scaling(x, y, z);
    return mat4_multiply(m1, m2);
}

mat4_t mat4_transpose(mat4_t m)
{
    mat4_t result = {
        m.v[0], m.v[4], m.v[8], m.v[12],
        m.v[1], m.v[5], m.v[9], m.v[13],
        m.v[2], m.v[6], m.v[10],m.v[14],
        m.v[3], m.v[7], m.v[11],m.v[15],
    };
    return result;
}

mat4_t mat4_inverse(mat4_t m)
{
    float a = m.m00 * m.m11 - m.m01 * m.m10;
    float b = m.m00 * m.m12 - m.m02 * m.m10;
    float c = m.m00 * m.m13 - m.m03 * m.m10;
    float d = m.m01 * m.m12 - m.m02 * m.m11;
    float e = m.m01 * m.m13 - m.m03 * m.m11;
    float f = m.m02 * m.m13 - m.m03 * m.m12;
    float g = m.m20 * m.m31 - m.m21 * m.m30;
    float h = m.m20 * m.m32 - m.m22 * m.m30;
    float i = m.m20 * m.m33 - m.m23 * m.m30;
    float j = m.m21 * m.m32 - m.m22 * m.m31;
    float k = m.m21 * m.m33 - m.m23 * m.m31;
    float l = m.m22 * m.m33 - m.m23 * m.m32;
    float det = 1.0f / (a * l - b * k + c * j + d * i - e * h + f * g);
    mat4_t result = {
        ( m.m11 * l - m.m12 * k + m.m13 * j) * det,
        (-m.m01 * l + m.m02 * k - m.m03 * j) * det,
        ( m.m31 * f - m.m32 * e + m.m33 * d) * det,
        (-m.m21 * f + m.m22 * e - m.m23 * d) * det,
        (-m.m10 * l + m.m12 * i - m.m13 * h) * det,
        ( m.m00 * l - m.m02 * i + m.m03 * h) * det,
        (-m.m30 * f + m.m32 * c - m.m33 * b) * det,
        ( m.m20 * f - m.m22 * c + m.m23 * b) * det,
        ( m.m10 * k - m.m11 * i + m.m13 * g) * det,
        (-m.m00 * k + m.m01 * i - m.m03 * g) * det,
        ( m.m30 * e - m.m31 * c + m.m33 * a) * det,
        (-m.m20 * e + m.m21 * c - m.m23 * a) * det,
        (-m.m10 * j + m.m11 * h - m.m12 * g) * det,
        ( m.m00 * j - m.m01 * h + m.m02 * g) * det,
        (-m.m30 * d + m.m31 * b - m.m32 * a) * det,
        ( m.m20 * d - m.m21 * b + m.m22 * a) * det,
    };
    return result;
}

vec4_t mat4_project(mat4_t m, float x, float y, float z, int viewport[4])
{
    // multiply
    vec4_t result = {
        m.m00 * x + m.m01 * y + m.m02 * z + m.m03,
        m.m10 * x + m.m11 * y + m.m12 * z + m.m13,
        m.m20 * x + m.m21 * y + m.m22 * z + m.m23,
        m.m30 * x + m.m31 * y + m.m32 * z + m.m33,
    };
    result = vec4_divide_f(result, result.w);
    result.x = (result.x * 0.5f + 0.5f) * viewport[2] + viewport[0];
    result.y = (result.y * 0.5f + 0.5f) * viewport[3] + viewport[1];
    result.z = (1.0f + result.z) * 0.5f;
    return result;
}

vec4_t mat4_unproject(mat4_t m, float x, float y, float z, int viewport[4])
{
    return mat4_unproject_inv(mat4_inverse(m), x, y, z, viewport);
}

vec4_t mat4_unproject_inv(mat4_t m, float x, float y, float z, int viewport[4])
{
    float ndcX = (x - viewport[0]) / viewport[2] * 2.0f - 1.0f;
    float ndcY = (y - viewport[1]) / viewport[3] * 2.0f - 1.0f;
    float ndcZ = z * 2 - 1.0f;
    // transpose multiply
    vec4_t result = {
        m.m00 * ndcX + m.m10 * ndcY + m.m20 * ndcZ + m.m30,
        m.m01 * ndcX + m.m11 * ndcY + m.m21 * ndcZ + m.m31,
        m.m02 * ndcX + m.m12 * ndcY + m.m22 * ndcZ + m.m32,
        m.m03 * ndcX + m.m13 * ndcY + m.m23 * ndcZ + m.m33,
    };
    result = vec4_divide_f(result, result.w);
    return result;
}

#if 1
void mat4_dump(mat4_t m)
{}
#else
#include <stdio.h>
void mat4_dump(mat4_t m)
{
    printf("[");
    for (int i = 0; i < 16; i++)
    {
        if (i % 4 == 0)
            printf("\n  ");
        printf("%.2f ", m.v[i]);
    }
    printf("\n]\n");
}
#endif

//
// quat
//

// bool quat_is_zero(quat_t q0);
// bool quat_is_equal(quat_t q0, quat_t q1);

quat_t quat(float x, float y, float z, float w)
{
    quat_t result = { x, y, z, w };
    return result;
}

quat_t quat_from_array(float *v)
{
    quat_t result = { v[0], v[1], v[2], v[3] };
    return result;
}

quat_t quat_zero()
{
    quat_t result = { 0, 0, 0, 0 };
    return result;
}

quat_t quat_unit()
{
    quat_t result = { 0, 0, 0, 1 };
    return result;
}

quat_t quat_multiply(quat_t q0, quat_t q1)
{
    quat_t result = {
        q0.v[3] * q1.v[0] + q0.v[0] * q1.v[3] + q0.v[1] * q1.v[2] - q0.v[2] * q1.v[1],
        q0.v[3] * q1.v[1] + q0.v[1] * q1.v[3] + q0.v[2] * q1.v[0] - q0.v[0] * q1.v[2],
        q0.v[3] * q1.v[2] + q0.v[2] * q1.v[3] + q0.v[0] * q1.v[1] - q0.v[1] * q1.v[0],
        q0.v[3] * q1.v[3] - q0.v[0] * q1.v[0] - q0.v[1] * q1.v[1] - q0.v[2] * q1.v[2],
    };
    return result;
}

quat_t quat_multiply_f(quat_t q0, float f)
{
    quat_t result = {
        q0.v[0] * f,
        q0.v[1] * f,
        q0.v[2] * f,
        q0.v[3] * f,
    };
    return result;
}

quat_t quat_divide(quat_t q0, quat_t q1)
{
    float x = q0.v[0];
    float y = q0.v[1];
    float z = q0.v[2];
    float w = q0.v[3];
    float ls = q1.v[0] * q1.v[0] + q1.v[1] * q1.v[1] + q1.v[2] * q1.v[2] + q1.v[3] * q1.v[3];
    float normalized_x = -q1.v[0] / ls;
    float normalized_y = -q1.v[1] / ls;
    float normalized_z = -q1.v[2] / ls;
    float normalized_w = q1.v[3] / ls;
    quat_t result = {
        x * normalized_w + normalized_x * w + (y * normalized_z - z * normalized_y),
        y * normalized_w + normalized_y * w + (z * normalized_x - x * normalized_z),
        z * normalized_w + normalized_z * w + (x * normalized_y - y * normalized_x),
        w * normalized_w - (x * normalized_x + y * normalized_y + z * normalized_z),
    };
    return result;
}

quat_t quat_divide_f(quat_t q0, float f)
{
    quat_t result = {
        q0.v[0] / f,
        q0.v[1] / f,
        q0.v[2] / f,
        q0.v[3] / f,
    };
    return result;
}

quat_t quat_negative(quat_t q0)
{
    quat_t result = {
        -q0.v[0],
        -q0.v[1],
        -q0.v[2],
        -q0.v[3],
    };
    return result;
}

quat_t quat_conjugate(quat_t q0)
{
    quat_t result = {
        -q0.v[0],
        -q0.v[1],
        -q0.v[2],
        q0.v[3],
    };
    return result;
}

quat_t quat_inverse(quat_t q0)
{
    float l = 1.0f / (q0.v[0] * q0.v[0] + q0.v[1] * q0.v[1] + q0.v[2] * q0.v[2] + q0.v[3] * q0.v[3]);
    quat_t result = {
        -q0.v[0] * l,
        -q0.v[1] * l,
        -q0.v[2] * l,
        q0.v[3] * l,
    };
    return result;
}

quat_t quat_normalize(quat_t q0)
{
    float l = 1.0f / sqrtf(q0.v[0] * q0.v[0] + q0.v[1] * q0.v[1] + q0.v[2] * q0.v[2] + q0.v[3] * q0.v[3]);
    quat_t result = {
        q0.v[0] * l,
        q0.v[1] * l,
        q0.v[2] * l,
        q0.v[3] * l,
    };
    return result;
}

float quat_dot(quat_t q0, quat_t q1)
{
    return q0.v[0] * q1.v[0] + q0.v[1] * q1.v[1] + q0.v[2] * q1.v[2] + q0.v[3] * q1.v[3];
}

// quat_t quat_power(quat_t q0, float exponent);

quat_t quat_from_axis_angle(vec3_t axis, float angle)
{
    float half = angle * 0.5f;
    float s = sinf(half);
    quat_t result = {
        axis.v[0] * s,
        axis.v[1] * s,
        axis.v[2] * s,
        cosf(half),
    };
    return result;
}

// quat_t quat_from_vec3(float *v0, float *v1);

quat_t quat_from_mat4(mat4_t m0)
{
    float scale = m0.v[0] + m0.v[5] + m0.v[10];
    quat_t result;
    if (scale > 0.0f) {
        float sr = sqrtf(scale + 1.0f);
        result.v[3] = sr * 0.5f;
        sr = 0.5f / sr;
        result.v[0] = (m0.v[9] - m0.v[6]) * sr;
        result.v[1] = (m0.v[2] - m0.v[8]) * sr;
        result.v[2] = (m0.v[4] - m0.v[1]) * sr;
    } else if ((m0.v[0] >= m0.v[5]) && (m0.v[0] >= m0.v[10])) {
        float sr = sqrtf(1.0f + m0.v[0] - m0.v[5] - m0.v[10]);
        float half = 0.5f / sr;
        result.v[0] = 0.5f * sr;
        result.v[1] = (m0.v[4] + m0.v[1]) * half;
        result.v[2] = (m0.v[8] + m0.v[2]) * half;
        result.v[3] = (m0.v[9] - m0.v[6]) * half;
    } else if (m0.v[5] > m0.v[10]) {
        float sr = sqrtf(1.0f + m0.v[5] - m0.v[0] - m0.v[10]);
        float half = 0.5f / sr;
        result.v[0] = (m0.v[1] + m0.v[4]) * half;
        result.v[1] = 0.5f * sr;
        result.v[2] = (m0.v[6] + m0.v[9]) * half;
        result.v[3] = (m0.v[2] - m0.v[8]) * half;
    } else {
        float sr = sqrtf(1.0f + m0.v[10] - m0.v[0] - m0.v[5]);
        float half = 0.5f / sr;
        result.v[0] = (m0.v[2] + m0.v[8]) * half;
        result.v[1] = (m0.v[6] + m0.v[9]) * half;
        result.v[2] = 0.5f * sr;
        result.v[3] = (m0.v[4] - m0.v[1]) * half;
    }
    return result;
}

quat_t quat_lerp(quat_t q0, quat_t q1, float f)
{
    quat_t result = {
        q0.v[0] + (q1.v[0] - q0.v[0]) * f,
        q0.v[1] + (q1.v[1] - q0.v[1]) * f,
        q0.v[2] + (q1.v[2] - q0.v[2]) * f,
        q0.v[3] + (q1.v[3] - q0.v[3]) * f,
    };
    return result;
}

quat_t quat_slerp(quat_t q0, quat_t q1, float f)
{
    quat_t tmp1;
    float d = quat_dot(q0, q1);
    float f0;
    float f1;
    tmp1 = q1;
    if (d < 0.0f) {
        tmp1 = quat_negative(tmp1);
        d = -d;
    }
    if (d > 0.9995f) {
        f0 = 1.0f - f;
        f1 = f;
    } else {
        float theta = acosf(d);
        float sin_theta = sinf(theta);
        f0 = sinf((1.0f - f) * theta) / sin_theta;
        f1 = sinf(f * theta) / sin_theta;
    }
    quat_t result = {
        q0.v[0] * f0 + tmp1.v[0] * f1,
        q0.v[1] * f0 + tmp1.v[1] * f1,
        q0.v[2] * f0 + tmp1.v[2] * f1,
        q0.v[3] * f0 + tmp1.v[3] * f1,
    };
    return result;
}

float quat_length(quat_t q0)
{
    return sqrtf(q0.v[0] * q0.v[0] + q0.v[1] * q0.v[1] + q0.v[2] * q0.v[2] + q0.v[3] * q0.v[3]);
}

float quat_length_squared(quat_t q0)
{
    return q0.v[0] * q0.v[0] + q0.v[1] * q0.v[1] + q0.v[2] * q0.v[2] + q0.v[3] * q0.v[3];
}

float quat_angle(quat_t q0, quat_t q1)
{
    float s = sqrtf(quat_length_squared(q0) * quat_length_squared(q1));
    s = 1.0f / s;
    return acosf(quat_dot(q0, q1) * s);
}

//
// mat4_stack
//

#include <stdlib.h>

mat4_stack_t mat4_stack_init(int size)
{
    mat4_stack_t ms = malloc(sizeof(struct mat4_stack));
    ms->array = malloc(sizeof(mat4_t) * size);
    ms->size = size;
    ms->current = 0;
    ms->array[0] = mat4_identity();
    return ms;
}

void mat4_stack_free(mat4_stack_t ms)
{
    free(ms->array);
    free(ms);
}

bool mat4_stack_push(mat4_stack_t ms)
{
    if (ms->current == ms->size - 1)
        return false;
    ms->array[ms->current + 1] = ms->array[ms->current];
    ms->current++;
    return true;
}

bool mat4_stack_pop(mat4_stack_t ms)
{
    if (ms->current == 0)
        return false;
    ms->current--;
    return true;
}

void mat4_stack_set(mat4_stack_t ms, mat4_t m)
{
    ms->array[ms->current] = m;
}

mat4_t mat4_stack_get(mat4_stack_t ms)
{
    return ms->array[ms->current];
}

float * mat4_stack_getp(mat4_stack_t ms)
{
    return ms->array[ms->current].v;
}

void mat4_stack_identity(mat4_stack_t ms)
{
    ms->array[ms->current] = mat4_identity();
}

void mat4_stack_translate(mat4_stack_t ms, float x, float y, float z)
{
    ms->array[ms->current] = mat4_translate(ms->array[ms->current], x, y, z);
}

void mat4_stack_rotate_x(mat4_stack_t ms, float rad)
{
    ms->array[ms->current] = mat4_rotate_x(ms->array[ms->current], rad);
}

void mat4_stack_rotate_y(mat4_stack_t ms, float rad)
{
    ms->array[ms->current] = mat4_rotate_y(ms->array[ms->current], rad);
}

void mat4_stack_rotate_z(mat4_stack_t ms, float rad)
{
    ms->array[ms->current] = mat4_rotate_z(ms->array[ms->current], rad);
}

void mat4_stack_rotate_axis(mat4_stack_t ms, float rad, float x, float y, float z)
{
    ms->array[ms->current] = mat4_rotate_axis(ms->array[ms->current], rad, x, y, z);
}

void mat4_stack_scale(mat4_stack_t ms, float x, float y, float z)
{
    ms->array[ms->current] = mat4_scale(ms->array[ms->current], x, y, z);
}

void mat4_stack_multiply(mat4_stack_t ms, mat4_t m)
{
    ms->array[ms->current] = mat4_multiply(ms->array[ms->current], m);
}
