#pragma once

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

typedef struct mat3 {
    union {
        struct {
            float m11;
            float m21;
            float m31;
            float m12;
            float m22;
            float m32;
            float m13;
            float m23;
            float m33;
        };
        float v[9];
    };
} mat3_t;

typedef struct mat4 {
    union {
        struct {
            float m11;
            float m21;
            float m31;
            float m41;
            float m12;
            float m22;
            float m32;
            float m42;
            float m13;
            float m23;
            float m33;
            float m43;
            float m14;
            float m24;
            float m34;
            float m44;
        };
        float v[16];
    };
} mat4_t;

// vec2
vec2_t vec2(float x, float y);

// vec3
vec3_t vec3(float x, float y, float z);

// vec4
vec4_t vec4(float x, float y, float z, float w);

// mat3
mat3_t mat3_identity();

// mat4
mat4_t mat4_identity();
