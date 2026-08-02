#include "modules/modules.h"
#include <cstring>

#if defined(__aarch64__) || defined(__ARM_NEON) || defined(__ARM_NEON__)
#include <arm_neon.h>
#endif

#define MOD_NAME "unpack"

/**
 * @brief  mipi raw10 to raw16
 * [p1 9:2][p2 9:2][p3 9:2][p4 9:2][(p1 1:0)(p2 1:0)(p3 1:0)(p4 1:0)]
 * @param raw
 * @param unpack_raw16
 * @param width
 * @param height
 */
static void UnpackRaw10ToRaw16Scalar(uint8_t *raw, uint16_t *unpack_raw16, int width, int height)
{
    uint8_t *raw10_packed_in = raw;
    uint16_t *raw16_unpacked_out = unpack_raw16;

    for (int p1 = 0, p2 = 0; p2 < width * height; p1 += 5, p2 += 4)
    {
        raw10_packed_in = raw + p1;
        raw16_unpacked_out = unpack_raw16 + p2;
        raw16_unpacked_out[0] = ((uint16_t)raw10_packed_in[0] << 2) | (((uint16_t)raw10_packed_in[4] >> 6) & 0x03);
        raw16_unpacked_out[1] = ((uint16_t)raw10_packed_in[1] << 2) | (((uint16_t)raw10_packed_in[4] >> 4) & 0x03);
        raw16_unpacked_out[2] = ((uint16_t)raw10_packed_in[2] << 2) | (((uint16_t)raw10_packed_in[4] >> 2) & 0x03);
        raw16_unpacked_out[3] = ((uint16_t)raw10_packed_in[3] << 2) | (((uint16_t)raw10_packed_in[4] >> 0) & 0x03);
    }
}

/**
 * @brief  mipi raw12 to raw16
 * [p1 11:4][p2 11:4][(p1 3:0)(p1 3:0)]
 * @param raw
 * @param unpack_raw16
 * @param width
 * @param height
 */
static void UnpackRaw12ToRaw16Scalar(uint8_t *raw, uint16_t *unpack_raw16, int width, int height)
{
    uint8_t *raw12_packed_in = raw;
    uint16_t *raw16_unpacked_out = unpack_raw16;

    for (int p1 = 0, p2 = 0; p2 < width * height; p1 += 3, p2 += 2)
    {
        raw12_packed_in = raw + p1;
        raw16_unpacked_out = unpack_raw16 + p2;
        raw16_unpacked_out[0] = (raw12_packed_in[0] << 4) | ((raw12_packed_in[2] >> 4) & 0x0f);
        raw16_unpacked_out[1] = (raw12_packed_in[1] << 4) | ((raw12_packed_in[2] >> 0) & 0x0f);
    }
}

/**
 * @brief  mipi raw16 to raw16
 * [p1 15: 8][p1 7 : 0]
 * @param raw
 * @param unpack_raw16
 * @param width
 * @param height
 */
static void UnpackRaw16ToRaw16Scalar(uint8_t *raw, uint16_t *unpack_raw16, int width, int height)
{
    memcpy(unpack_raw16, raw, width * height * 2);
}
#if defined(__aarch64__) || defined(__ARM_NEON) || defined(__ARM_NEON__)
static void UnpackRaw10ToRaw16Neon(uint8_t *raw, uint16_t *unpack_raw16, int width, int height)
{
    const int total_pixels = width * height;
    int pixels_done = 0;

    while (pixels_done + 4 <= total_pixels)
    {
        const uint8_t *src = raw + (pixels_done / 4) * 5;
        uint16_t *dst = unpack_raw16 + pixels_done;

        uint8_t packed_bytes[8] = {0};
        memcpy(packed_bytes, src, 5);

        uint8x8_t packed = vld1_u8(packed_bytes);
        const uint8_t b0 = vget_lane_u8(packed, 0);
        const uint8_t b1 = vget_lane_u8(packed, 1);
        const uint8_t b2 = vget_lane_u8(packed, 2);
        const uint8_t b3 = vget_lane_u8(packed, 3);
        const uint8_t b4 = vget_lane_u8(packed, 4);

        const uint16_t out0 = ((uint16_t)b0 << 2) | ((b4 >> 6) & 0x03);
        const uint16_t out1 = ((uint16_t)b1 << 2) | ((b4 >> 4) & 0x03);
        const uint16_t out2 = ((uint16_t)b2 << 2) | ((b4 >> 2) & 0x03);
        const uint16_t out3 = ((uint16_t)b3 << 2) | ((b4 >> 0) & 0x03);

        uint16x4_t out_vec = {out0, out1, out2, out3};
        vst1_u16(dst, out_vec);

        pixels_done += 4;
    }

    if (pixels_done < total_pixels)
    {
        for (int i = pixels_done; i < total_pixels; ++i)
        {
            const int block = i / 4;
            const int lane = i % 4;
            const uint8_t *src = raw + block * 5;

            uint8_t b4 = src[4];
            uint16_t value = 0;
            switch (lane)
            {
            case 0:
                value = ((uint16_t)src[0] << 2) | ((b4 >> 6) & 0x03);
                break;
            case 1:
                value = ((uint16_t)src[1] << 2) | ((b4 >> 4) & 0x03);
                break;
            case 2:
                value = ((uint16_t)src[2] << 2) | ((b4 >> 2) & 0x03);
                break;
            default:
                value = ((uint16_t)src[3] << 2) | ((b4 >> 0) & 0x03);
                break;
            }

            unpack_raw16[i] = value;
        }
    }
}

