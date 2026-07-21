#include "CgePix.h"
#include <string.h>

enum FormatType {
    TYPE_UNKNOWN,
    TYPE_INDEX,
    TYPE_IARRAY,
    TYPE_FARRAY,
    TYPE_PACKED
};

struct FormatInfo {
    int type;
    size_t bytesPerPixel;
    size_t bitsPerChannel;
    size_t channels;
    uint16_t masks[4];
    uint8_t shifts[8];
};

static struct FormatInfo globalInfo[] = {
    /* CGE_PIX_UNKNOWN */
    { TYPE_UNKNOWN, 0, 0, 0, {}, {}},
    /* CGE_PIX_INDEX1 */
    { TYPE_INDEX, 0, 1, 8, {0x1}, {7, 6, 5, 4, 3, 2, 1, 0,}},
    /* CGE_PIX_INDEX1_LSB */
    { TYPE_INDEX, 0, 1, 8, {0x1}, {0, 1, 2, 3, 4, 5, 6, 7,}},
    /* CGE_PIX_INDEX2 */
    { TYPE_INDEX, 0, 2, 4, {0x3}, {6, 4, 2, 0,}},
    /* CGE_PIX_INDEX2_LSB */
    { TYPE_INDEX, 0, 2, 4, {0x3}, {0, 2, 4, 6,}},
    /* CGE_PIX_INDEX4 */
    { TYPE_INDEX, 0, 4, 2, {0xF}, {4, 0,}},
    /* CGE_PIX_INDEX4_LSB */
    { TYPE_INDEX, 0, 4, 2, {0xF}, {0, 4,}},
    /* CGE_PIX_INDEX8 */
    { TYPE_INDEX, 1, 8, 1, {0xFF}, {0,}},
    /* CGE_PIX_R8 */
    { TYPE_IARRAY, 1, 8, 1, {0xFF}, {0,}},
    /* CGE_PIX_R16 */
    { TYPE_IARRAY, 2, 16, 1, {0xFFFF}, {0,}},
    /* CGE_PIX_R32F */
    { TYPE_FARRAY, 4, 32, 1, {}, {0,}},
    /* CGE_PIX_RGB8 */
    { TYPE_IARRAY, 3, 8, 3, {0xFF, 0xFF, 0xFF}, {0, 1, 2,}},
    /* CGE_PIX_BGR8 */
    { TYPE_IARRAY, 3, 8, 3, {0xFF, 0xFF, 0xFF}, {2, 1, 0,}},
    /* CGE_PIX_RGB16 */
    { TYPE_IARRAY, 6, 16, 3, {0xFFFF, 0xFFFF, 0xFFFF}, {0, 1, 2,}},
    /* CGE_PIX_BGR16 */
    { TYPE_IARRAY, 6, 16, 3, {0xFFFF, 0xFFFF, 0xFFFF}, {2, 1, 0,}},
    /* CGE_PIX_RGB32F */
    { TYPE_FARRAY, 12, 32, 3, {}, {0, 1, 2,}},
    /* CGE_PIX_BGR32F */
    { TYPE_FARRAY, 12, 32, 3, {}, {2, 1, 0,}},
    /* CGE_PIX_RGBA8 */
    { TYPE_IARRAY, 4, 8, 4, {0xFF, 0xFF, 0xFF, 0xFF}, {0, 1, 2, 3,}},
    /* CGE_PIX_BGRA8 */
    { TYPE_IARRAY, 4, 8, 4, {0xFF, 0xFF, 0xFF, 0xFF}, {2, 1, 0, 3,}},
    /* CGE_PIX_ARGB8 */
    { TYPE_IARRAY, 4, 8, 4, {0xFF, 0xFF, 0xFF, 0xFF}, {1, 2, 3, 0,}},
    /* CGE_PIX_ABGR8 */
    { TYPE_IARRAY, 4, 8, 4, {0xFF, 0xFF, 0xFF, 0xFF}, {3, 2, 1, 0,}},
    /* CGE_PIX_RGBA16 */
    { TYPE_IARRAY, 8, 16, 4, {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF}, {0, 1, 2, 3,}},
    /* CGE_PIX_BGRA16 */
    { TYPE_IARRAY, 8, 16, 4, {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF}, {2, 1, 0, 3,}},
    /* CGE_PIX_ARGB16 */
    { TYPE_IARRAY, 8, 16, 4, {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF}, {1, 2, 3, 0,}},
    /* CGE_PIX_ABGR16 */
    { TYPE_IARRAY, 8, 16, 4, {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF}, {3, 2, 1, 0,}},
    /* CGE_PIX_RGBA32F */
    { TYPE_FARRAY, 16, 32, 4, {}, {0, 1, 2, 3,}},
    /* CGE_PIX_BGRA32F */
    { TYPE_FARRAY, 16, 32, 4, {}, {2, 1, 0, 3,}},
    /* CGE_PIX_ARGB32F */
    { TYPE_FARRAY, 16, 32, 4, {}, {1, 2, 3, 0,}},
    /* CGE_PIX_ABGR32F */
    { TYPE_FARRAY, 16, 32, 4, {}, {3, 2, 1, 0,}},
    /* CGE_PIX_RGB565 */
    { TYPE_PACKED, 2, 16, 3, {0x1F, 0x3F, 0x1F,}, {11, 5, 0,}},
    /* CGE_PIX_BGR565 */
    { TYPE_PACKED, 2, 16, 3, {0x1F, 0x3F, 0x1F,}, {0, 5, 11,}},
    /* CGE_PIX_RGBA5551 */
    { TYPE_PACKED, 2, 16, 4, {0x1F, 0x1F, 0x1F, 0x1,}, {11, 6, 1, 0,}},
    /* CGE_PIX_BGRA5551 */
    { TYPE_PACKED, 2, 16, 4, {0x1F, 0x1F, 0x1F, 0x1,}, {1, 6, 11, 0,}},
    /* CGE_PIX_ARGB1555 */
    { TYPE_PACKED, 2, 16, 4, {0x1F, 0x1F, 0x1F, 0x1,}, {10, 5, 0, 15,}},
    /* CGE_PIX_ABGR1555 */
    { TYPE_PACKED, 2, 16, 4, {0x1F, 0x1F, 0x1F, 0x1,}, {0, 5, 10, 15,}},
    /* CGE_PIX_RGBA1010102 */
    { TYPE_PACKED, 4, 32, 4, {0x3FF, 0x3FF, 0x3FF, 0x3}, {22, 12, 2, 0,}},
    /* CGE_PIX_BGRA1010102 */
    { TYPE_PACKED, 4, 32, 4, {0x3FF, 0x3FF, 0x3FF, 0x3}, {2, 12, 22, 0,}},
    /* CGE_PIX_ARGB2101010 */
    { TYPE_PACKED, 4, 32, 4, {0x3FF, 0x3FF, 0x3FF, 0x3}, {20, 10, 0, 30,}},
    /* CGE_PIX_ABGR2101010 */
    { TYPE_PACKED, 4, 32, 4, {0x3FF, 0x3FF, 0x3FF, 0x3}, {0, 10, 20, 30,}},
};

