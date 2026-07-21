#include "CgePix.h"
#include <math.h>

#define MIN(a,b)        (((a)<(b))?(a):(b))
#define MAX(a,b)        (((a)>(b))?(a):(b))

/* Conversion between RGB <-> HSL/HSV has error of around ~ 0.009% */
#define DEGREE_MULT     1.8204444e02f

void CgeColorRGBA8(const CgeColor *color, uint8_t *r, uint8_t *g, uint8_t *b,
                   uint8_t *a) {
    CgeColor tmp;

    CgeColorToRGBA(color, &tmp);
    *r = tmp.data.rgba.r >> 8;
    *g = tmp.data.rgba.g >> 8;
    *b = tmp.data.rgba.b >> 8;
    *a = tmp.data.rgba.a >> 8;
}

void CgeColorRGBA16(const CgeColor *color, uint16_t *r, uint16_t *g,
                    uint16_t *b, uint16_t *a) {
    CgeColor tmp;

    CgeColorToRGBA(color, &tmp);
    *r = tmp.data.rgba.r;
    *g = tmp.data.rgba.g;
    *b = tmp.data.rgba.b;
    *a = tmp.data.rgba.a;
}

void CgeColorRGBAf(const CgeColor *color, float *r, float *g, float *b,
                   float *a) {
    CgeColor tmp;

    CgeColorToRGBA(color, &tmp);
    *r = tmp.data.rgba.r / 65535.0f;
    *g = tmp.data.rgba.g / 65535.0f;
    *b = tmp.data.rgba.b / 65535.0f;
    *a = tmp.data.rgba.a / 65535.0f;
}

void CgeColorSetRGBA8(CgeColor *color, uint8_t r, uint8_t g, uint8_t b,
                      uint8_t a) {
    color->type = CGE_COLOR_RGBA;
    color->data.rgba.r = (uint16_t)r * 0x0101;
    color->data.rgba.g = (uint16_t)g * 0x0101;
    color->data.rgba.b = (uint16_t)b * 0x0101;
    color->data.rgba.a = (uint16_t)a * 0x0101;
}

void CgeColorSetRGBA16(CgeColor *color, uint16_t r, uint16_t g, uint16_t b,
                       uint16_t a) {
    color->type = CGE_COLOR_RGBA;
    color->data.rgba.r = r;
    color->data.rgba.g = g;
    color->data.rgba.b = b;
    color->data.rgba.a = a;
}

void CgeColorSetRGBAf(CgeColor *color, float r, float g, float b, float a) {
    r = MAX(MIN(1.0f, r), 0.0f);
    g = MAX(MIN(1.0f, g), 0.0f);
    b = MAX(MIN(1.0f, b), 0.0f);
    a = MAX(MIN(1.0f, a), 0.0f);

    color->type = CGE_COLOR_RGBA;
    color->data.rgba.r = r * 65535.0f;
    color->data.rgba.g = g * 65535.0f;
    color->data.rgba.b = b * 65535.0f;
    color->data.rgba.a = a * 65535.0f;
}

void CgeColorHSVAf(const CgeColor *color, float *h, float *s, float *v,
                   float *a) {
    CgeColor tmp;

    CgeColorToHSVA(color, &tmp);
    *h = tmp.data.hsva.h / DEGREE_MULT;
    *s = tmp.data.hsva.s / 65535.0f;
    *v = tmp.data.hsva.v / 65535.0f;
    *a = tmp.data.hsva.a / 65535.0f;
}

void CgeColorSetHSVAf(CgeColor *color, float h, float s, float v, float a) {
    while (h >= 360.0f)
        h -= 360.0f;
    while (h < 0.0f)
        h += 360.0f;

    s = MAX(MIN(1.0f, s), 0.0f);
    v = MAX(MIN(1.0f, v), 0.0f);

    color->type = CGE_COLOR_HSVA;
    color->data.hsva.h = h * DEGREE_MULT;
    color->data.hsva.s = s * 65535.0f;
    color->data.hsva.v = v * 65535.0f;
    color->data.hsva.a = a * 65535.0f;
}

void CgeColorHSLAf(const CgeColor *color, float *h, float *s, float *l,
                   float *a) {
    CgeColor tmp;

    CgeColorToHSLA(color, &tmp);
    *h = tmp.data.hsla.h / DEGREE_MULT;
    *s = tmp.data.hsla.s / 65535.0f;
    *l = tmp.data.hsla.l / 65535.0f;
    *a = tmp.data.hsla.a / 65535.0f;
}