static void UnpackRaw12ToRaw16Neon(uint8_t *raw, uint16_t *unpack_raw16, int width, int height)
{
    const int total_pixels = width * height;
    int pixels_done = 0;

    while (pixels_done + 2 <= total_pixels)
    {
        const uint8_t *src = raw + (pixels_done / 2) * 3;
        uint8_t packed_bytes[8] = {0};
        memcpy(packed_bytes, src, 3);

        uint8x8_t packed = vld1_u8(packed_bytes);
        const uint8_t b0 = vget_lane_u8(packed, 0);
        const uint8_t b1 = vget_lane_u8(packed, 1);
        const uint8_t b2 = vget_lane_u8(packed, 2);

        unpack_raw16[pixels_done] = (b0 << 4) | ((b2 >> 4) & 0x0f);
        unpack_raw16[pixels_done + 1] = (b1 << 4) | ((b2 >> 0) & 0x0f);

        pixels_done += 2;
    }

    if (pixels_done < total_pixels)
    {
        for (int i = pixels_done; i < total_pixels; ++i)
        {
            const int block = i / 2;
            const int lane = i % 2;
            const uint8_t *src = raw + block * 3;
            if (lane == 0)
            {
                unpack_raw16[i] = (src[0] << 4) | ((src[2] >> 4) & 0x0f);
            }
            else
            {
                unpack_raw16[i] = (src[1] << 4) | ((src[2] >> 0) & 0x0f);
            }
        }
    }
}

static void UnpackRaw16ToRaw16Neon(uint8_t *raw, uint16_t *unpack_raw16, int width, int height)
{
    const int bytes = width * height * 2;
    int offset = 0;
    uint8_t *dst_bytes = reinterpret_cast<uint8_t *>(unpack_raw16);

    while (offset + 16 <= bytes)
    {
        uint8x16_t src_bytes = vld1q_u8(raw + offset);
        vst1q_u8(dst_bytes + offset, src_bytes);
        offset += 16;
    }

    if (offset < bytes)
    {
        memcpy(dst_bytes + offset, raw + offset, bytes - offset);
    }
}
#endif

static int MipiDataUnpack(Frame *frame, const IspPrms *prms)
{
    if ((frame == nullptr) || (prms == nullptr))
    {
        LOG(ERROR) << "input prms is null";
        return -1;
    }

    switch (frame->info.dt)
{
case RawDataTypes::RAW10:
#if defined(__aarch64__) || defined(__ARM_NEON) || defined(__ARM_NEON__)
    LOG(INFO) << "unpack route: RAW10 -> NEON";
    UnpackRaw10ToRaw16Neon((uint8_t *)frame->data.raw_u8_i, (uint16_t *)frame->data.raw_u16_o, frame->info.width, frame->info.height);
#else
    LOG(INFO) << "unpack route: RAW10 -> scalar";
    UnpackRaw10ToRaw16Scalar((uint8_t *)frame->data.raw_u8_i, (uint16_t *)frame->data.raw_u16_o, frame->info.width, frame->info.height);
#endif
    break;

case RawDataTypes::RAW12:
#if defined(__aarch64__) || defined(__ARM_NEON) || defined(__ARM_NEON__)
    LOG(INFO) << "unpack route: RAW12 -> NEON";
    UnpackRaw12ToRaw16Neon((uint8_t *)frame->data.raw_u8_i, (uint16_t *)frame->data.raw_u16_o, frame->info.width, frame->info.height);
#else
    LOG(INFO) << "unpack route: RAW12 -> scalar";
    UnpackRaw12ToRaw16Scalar((uint8_t *)frame->data.raw_u8_i, (uint16_t *)frame->data.raw_u16_o, frame->info.width, frame->info.height);
#endif
    break;

case RawDataTypes::RAW16:
#if defined(__aarch64__) || defined(__ARM_NEON) || defined(__ARM_NEON__)
    LOG(INFO) << "unpack route: RAW16 -> NEON";
    UnpackRaw16ToRaw16Neon((uint8_t *)frame->data.raw_u8_i, (uint16_t *)frame->data.raw_u16_o, frame->info.width, frame->info.height);
#else
    LOG(INFO) << "unpack route: RAW16 -> scalar";
    UnpackRaw16ToRaw16Scalar((uint8_t *)frame->data.raw_u8_i, (uint16_t *)frame->data.raw_u16_o, frame->info.width, frame->info.height);
#endif
    break;

default:
    break;
}

    SwapMem<void>(frame->data.raw_u16_o, frame->data.raw_u16_i);

    return 0;
}

void RegisterUnpackMod()
{
    IspModule mod;

    mod.in_type = DataPtrTypes::TYPE_UINT8;
    mod.out_type = DataPtrTypes::TYPE_UINT16;

    mod.in_domain = ColorDomains::RAW;
    mod.out_domain = ColorDomains::RAW;

    mod.name = MOD_NAME;

    mod.run_function = MipiDataUnpack;

    RegisterIspModule(mod);
}