static uint16_t normilize(uint16_t mask, uint16_t value) {
    return ((uint32_t)(value & mask) * 65535) / mask;
}

static uint16_t denormilize(uint16_t mask, uint16_t value) {
    return (((uint32_t)(value * mask)) / 65535) & mask;
}

static void fixColor(CgeColor *color, const struct FormatInfo *format, int flags) {
    if (format->type == TYPE_INDEX)
        return;

    if (flags & CGE_PIX_NOALPHA)
        color->data.channel[3] = 65535;

    if (format->type != TYPE_INDEX) {
        switch (format->channels) {
        case 3:
            color->data.channel[3] = 65535;
            break;

        case 1:
            color->data.channel[3] = color->data.channel[0];
            color->data.channel[2] = color->data.channel[0];
            color->data.channel[1] = color->data.channel[0];
            break;
        }
    }
}

static size_t bestPaletteIndex(CgeColor *color, const CgeColor *palette,
                               const struct FormatInfo *format) {
    uint32_t bestError = -1;
    size_t i, bestIndex = 0, maxIndex;
    CgeColor tmpColor;

    maxIndex = (1 << format->bitsPerChannel);
    for (i = 0; i < maxIndex; ++i) {
        uint32_t currentError;
        int32_t delta[4];

        switch (palette[i].type) {
        case CGE_COLOR_HSLA: CgeColorToHSLA(color, &tmpColor); break;
        case CGE_COLOR_HSVA: CgeColorToHSVA(color, &tmpColor); break;
        case CGE_COLOR_RGBA: CgeColorToRGBA(color, &tmpColor); break;
        }

        delta[0] = (int32_t)tmpColor.data.channel[0] - palette[i].data.channel[0];
        delta[1] = (int32_t)tmpColor.data.channel[1] - palette[i].data.channel[1];
        delta[2] = (int32_t)tmpColor.data.channel[2] - palette[i].data.channel[2];
        delta[3] = (int32_t)tmpColor.data.channel[3] - palette[i].data.channel[3];

        currentError = delta[0] * delta[0] + delta[1] * delta[1] +
                       delta[2] * delta[2] + delta[3] * delta[3];

        if (currentError < bestError) {
            bestError = currentError;
            bestIndex = i;
        }
    }

    return bestIndex;
}