void CgeColorSetHSLAf(CgeColor *color, float h, float s, float l, float a) {
    while (h >= 360.0f)
        h -= 360.0f;
    while (h < 0.0f)
        h += 360.0f;

    s = MAX(MIN(1.0f, s), 0.0f);
    l = MAX(MIN(1.0f, l), 0.0f);

    color->type = CGE_COLOR_HSLA;
    color->data.hsla.h = h * DEGREE_MULT;
    color->data.hsla.s = s * 65535;
    color->data.hsla.l = l * 65535;
    color->data.hsla.a = a * 65535;
}

/*
 * Transformations between color models are based on:
 * Computer Graphics and Geometric Modeling by Max K. Agoston
 */
static void rgbToHsv(const CgeColor *color, CgeColor *out) {
    float min, max, d, r, g, b, a, h, s, v;

    CgeColorRGBAf(color, &r, &g, &b, &a);
    max = MAX(MAX(r, g), b);
    min = MIN(MIN(r, g), b);

    v = max;
    s = 0.0f;
    h = 0.0f;
    d = max - min;

    if (max)
        s = d / max;

    if (s) {
        if (r == max)
            h = (g - b) / d;
        else if (g == max)
            h = 2 + (b - r) / d;
        else
            h = 4 + (r - g) / d;
    }

    CgeColorSetHSVAf(out, h * 60.0f, s, v, a);
}

static void hsvToRgb(const CgeColor *color, CgeColor *out) {
    float h, s, v, a, r, g, b, fract, p, q, t;
    int sextant;
    CgeColorHSVAf(color, &h, &s, &v, &a);

    r = v;
    g = v;
    b = v;

    if (s) {
        h /= 60.0f;
        sextant = (int)h;
        fract = h - sextant;

        p = v * (1 - s);
        q = v * (1 - (s * fract));
        t = v * (1 - (s * (1 - fract)));

        switch (sextant) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
        }
    }

    CgeColorSetRGBAf(out, r, g, b, a);
}

static void rgbToHsl(const CgeColor *color, CgeColor *out) {
    float min, max, d, r, g, b, a, h, s, l;

    CgeColorRGBAf(color, &r, &g, &b, &a);
    max = MAX(MAX(r, g), b);
    min = MIN(MIN(r, g), b);

    l = (max + min) / 2.0;
    s = 0.0f;
    h = 0.0f;
    d =  max - min;

    if (max != min) {
        if (l <= 0.5f)
            s = d / (max + min);
        else
            s = d / (2.0f - max - min);

        if (r == max)
            h = (g - b) / d;
        else if (g == max)
            h = 2 + (b - r) / d;
        else
            h = 4 + (r - g) / d;
    }

    CgeColorSetHSLAf(out, h * 60.0f, s, l, a);
}

static void hslToHsv(const CgeColor *color, CgeColor *out) {
    float h, s, l, v, a;

    CgeColorHSLAf(color, &h, &s, &l, &a);
    v = s * MIN(l, 1.0f - l) + l;
    s = (v) ? (2.0f - 2.0f * l / v) : (0.0f);

    CgeColorSetHSVAf(out, h, s, v, a);
}

static void hsvToHsl(const CgeColor *color, CgeColor *out) {
    float h, s, v, l, a, m;

    CgeColorHSVAf(color, &h, &s, &v, &a);
    l = v - v * s / 2.0f;
    m = MIN(l, 1.0f - l);
    s = (m) ? ((v - l) / m) : (0);
    CgeColorSetHSLAf(out, h, s, l, a);
}

void CgeColorToRGBA(const CgeColor *color, CgeColor *out) {
    switch (color->type) {
    default:
        /* fallthrough */
    case CGE_COLOR_RGBA:
        *out = *color;
        break;

    case CGE_COLOR_HSVA:
        hsvToRgb(color, out);
        break;

    case CGE_COLOR_HSLA:
        hslToHsv(color, out);
        hsvToRgb(out, out);
        break;
    }
}

void CgeColorToHSVA(const CgeColor *color, CgeColor *out) {
    switch (color->type) {
    case CGE_COLOR_RGBA:
        rgbToHsv(color, out);
        break;

    default:
        /* fallthrough */
    case CGE_COLOR_HSVA:
        *out = *color;
        break;

    case CGE_COLOR_HSLA:
        hslToHsv(color, out);
        break;
    }
}

