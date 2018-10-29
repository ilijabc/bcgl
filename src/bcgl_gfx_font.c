#include "bcgl_internal.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>

#define BAKE_BITMAP_WIDTH   512
#define BAKE_BITMAP_HEIGHT  512
#define BAKE_CHAR_FIRST     32
#define BAKE_CHAR_COUNT     96

struct font_data_bm
{
    int cols;
    int rows;
    int char_width;
    int char_height;
};

static bool getFontQuad(BCFont *font, char ch, float *px, float *py, stbtt_aligned_quad *pq)
{
    if (ch < font->char_first || ch >= font->char_first + font->char_count)
        return false;
    if (font->type == FONT_TYPE_TRUETYPE)
    {
        stbtt_GetBakedQuad(font->cdata,
                           BAKE_BITMAP_WIDTH, BAKE_BITMAP_HEIGHT,
                           ch - font->char_first, px, py, pq, 1); // 1=opengl & d3d10+, 0=d3d9
    }
    else if (font->type == FONT_TYPE_ANGELCODE)
    {
    }
    else if (font->type == FONT_TYPE_BITMAP)
    {
        struct font_data_bm *bm = (struct font_data_bm *) font->cdata;
        pq->x0 = *px;
        pq->y0 = *py;
        pq->x1 = pq->x0 + bm->char_width;
        pq->y1 = pq->y0 + bm->char_height;
        pq->s0 = (ch % bm->cols) / (float) (bm->cols);
        pq->t0 = (ch / bm->cols) / (float) (bm->cols);
        pq->s1 = pq->s0 + 1.0f / bm->cols;
        pq->t1 = pq->t0 + 1.0f / bm->cols;
        *px += bm->char_width;
        if (ch == '\n')
            *py += bm->char_height;
    }
    return true;
}

BCFont * bcCreateFontTTF(const char *filename, float height)
{
    unsigned char *ttf_buffer = bcLoadDataFile(filename, NULL);
    if (ttf_buffer == NULL)
        return NULL;
    BCFont *font = NEW_OBJECT(BCFont);
    font->type = FONT_TYPE_TRUETYPE;
    font->char_first = BAKE_CHAR_FIRST;
    font->char_count = BAKE_CHAR_COUNT;
    font->cdata = NEW_ARRAY(BAKE_CHAR_COUNT, stbtt_bakedchar);
    BCImage *image = bcCreateImage(BAKE_BITMAP_WIDTH, BAKE_BITMAP_HEIGHT, 1);
    stbtt_BakeFontBitmap(ttf_buffer, 0, height, image->data,
                         BAKE_BITMAP_WIDTH, BAKE_BITMAP_HEIGHT,
                         BAKE_CHAR_FIRST, BAKE_CHAR_COUNT, (stbtt_bakedchar *) font->cdata); // no guarantee this fits!
    font->texture = bcCreateTextureFromImage(image, 0);
    free(ttf_buffer);
    bcDestroyImage(image);
    return font;
}

BCFont * bcCreateFontFNT(const char *filename)
{
    return NULL;
}

BCFont * bcCreateFontBMP(const char *filename, int char_first, int char_count, int cols)
{
    BCImage *image = bcCreateImageFromFile(filename);
    if (image == NULL)
        return NULL;
    struct font_data_bm *bm = NEW_OBJECT(struct font_data_bm);
    bm->cols = cols;
    bm->rows = char_count / cols;
    bm->char_width = image->width / cols;
    bm->char_height = image->height / bm->rows;
    BCFont *font = NEW_OBJECT(BCFont);
    font->type = FONT_TYPE_BITMAP;
    font->char_first = char_first;
    font->char_count = char_count;
    font->cdata = bm;
    font->texture = bcCreateTextureFromImage(image, 0);
    bcDestroyImage(image);
    return font;
}

void bcDestroyFont(BCFont *font)
{
    if (font == NULL)
        return;
    bcDestroyTexture(font->texture);
    free(font->cdata);
    free(font);
}

void bcDrawText(BCFont *font, float x, float y, const char *text)
{
    if (font == NULL || text == NULL)
        return;
    bcBindTexture(font->texture);
    bcBegin(BC_TRIANGLES);
    while (*text)
    {
        stbtt_aligned_quad q;
        if (getFontQuad(font, *text, &x, &y, &q))
        {
            bcTexCoord2f(q.s0, q.t0);
            bcVertex2f(q.x0, q.y0);
            bcTexCoord2f(q.s1, q.t0);
            bcVertex2f(q.x1, q.y0);
            bcTexCoord2f(q.s1, q.t1);
            bcVertex2f(q.x1, q.y1);
            bcTexCoord2f(q.s1, q.t1);
            bcVertex2f(q.x1, q.y1);
            bcTexCoord2f(q.s0, q.t1);
            bcVertex2f(q.x0, q.y1);
            bcTexCoord2f(q.s0, q.t0);
            bcVertex2f(q.x0, q.y0);
        }
        ++text;
    }
    bcEnd();
    bcBindTexture(NULL);
}

void bcGetTextSize(BCFont *font, const char *text, float *px, float *py)
{
    if (font == NULL || text == NULL)
        return;
    float x = 0;
    float y = 0;
    float my = 0;
    while (*text)
    {
        stbtt_aligned_quad q;
        if (getFontQuad(font, *text, &x, &y, &q))
        {
            float qy = -q.y0 + q.y1;
            if (my < qy)
                my = qy;
        }
        ++text;
    }
    y += my;
    if (px)
        *px = x;
    if (py)
        *py = y;
}