static void readPacked(const struct FormatInfo *format, void *data,
                       CgeColor *value) {
    uint32_t raw;

    switch (format->bitsPerChannel) {
    default: return;
    case 16: raw = *(uint16_t *)data; break;
    case 32: raw = *(uint32_t *)data; break;
    }

    switch (format->channels) {
    case 4: value->data.channel[3] = normilize(format->masks[3], raw >> format->shifts[3]);
    /* fallthrough */
    case 3: value->data.channel[2] = normilize(format->masks[2], raw >> format->shifts[2]);
    /* fallthrough */
    case 2: value->data.channel[1] = normilize(format->masks[1], raw >> format->shifts[1]);
    /* fallthrough */
    case 1: value->data.channel[0] = normilize(format->masks[0], raw >> format->shifts[0]);
    /* fallthrough */
    }
}

static void writePacked(const struct FormatInfo *format, void *data,
                        const CgeColor *value) {
    uint32_t raw = 0;

    switch (format->channels) {
    case 4: raw |= (uint32_t)denormilize(format->masks[3], value->data.channel[3]) << format->shifts[3];
    /* fallthrough */
    case 3: raw |= (uint32_t)denormilize(format->masks[2], value->data.channel[2]) << format->shifts[2];
    /* fallthrough */
    case 2: raw |= (uint32_t)denormilize(format->masks[1], value->data.channel[1]) << format->shifts[1];
    /* fallthrough */
    case 1: raw |= (uint32_t)denormilize(format->masks[0], value->data.channel[0]) << format->shifts[0];
    /* fallthrough */
    }

    switch (format->bitsPerChannel) {
    case 16: *(uint16_t *)data = raw; break;
    case 32: *(uint32_t *)data = raw; break;
    }
}

static void readFloatArray32(const struct FormatInfo *format, float *data,
                             CgeColor *value) {
    float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;

    switch (format->channels) {
    case 4: a = data[format->shifts[3]];
    /* fallthrough */
    case 3: b = data[format->shifts[2]];
    /* fallthrough */
    case 2: g = data[format->shifts[1]];
    /* fallthrough */
    case 1: r = data[format->shifts[0]];
    /* fallthrough */
    }
    CgeColorSetRGBAf(value, r, g, b, a);
}

static void readFloatArray(const struct FormatInfo *format, void *data,
                           CgeColor *value) {
    switch (format->bitsPerChannel) {
    default: return;
    case 32: readFloatArray32(format, (float *)data, value); break;
    }
}

static void writeFloatArray32(const struct FormatInfo *format, float *data,
                              const CgeColor *value) {
    float r, g, b, a;

    CgeColorRGBAf(value, &r, &g, &b, &a);
    switch (format->channels) {
    case 4: data[format->shifts[3]] = a;
    /* fallthrough */
    case 3: data[format->shifts[2]] = b;
    /* fallthrough */
    case 2: data[format->shifts[1]] = g;
    /* fallthrough */
    case 1: data[format->shifts[0]] = r;
    /* fallthrough */
    }
}

static void writeFloatArray(const struct FormatInfo *format, void *data,
                            const CgeColor *value) {
    switch (format->bitsPerChannel) {
    default: return;
    case 32: writeFloatArray32(format, (float *)data, value); break;
    }
}

static void readIntArray8(const struct FormatInfo *format, uint8_t *data,
                          CgeColor *value) {
    switch (format->channels) {
    case 4: value->data.channel[3] = normilize(format->masks[3], data[format->shifts[3]]);
    /* fallthrough */
    case 3: value->data.channel[2] = normilize(format->masks[2], data[format->shifts[2]]);
    /* fallthrough */
    case 2: value->data.channel[1] = normilize(format->masks[1], data[format->shifts[1]]);
    /* fallthrough */
    case 1: value->data.channel[0] = normilize(format->masks[0], data[format->shifts[0]]);
    /* fallthrough */
    }
}