void CgeColorToHSLA(const CgeColor *color, CgeColor *out) {
    switch (color->type) {
    case CGE_COLOR_RGBA:
        rgbToHsl(color, out);
        break;

    case CGE_COLOR_HSVA:
        hsvToHsl(color, out);
        break;

    default:
        /* fallthrough */
    case CGE_COLOR_HSLA:
        *out = *color;
        break;
    }
}

static uint16_t mulDiv16(uint16_t x, uint16_t y) {
    uint32_t product = (uint32_t)x * (uint32_t)y + 32767u;
    return (uint16_t)((product + (product >> 16)) >> 16);
}

static uint16_t divScale16(uint16_t x, uint16_t denom) {
    return (uint16_t)(((uint32_t)x * 65535u + denom/2 ) / denom);
}

void CgeColorPremultiply(const CgeColor *color, CgeColor *out) {
    uint16_t r, g, b, a;

    CgeColorRGBA16(color, &r, &g, &b, &a);
    if (a) {
        out->data.channel[0] = mulDiv16(r, a);
        out->data.channel[1] = mulDiv16(g, a);
        out->data.channel[2] = mulDiv16(b, a);
    } else {
        out->data.channel[0] = 0;
        out->data.channel[1] = 0;
        out->data.channel[2] = 0;
    }
    out->data.channel[3] = a;
    out->type = CGE_COLOR_RGBA;
}

void CgeColorUnpremultiply(const CgeColor *color, CgeColor *out) {
    uint16_t r, g, b, a;

    CgeColorRGBA16(color, &r, &g, &b, &a);
    if (a) {
        out->data.channel[0] = divScale16(r, a);
        out->data.channel[1] = divScale16(g, a);
        out->data.channel[2] = divScale16(b, a);
    } else {
        out->data.channel[0] = 0;
        out->data.channel[1] = 0;
        out->data.channel[2] = 0;
    }
    out->data.channel[3] = a;
    out->type = CGE_COLOR_RGBA;
}

static float blendMultiply(float fg, float bg) {
    return fg * bg;
}

static float blendScreen(float fg, float bg) {
    return fg + bg - (fg * bg);
}

static float blendDarken(float fg, float bg) {
    return MIN(fg, bg);
}

static float blendLighten(float fg, float bg) {
    return MAX(fg, bg);
}

static float blendColorDodge(float fg, float bg) {
    if (fg != 1.0f)
        return MIN(1.0f, bg / (1.0f - fg));
    return fg;
}

static float blendColorBurn(float fg, float bg) {
    if (fg != 0.0f)
        return 1.0f - MIN(1.0f, (1.0f - bg) / fg);
    return fg;
}

static float blendLinearDodge(float fg, float bg) {
    return fg + bg;
}

static float blendLinearBurn(float fg, float bg) {
    return fg + bg - 1.0f;
}

static float blendDifference(float fg, float bg) {
    return fabs(bg - fg);
}

static float blendExclusion(float fg, float bg) {
    return fg + bg - (2.0f * fg * bg);
}

static float blendHardLight(float fg, float bg) {
    if (fg <= 0.5f)
        return blendMultiply(2.0f * fg, bg);
    else
        return blendScreen(2.0f * fg - 1.0f, bg);
}

static float blendSoftLight(float fg, float bg) {
    return (1.0f - 2.0f * fg) * bg * bg + 2.0f * fg * bg;
}

static float blendOverlay(float fg, float bg) {
    return blendHardLight(bg, fg);
}

void CgeColorBlendNormal(const CgeColor *fg, const CgeColor *bg,
                         CgeColor *out) {
    /* Unused */
    (void)bg;
    *out = *fg;
}

void CgeColorBlendMultiply(const CgeColor *fg, const CgeColor *bg,
                           CgeColor *out) {
    float bgr, bgg, bgb, bga;
    float fgr, fgg, fgb, fga;

    CgeColorRGBAf(fg, &fgr, &fgg, &fgb, &fga);
    CgeColorRGBAf(bg, &bgr, &bgg, &bgb, &bga);

    fgr = blendMultiply(fgr, bgr);
    fgg = blendMultiply(fgg, bgg);
    fgb = blendMultiply(fgb, bgb);

    CgeColorSetRGBAf(out, fgr, fgg, fgb, fga);
}

void CgeColorBlendScreen(const CgeColor *fg, const CgeColor *bg,
                         CgeColor *out) {
    float bgr, bgg, bgb, bga;
    float fgr, fgg, fgb, fga;

    CgeColorRGBAf(fg, &fgr, &fgg, &fgb, &fga);
    CgeColorRGBAf(bg, &bgr, &bgg, &bgb, &bga);

    fgr = blendScreen(fgr, bgr);
    fgg = blendScreen(fgg, bgg);
    fgb = blendScreen(fgb, bgb);

    CgeColorSetRGBAf(out, fgr, fgg, fgb, fga);
}

