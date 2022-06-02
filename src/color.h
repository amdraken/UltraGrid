/**
 * @file   color.h
 * @author Martin Pulec     <martin.pulec@cesnet.cz>
 */
/*
 * Copyright (c) 2022 CESNET, z. s. p. o.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, is permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of CESNET nor the names of its contributors may be
 *    used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef COLOR_H_CD26B745_C30E_4DA3_8280_C9492B6BFF25
#define COLOR_H_CD26B745_C30E_4DA3_8280_C9492B6BFF25

/* @brief Color space coedfficients - RGB full range to YCbCr bt. 709 limited range
 *
 * RGB should use SDI full range [1<<(depth-8)..255<<(depth-8)-1], see [limits]
 *
 * Scaled by 1<<COMP_BASE, footroom 16/255, headroom 235/255 (luma), 240/255 (chroma); limits [2^(depth-8)..255*2^(depth-8)-1]
 * matrix Y = [ 0.182586, 0.614231, 0.062007; -0.100643, -0.338572, 0.4392157; 0.4392157, -0.398942, -0.040274 ]
 * * [coefficients]: https://gist.github.com/yohhoy/dafa5a47dade85d8b40625261af3776a "Rec. 709 coefficients"
 * * [limits]:       https://tech.ebu.ch/docs/r/r103.pdf                             "SDI limits"
 *
 * @ingroup lavc_video_conversions
 *
 * @todo
 * Use this transformations in all conversions.
 * @{
 */
typedef int32_t comp_type_t; // int32_t provides much better performance than int_fast32_t
#define COMP_BASE (sizeof(comp_type_t) == 4 ? 14 : 18) // computation will be less precise when comp_type_t is 32 bit
static_assert(sizeof(comp_type_t) * 8 >= COMP_BASE + 18, "comp_type_t not wide enough (we are computing in up to 16 bits!)");

#define Y_R ((comp_type_t) ((0.2126*219/255) * (1<<COMP_BASE)))
#define Y_G ((comp_type_t) ((0.7152*219/255) * (1<<COMP_BASE)))
#define Y_B ((comp_type_t) ((0.0722*219/255) * (1<<COMP_BASE)))
#define CB_R ((comp_type_t) ((-0.2126/1.8556*224/255) * (1<<COMP_BASE)))
#define CB_G ((comp_type_t) ((-0.7152/1.8556*224/255) * (1<<COMP_BASE)))
#define CB_B ((comp_type_t) (((1-0.0722)/1.8556*224/255) * (1<<COMP_BASE)))
#define CR_R ((comp_type_t) (((1-0.2126)/1.5748*224/255) * (1<<COMP_BASE)))
#define CR_G ((comp_type_t) ((-0.7152/1.5748*224/255) * (1<<COMP_BASE)))
#define CR_B ((comp_type_t) ((-0.0722/1.5748*224/255) * (1<<COMP_BASE)))
#define RGB_TO_Y_709_SCALED(r, g, b) ((r) * Y_R + (g) * Y_G + (b) * Y_B)
#define RGB_TO_CB_709_SCALED(r, g, b) ((r) * CB_R + (g) * CB_G + (b) * CB_B)
#define RGB_TO_CR_709_SCALED(r, g, b) ((r) * CR_R + (g) * CR_G + (b) * CR_B)
#define CLAMP_LIMITED_Y(val, depth) MIN(MAX(val, 1<<(depth-4)), 235 * (1<<(depth-8)));
#define CLAMP_LIMITED_CBCR(val, depth) MIN(MAX(val, 1<<(depth-4)), 240 * (1<<(depth-8)));

#define KR_709 .212639
#define KB_709 .072192

#define KG(kr,kb)  (1.-kr-kb)
#define Y_LIMIT    (255.0/219.0)
#define CBCR_LIMIT (255.0/224.0)
#define R_CB(kr,kb) 0.0
#define R_CR(kr,kb) (CBCR_LIMIT*(2.*(1.-kr)))
#define G_CB(kr,kb) (-CBCR_LIMIT*kb*(2.*(kr+KG(kr,kb)))/KG(kr,kb))
#define G_CR(kr,kb) (-CBCR_LIMIT*kr*(2.*(1.-kr))/KG(kr,kb))
#define B_CB(kr,kb) (CBCR_LIMIT*(2.*(kr+KG(kr,kb))))
#define B_CR(kr,kb) 0.0
#define SCALED(x) ((comp_type_t) ((x) * (1<<COMP_BASE)))
#define Y_SCALE SCALED(Y_LIMIT) // precomputed value, Y multiplier is same for all channels
#define YCBCR_TO_R_709_SCALED(y, cb, cr) ((y) /* * r_y */ /* + (cb) * SCALED(r_cb(KR_709,KB_709)) */ + (cr) * SCALED(R_CR(KR_709,KB_709)))
#define YCBCR_TO_G_709_SCALED(y, cb, cr) ((y) /* * g_y */    + (cb) * SCALED(G_CB(KR_709,KB_709))    + (cr) * SCALED(G_CR(KR_709,KB_709)))
#define YCBCR_TO_B_709_SCALED(y, cb, cr) ((y) /* * b_y */    + (cb) * SCALED(B_CB(KR_709,KB_709)) /* + (cr) * SCALED(b_cr(KR_709,KB_709))) */)

#define FULL_FOOT(depth) (1<<((depth)-8))
#define FULL_HEAD(depth) ((255<<((depth)-8))-1)
#define CLAMP_FULL(val, depth) MIN(FULL_HEAD(depth), MAX((val), FULL_FOOT(depth)))

#define FORMAT_RGBA(r, g, b, depth) (~(0xFFU << (rgb_shift[R]) | 0xFFU << (rgb_shift[G]) | 0xFFU << (rgb_shift[B])) | \
        (CLAMP_FULL((r), (depth)) << rgb_shift[R] | CLAMP_FULL((g), (depth)) << rgb_shift[G] | CLAMP_FULL((b), (depth)) << rgb_shift[B]))
/// @}

#endif // !defined COLOR_H_CD26B745_C30E_4DA3_8280_C9492B6BFF25