static void readIntArray16(const struct FormatInfo *format, uint16_t *data,
                           CgeColor *value) {
    switch (format->channels) {
    case 4: value->data.channel[3] = normilize(format->masks[3], data[format->shifts[3]]);
    /* fallthrough */
    case 3: value->data.channel[2] = normilize(format->masks[2], data[format->shifts[2]]);
    /* fallthrough */
    case 2: value->data.channel[1] = normilize(format->masks[1], data[format->shifts[1]]);
    /* fallthrough */
    case 1: value->data.channel[0] = normilize(format->masks[0], data[format->shifts[0]]);
    /* fallthrough */
    }
}

static void readIntArray(const struct FormatInfo *format, void *data,
                         CgeColor *value) {
    switch (format->bitsPerChannel) {
    default: return;
    case 8: readIntArray8(format, (uint8_t *)data, value); break;
    case 16: readIntArray16(format, (uint16_t *)data, value); break;
    }
}

static void writeIntArray8(const struct FormatInfo *format, uint8_t *data,
                           const CgeColor *value) {
    switch (format->channels) {
    case 4: data[format->shifts[3]] = denormilize(format->masks[3], value->data.channel[3]);
    /* fallthrough */
    case 3: data[format->shifts[2]] = denormilize(format->masks[2], value->data.channel[2]);
    /* fallthrough */
    case 2: data[format->shifts[1]] = denormilize(format->masks[1], value->data.channel[1]);
    /* fallthrough */
    case 1: data[format->shifts[0]] = denormilize(format->masks[0], value->data.channel[0]);
    /* fallthrough */
    }
}

static void writeIntArray16(const struct FormatInfo *format, uint16_t *data,
                            const CgeColor *value) {
    switch (format->channels) {
    case 4: data[format->shifts[3]] = denormilize(format->masks[3], value->data.channel[3]);
    /* fallthrough */
    case 3: data[format->shifts[2]] = denormilize(format->masks[2], value->data.channel[2]);
    /* fallthrough */
    case 2: data[format->shifts[1]] = denormilize(format->masks[1], value->data.channel[1]);
    /* fallthrough */
    case 1: data[format->shifts[0]] = denormilize(format->masks[0], value->data.channel[0]);
    /* fallthrough */
    }
}

static void writeIntArray(const struct FormatInfo *format, void *data,
                          const CgeColor *value) {
    switch (format->bitsPerChannel) {
    default: return;
    case 8: writeIntArray8(format, (uint8_t *)data, value); break;
    case 16: writeIntArray16(format, (uint16_t *)data, value); break;
    }
}

static uint8_t readIndex(const struct FormatInfo *format, uint8_t *data,
                         uint32_t x) {
    return (*data >> format->shifts[x & (format->channels - 1)]) & format->masks[0];
}

static void writeIndex(const struct FormatInfo *format, uint8_t *data, uint32_t x,
                       uint8_t index) {
    uint8_t mask, value;

    mask = ~(format->masks[0] << format->shifts[x & (format->channels - 1)]);
    value = (index & format->masks[0]) << format->shifts[x & (format->channels - 1)];
    *data = (*data & mask) | value;
}

static int isLittleEndian(void) {
    uint16_t number = 0x0001;
    return (*((uint8_t *)&number) == 0x01);
}

static void readData(const struct FormatInfo *format, void *data, uint32_t x,
                     CgeColor *value, const CgeColor *palette, int flags) {
    switch (format->type) {
    case TYPE_INDEX:
        *value = palette[readIndex(format, data, x)];
        break;

    case TYPE_IARRAY:
        readIntArray(format, data, value);
        value->type = CGE_COLOR_RGBA;
        break;

    case TYPE_FARRAY:
        readFloatArray(format, data, value);
        value->type = CGE_COLOR_RGBA;
        break;

    case TYPE_PACKED:
        readPacked(format, data, value);
        value->type = CGE_COLOR_RGBA;
        break;
    }

    fixColor(value, format, flags);
}