void CgeColorBlendOverlay(const CgeColor *fg, const CgeColor *bg,
                          CgeColor *out) {
    float bgr, bgg, bgb, bga;
    float fgr, fgg, fgb, fga;

    CgeColorRGBAf(fg, &fgr, &fgg, &fgb, &fga);
    CgeColorRGBAf(bg, &bgr, &bgg, &bgb, &bga);

    fgr = blendOverlay(fgr, bgr);
    fgg = blendOverlay(fgg, bgg);
    fgb = blendOverlay(fgb, bgb);

    CgeColorSetRGBAf(out, fgr, fgg, fgb, fga);
}

void CgeColorBlendDarken(const CgeColor *fg, const CgeColor *bg,
                         CgeColor *out) {
    float bgr, bgg, bgb, bga;
    float fgr, fgg, fgb, fga;

    CgeColorRGBAf(fg, &fgr, &fgg, &fgb, &fga);
    CgeColorRGBAf(bg, &bgr, &bgg, &bgb, &bga);

    fgr = blendDarken(fgr, bgr);
    fgg = blendDarken(fgg, bgg);
    fgb = blendDarken(fgb, bgb);

    CgeColorSetRGBAf(out, fgr, fgg, fgb, fga);
}

void CgeColorBlendLighten(const CgeColor *fg, const CgeColor *bg,
                          CgeColor *out) {
    float bgr, bgg, bgb, bga;
    float fgr, fgg, fgb, fga;

    CgeColorRGBAf(fg, &fgr, &fgg, &fgb, &fga);
    CgeColorRGBAf(bg, &bgr, &bgg, &bgb, &bga);

    fgr = blendLighten(fgr, bgr);
    fgg = blendLighten(fgg, bgg);
    fgb = blendLighten(fgb, bgb);

    CgeColorSetRGBAf(out, fgr, fgg, fgb, fga);
}

void CgeColorBlendColorDodge(const CgeColor *fg, const CgeColor *bg,
                             CgeColor *out) {
    float bgr, bgg, bgb, bga;
    float fgr, fgg, fgb, fga;

    CgeColorRGBAf(fg, &fgr, &fgg, &fgb, &fga);
    CgeColorRGBAf(bg, &bgr, &bgg, &bgb, &bga);

    fgr = blendColorDodge(fgr, bgr);
    fgg = blendColorDodge(fgg, bgg);
    fgb = blendColorDodge(fgb, bgb);

    CgeColorSetRGBAf(out, fgr, fgg, fgb, fga);
}

void CgeColorBlendColorBurn(const CgeColor *fg, const CgeColor *bg,
                            CgeColor *out) {
    float bgr, bgg, bgb, bga;
    float fgr, fgg, fgb, fga;

    CgeColorRGBAf(fg, &fgr, &fgg, &fgb, &fga);
    CgeColorRGBAf(bg, &bgr, &bgg, &bgb, &bga);

    fgr = blendColorBurn(fgr, bgr);
    fgg = blendColorBurn(fgg, bgg);
    fgb = blendColorBurn(fgb, bgb);

    CgeColorSetRGBAf(out, fgr, fgg, fgb, fga);
}

void CgeColorBlendLinearDodge(const CgeColor *fg, const CgeColor *bg,
                              CgeColor *out) {
    float bgr, bgg, bgb, bga;
    float fgr, fgg, fgb, fga;

    CgeColorRGBAf(fg, &fgr, &fgg, &fgb, &fga);
    CgeColorRGBAf(bg, &bgr, &bgg, &bgb, &bga);

    fgr = blendLinearDodge(fgr, bgr);
    fgg = blendLinearDodge(fgg, bgg);
    fgb = blendLinearDodge(fgb, bgb);

    CgeColorSetRGBAf(out, fgr, fgg, fgb, fga);
}

void CgeColorBlendLinearBurn(const CgeColor *fg, const CgeColor *bg,
                             CgeColor *out) {
    float bgr, bgg, bgb, bga;
    float fgr, fgg, fgb, fga;

    CgeColorRGBAf(fg, &fgr, &fgg, &fgb, &fga);
    CgeColorRGBAf(bg, &bgr, &bgg, &bgb, &bga);

    fgr = blendLinearBurn(fgr, bgr);
    fgg = blendLinearBurn(fgg, bgg);
    fgb = blendLinearBurn(fgb, bgb);

    CgeColorSetRGBAf(out, fgr, fgg, fgb, fga);
}

