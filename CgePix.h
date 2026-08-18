#ifndef CGE_PIX_H
#define CGE_PIX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

enum CgeColorType {
    CGE_COLOR_RGBA,
    CGE_COLOR_HSVA,
    CGE_COLOR_HSLA
};

enum CgePixFormat {
    CGE_PIX_UNKNOWN,
    CGE_PIX_INDEX1,
    CGE_PIX_INDEX1_LSB,
    CGE_PIX_INDEX2,
    CGE_PIX_INDEX2_LSB,
    CGE_PIX_INDEX4,
    CGE_PIX_INDEX4_LSB,
    CGE_PIX_INDEX8,
    CGE_PIX_R8,
    CGE_PIX_R16,
    CGE_PIX_R32F,
    CGE_PIX_RGB8,
    CGE_PIX_BGR8,
    CGE_PIX_RGB16,
    CGE_PIX_BGR16,
    CGE_PIX_RGB32F,
    CGE_PIX_BGR32F,
    CGE_PIX_RGBA8,
    CGE_PIX_BGRA8,
    CGE_PIX_ARGB8,
    CGE_PIX_ABGR8,
    CGE_PIX_RGBA16,
    CGE_PIX_BGRA16,
    CGE_PIX_ARGB16,
    CGE_PIX_ABGR16,
    CGE_PIX_RGBA32F,
    CGE_PIX_BGRA32F,
    CGE_PIX_ARGB32F,
    CGE_PIX_ABGR32F,
    CGE_PIX_RGB565,
    CGE_PIX_BGR565,
    CGE_PIX_RGBA5551,
    CGE_PIX_BGRA5551,
    CGE_PIX_ARGB1555,
    CGE_PIX_ABGR1555,
    CGE_PIX_RGBA1010102,
    CGE_PIX_BGRA1010102,
    CGE_PIX_ARGB2101010,
    CGE_PIX_ABGR2101010,
};

enum CgePixFlags {
    CGE_PIX_PREMULTIPLIED = (1 << 0),
    CGE_PIX_NOALPHA       = (1 << 1),
    CGE_PIX_ALIGN32       = (1 << 2),
};

typedef struct CgeColor {
    union {
        struct {
            uint16_t r;
            uint16_t g;
            uint16_t b;
            uint16_t a;
        } rgba;
        struct {
            uint16_t h;
            uint16_t s;
            uint16_t v;
            uint16_t a;
        } hsva;
        struct {
            uint16_t h;
            uint16_t s;
            uint16_t l;
            uint16_t a;
        } hsla;
        uint16_t channel[4];
    } data;
    int type;
} CgeColor;

typedef struct CgePix
{
    uint32_t width;
    uint32_t height;
    int format;
    int flags;
    size_t stride;
    void *data;
    CgeColor *palette;
} CgePix;

void CgeColorRGBA8(const CgeColor *color, uint8_t *r, uint8_t *g, uint8_t *b,
                   uint8_t *a);
void CgeColorRGBA16(const CgeColor *color, uint16_t *r, uint16_t *g,
                    uint16_t *b, uint16_t *a);
void CgeColorRGBAf(const CgeColor *color, float *r, float *g, float *b,
                   float *a);
void CgeColorSetRGBA8(CgeColor *color, uint8_t r, uint8_t g, uint8_t b,
                      uint8_t a);
void CgeColorSetRGBA16(CgeColor *color, uint16_t r, uint16_t g, uint16_t b,
                       uint16_t a);
void CgeColorSetRGBAf(CgeColor *color, float r, float g, float b, float a);
void CgeColorHSVAf(const CgeColor *color, float *h, float *s, float *v,
                   float *a);
void CgeColorSetHSVAf(CgeColor *color, float h, float s, float v, float a);
void CgeColorHSLAf(const CgeColor *color, float *h, float *s, float *l,
                   float *a);
void CgeColorSetHSLAf(CgeColor *color, float h, float s, float l, float a);

void CgeColorToRGBA(const CgeColor *color, CgeColor *out);
void CgeColorToHSVA(const CgeColor *color, CgeColor *out);
void CgeColorToHSLA(const CgeColor *color, CgeColor *out);

void CgeColorPremultiply(const CgeColor *color, CgeColor *out);
void CgeColorUnpremultiply(const CgeColor *color, CgeColor *out);

void CgeColorBlendNormal(const CgeColor *fg, const CgeColor *bg,
                         CgeColor *out);
void CgeColorBlendMultiply(const CgeColor *fg, const CgeColor *bg,
                           CgeColor *out);
void CgeColorBlendScreen(const CgeColor *fg, const CgeColor *bg,
                         CgeColor *out);
void CgeColorBlendOverlay(const CgeColor *fg, const CgeColor *bg,
                          CgeColor *out);
void CgeColorBlendDarken(const CgeColor *fg, const CgeColor *bg,
                         CgeColor *out);
void CgeColorBlendLighten(const CgeColor *fg, const CgeColor *bg,
                          CgeColor *out);
void CgeColorBlendColorDodge(const CgeColor *fg, const CgeColor *bg,
                             CgeColor *out);
void CgeColorBlendColorBurn(const CgeColor *fg, const CgeColor *bg,
                            CgeColor *out);
void CgeColorBlendLinearDodge(const CgeColor *fg, const CgeColor *bg,
                              CgeColor *out);
void CgeColorBlendLinearBurn(const CgeColor *fg, const CgeColor *bg,
                             CgeColor *out);
void CgeColorBlendHardLight(const CgeColor *fg, const CgeColor *bg,
                            CgeColor *out);
void CgeColorBlendSoftLight(const CgeColor *fg, const CgeColor *bg,
                            CgeColor *out);
void CgeColorBlendDifference(const CgeColor *fg, const CgeColor *bg,
                             CgeColor *out);
void CgeColorBlendExclusion(const CgeColor *fg, const CgeColor *bg,
                            CgeColor *out);
void CgeColorBlendHue(const CgeColor *fg, const CgeColor *bg, CgeColor *out);
void CgeColorBlendSaturation(const CgeColor *fg, const CgeColor *bg,
                             CgeColor *out);
void CgeColorBlendColor(const CgeColor *fg, const CgeColor *bg, CgeColor *out);
void CgeColorBlendLuminosity(const CgeColor *fg, const CgeColor *bg,
                             CgeColor *out);

int CgePixFormatIs32Native(int format);
size_t CgePixInitLayout(uint32_t width, uint32_t height, int format, int flags,
                        CgePix *out);
void CgePixView(CgePix *pix, uint32_t x, uint32_t y, uint32_t width,
                uint32_t height, CgePix *out);
size_t CgePixSize(const CgePix *pix);
void *CgePixScanline(const CgePix *pix, uint32_t y);
void *CgePixAt(const CgePix *pix, uint32_t x, uint32_t y);
void CgePixColor(const CgePix *pix, uint32_t x, uint32_t y, CgeColor *value);
void CgePixSetColor(CgePix *pix, uint32_t x, uint32_t y, const CgeColor *value);
uint8_t CgePixIndex(const CgePix *pix, uint32_t x, uint32_t y);
void CgePixSetIndex(CgePix *pix, uint32_t x, uint32_t y, uint8_t index);
void CgePixConvertRow(void *src, int srcFormat, int srcFlags,
                      const CgeColor *srcPalette, void *dest, int destFormat,
                      int destFlags, const CgeColor *destPalette, size_t count);

#ifdef __cplusplus
}
#endif

#endif /* CGE_PIX_H */