static void writeData(const struct FormatInfo *format, void *data, uint32_t x,
                      const CgeColor *value, const CgeColor *palette) {
    CgeColor tmp;
    CgeColorToRGBA(value, &tmp);

    switch (format->type) {
    case TYPE_INDEX:
        writeIndex(format, data, x, bestPaletteIndex(&tmp, palette, format));
        break;

    case TYPE_IARRAY:
        writeIntArray(format, data, &tmp);
        break;

    case TYPE_FARRAY:
        writeFloatArray(format, data, &tmp);
        break;

    case TYPE_PACKED:
        writePacked(format, data, &tmp);
        break;
    }
}

int CgePixFormatIs32Native(int format) {
    switch (format) {
        case CGE_PIX_BGRA8: return isLittleEndian();
        case CGE_PIX_ARGB8: return !isLittleEndian();
    }
    return 0;
}

size_t CgePixInitLayout(uint32_t width, uint32_t height, int format, int flags,
                        CgePix *out) {
    struct FormatInfo *lookup = globalInfo + format;

    out->width = width;
    out->height = height;
    out->format = format;
    out->flags = flags;
    out->data = NULL;
    out->palette = NULL;

    if (lookup->type == TYPE_INDEX)
        out->stride = (lookup->bitsPerChannel * width + 7) >> 3;
    else
        out->stride = lookup->bytesPerPixel * width;

    if (flags & CGE_PIX_ALIGN32)
        out->stride = (out->stride + 3) & ~(size_t)0x3;

    return CgePixSize(out);
}

void CgePixView(CgePix *pix, uint32_t x, uint32_t y, uint32_t width,
                uint32_t height, CgePix *out) {
    memcpy(out, pix, sizeof(*pix));

    if (x > pix->width)
        x = pix->width;
    if (y > pix->height)
        y = pix->height;

    if (width > pix->width - x)
        width = pix->width - x;
    if (height > pix->height - y)
        height = pix->height - y;

    out->data = CgePixAt(pix, x, y);
    out->width = width;
    out->height = height;
}

size_t CgePixSize(const CgePix *pix) {
    return pix->stride * pix->height;
}

void *CgePixScanline(const CgePix *pix, uint32_t y) {
    return (char *)(pix->data) + y * pix->stride;
}

static void *rowAt(const struct FormatInfo *format, void *data, uint32_t x) {
    if (format->type == TYPE_INDEX)
        return (char *)data + ((x * format->bitsPerChannel) >> 3);
    return (char *)data + x * format->bytesPerPixel;
}

void *CgePixAt(const CgePix *pix, uint32_t x, uint32_t y) {
    struct FormatInfo *lookup = globalInfo + pix->format;
    return rowAt(lookup, CgePixScanline(pix, y), x);
}

void CgePixColor(const CgePix *pix, uint32_t x, uint32_t y, CgeColor *value) {
    struct FormatInfo *lookup = globalInfo + pix->format;
    readData(lookup, CgePixAt(pix, x, y), x, value, pix->palette, pix->flags);
}

void CgePixSetColor(CgePix *pix, uint32_t x, uint32_t y, const CgeColor *value) {
    struct FormatInfo *lookup = globalInfo + pix->format;
    writeData(lookup, CgePixAt(pix, x, y), x, value, pix->palette);
}

uint8_t CgePixIndex(const CgePix *pix, uint32_t x, uint32_t y) {
    struct FormatInfo *lookup = globalInfo + pix->format;
    return readIndex(lookup, CgePixAt(pix, x, y), x);
}

void CgePixSetIndex(CgePix *pix, uint32_t x, uint32_t y, uint8_t index) {
    struct FormatInfo *lookup = globalInfo + pix->format;
    writeIndex(lookup, CgePixAt(pix, x, y), x, index);
}

void CgePixConvertRow(void *src, int srcFormat, int srcFlags,
                      const CgeColor *srcPalette, void *dest, int destFormat,
                      int destFlags, const CgeColor *destPalette, size_t count) {
    const struct FormatInfo *srcInfo, *destInfo;
    size_t x;

    /* Unused */
    (void)destFlags;

    srcInfo = globalInfo + srcFormat;
    destInfo = globalInfo + destFormat;
    for (x = 0; count; --count, ++x) {
        void *srcAt, *destAt;
        CgeColor data;

        srcAt = rowAt(srcInfo, src, x);
        destAt = rowAt(destInfo, dest, x);
        readData(srcInfo, srcAt, x, &data, srcPalette, srcFlags);
        writeData(destInfo, destAt, x, &data, destPalette);
    }
}