void CgeColorBlendHardLight(const CgeColor *fg, const CgeColor *bg,
                            CgeColor *out) {
    float bgr, bgg, bgb, bga;
    float fgr, fgg, fgb, fga;

    CgeColorRGBAf(fg, &fgr, &fgg, &fgb, &fga);
    CgeColorRGBAf(bg, &bgr, &bgg, &bgb, &bga);

    fgr = blendHardLight(fgr, bgr);
    fgg = blendHardLight(fgg, bgg);
    fgb = blendHardLight(fgb, bgb);

    CgeColorSetRGBAf(out, fgr, fgg, fgb, fga);
}

void CgeColorBlendSoftLight(const CgeColor *fg, const CgeColor *bg,
                            CgeColor *out) {
    float bgr, bgg, bgb, bga;
    float fgr, fgg, fgb, fga;

    CgeColorRGBAf(fg, &fgr, &fgg, &fgb, &fga);
    CgeColorRGBAf(bg, &bgr, &bgg, &bgb, &bga);

    fgr = blendSoftLight(fgr, bgr);
    fgg = blendSoftLight(fgg, bgg);
    fgb = blendSoftLight(fgb, bgb);

    CgeColorSetRGBAf(out, fgr, fgg, fgb, fga);
}

void CgeColorBlendDifference(const CgeColor *fg, const CgeColor *bg,
                             CgeColor *out) {
    float bgr, bgg, bgb, bga;
    float fgr, fgg, fgb, fga;

    CgeColorRGBAf(fg, &fgr, &fgg, &fgb, &fga);
    CgeColorRGBAf(bg, &bgr, &bgg, &bgb, &bga);

    fgr = blendDifference(fgr, bgr);
    fgg = blendDifference(fgg, bgg);
    fgb = blendDifference(fgb, bgb);

    CgeColorSetRGBAf(out, fgr, fgg, fgb, fga);
}

void CgeColorBlendExclusion(const CgeColor *fg, const CgeColor *bg,
                            CgeColor *out) {
    float bgr, bgg, bgb, bga;
    float fgr, fgg, fgb, fga;

    CgeColorHSLAf(fg, &fgr, &fgg, &fgb, &fga);
    CgeColorHSLAf(bg, &bgr, &bgg, &bgb, &bga);

    fgr = blendExclusion(fgr, bgr);
    fgg = blendExclusion(fgg, bgg);
    fgb = blendExclusion(fgb, bgb);

    CgeColorSetRGBAf(out, fgr, fgg, fgb, fga);
}

void CgeColorBlendHue(const CgeColor *fg, const CgeColor *bg, CgeColor *out) {
    float bgr, bgg, bgb, bga;
    float fgr, fgg, fgb, fga;

    CgeColorHSLAf(fg, &fgr, &fgg, &fgb, &fga);
    CgeColorHSLAf(bg, &bgr, &bgg, &bgb, &bga);
    CgeColorSetHSLAf(out, fgr, bgg, bgb, fga);
}

void CgeColorBlendSaturation(const CgeColor *fg, const CgeColor *bg,
                             CgeColor *out) {
    float bgr, bgg, bgb, bga;
    float fgr, fgg, fgb, fga;

    CgeColorHSLAf(fg, &fgr, &fgg, &fgb, &fga);
    CgeColorHSLAf(bg, &bgr, &bgg, &bgb, &bga);
    CgeColorSetHSLAf(out, bgr, fgg, bgb, fga);
}

void CgeColorBlendColor(const CgeColor *fg, const CgeColor *bg, CgeColor *out) {
    float bgr, bgg, bgb, bga;
    float fgr, fgg, fgb, fga;

    CgeColorHSLAf(fg, &fgr, &fgg, &fgb, &fga);
    CgeColorHSLAf(bg, &bgr, &bgg, &bgb, &bga);
    CgeColorSetHSLAf(out, fgr, fgg, bgb, fga);
}

void CgeColorBlendLuminosity(const CgeColor *fg, const CgeColor *bg,
                             CgeColor *out) {
    float bgr, bgg, bgb, bga;
    float fgr, fgg, fgb, fga;

    CgeColorHSLAf(fg, &fgr, &fgg, &fgb, &fga);
    CgeColorHSLAf(bg, &bgr, &bgg, &bgb, &bga);
    CgeColorSetHSLAf(out, bgr, bgg, fgb, fga);
}
