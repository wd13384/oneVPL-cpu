/*############################################################################
  # Copyright (C) 2020 Intel Corporation
  #
  # SPDX-License-Identifier: MIT
  ############################################################################*/

#include <gtest/gtest.h>
#include "vpl/mfxjpeg.h"
#include "vpl/mfxvideo.h"

/* Init overview
    This function allocates memory and prepares structures for encode, decode, and VPP.
    Parameters are validated to ensure they are supported. 

   MFX_ERR_NONE  The function completed successfully. \n
   MFX_ERR_INVALID_VIDEO_PARAM  The function detected invalid video parameters. 
           These parameters may be out of the valid range, or the combination resulted in incompatibility. 
           Incompatibility not resolved. \n
   MFX_WRN_INCOMPATIBLE_VIDEO_PARAM  The function detected some video parameters were incompatible with others; 
           incompatibility resolved. \n
   MFX_ERR_UNDEFINED_BEHAVIOR  The function is called twice without a close;
*/

//EncodeInit
TEST(EncodeInit, ValidParamsInReturnsErrNone) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxEncParams               = { 0 };
    mfxEncParams.mfx.CodecId                 = MFX_CODEC_JPEG;
    mfxEncParams.mfx.FrameInfo.FourCC        = MFX_FOURCC_I420;
    mfxEncParams.mfx.FrameInfo.ChromaFormat  = MFX_CHROMAFORMAT_YUV420;
    mfxEncParams.mfx.FrameInfo.CropW         = 128;
    mfxEncParams.mfx.FrameInfo.CropH         = 96;
    mfxEncParams.mfx.FrameInfo.Width         = 128;
    mfxEncParams.mfx.FrameInfo.Height        = 96;
    mfxEncParams.mfx.FrameInfo.FrameRateExtN = 30;
    mfxEncParams.mfx.FrameInfo.FrameRateExtD = 1;
    mfxEncParams.IOPattern                   = MFX_IOPATTERN_IN_SYSTEM_MEMORY;

    sts = MFXVideoENCODE_Init(session, &mfxEncParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(EncodeInit, InvalidParamsInReturnsInvalidVideoParam) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxEncParams              = { 0 };
    mfxEncParams.mfx.CodecId                = MFX_CODEC_HEVC;
    mfxEncParams.mfx.FrameInfo.FourCC       = MFX_FOURCC_I420;
    mfxEncParams.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
    mfxEncParams.mfx.FrameInfo.CropW        = 128;
    mfxEncParams.mfx.FrameInfo.CropH        = 96;
    mfxEncParams.mfx.FrameInfo.Width        = 128;
    mfxEncParams.mfx.FrameInfo.Height       = 96;
    mfxEncParams.IOPattern                  = MFX_IOPATTERN_IN_VIDEO_MEMORY;

    sts = MFXVideoENCODE_Init(session, &mfxEncParams);
    ASSERT_EQ(sts, MFX_ERR_INVALID_VIDEO_PARAM);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(EncodeInit, MissingIOPatternReturnsInvalidVideoParam) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxEncParams              = { 0 };
    mfxEncParams.mfx.CodecId                = MFX_CODEC_HEVC;
    mfxEncParams.mfx.FrameInfo.FourCC       = MFX_FOURCC_I420;
    mfxEncParams.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
    mfxEncParams.mfx.FrameInfo.CropW        = 128;
    mfxEncParams.mfx.FrameInfo.CropH        = 96;
    mfxEncParams.mfx.FrameInfo.Width        = 128;
    mfxEncParams.mfx.FrameInfo.Height       = 96;

    sts = MFXVideoENCODE_Init(session, &mfxEncParams);
    ASSERT_EQ(sts, MFX_ERR_INVALID_VIDEO_PARAM);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

//profile/level incompatible with resolution
TEST(EncodeInit, DISABLED_IncompatibleParamsInReturnsIncompatibleVideoParam) {
    FAIL() << "Test not implemented";
}

TEST(EncodeInit, NullParamsInReturnsErrNull) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoENCODE_Init(session, nullptr);
    ASSERT_EQ(sts, MFX_ERR_NULL_PTR);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(EncodeInit, NullSessionInReturnsInvalidHandle) {
    mfxStatus sts = MFXVideoENCODE_Init(0, nullptr);
    ASSERT_EQ(sts, MFX_ERR_INVALID_HANDLE);
}

TEST(EncodeInit, DoubleInitReturnsUndefinedBehavior) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxEncParams               = { 0 };
    mfxEncParams.mfx.CodecId                 = MFX_CODEC_JPEG;
    mfxEncParams.mfx.FrameInfo.FourCC        = MFX_FOURCC_I420;
    mfxEncParams.mfx.FrameInfo.ChromaFormat  = MFX_CHROMAFORMAT_YUV420;
    mfxEncParams.mfx.FrameInfo.CropW         = 128;
    mfxEncParams.mfx.FrameInfo.CropH         = 96;
    mfxEncParams.mfx.FrameInfo.Width         = 128;
    mfxEncParams.mfx.FrameInfo.Height        = 96;
    mfxEncParams.mfx.FrameInfo.FrameRateExtN = 30;
    mfxEncParams.mfx.FrameInfo.FrameRateExtD = 1;
    mfxEncParams.IOPattern                   = MFX_IOPATTERN_IN_SYSTEM_MEMORY;

    sts = MFXVideoENCODE_Init(session, &mfxEncParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoENCODE_Init(session, &mfxEncParams);
    ASSERT_EQ(sts, MFX_ERR_UNDEFINED_BEHAVIOR);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(EncodeInit, EncodeParamsInReturnsInitializedHEVCContext) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxEncParams               = { 0 };
    mfxEncParams.mfx.CodecId                 = MFX_CODEC_HEVC;
    mfxEncParams.mfx.TargetUsage             = MFX_TARGETUSAGE_BALANCED;
    mfxEncParams.mfx.TargetKbps              = 4000;
    mfxEncParams.mfx.InitialDelayInKB        = 1000;
    mfxEncParams.mfx.RateControlMethod       = MFX_RATECONTROL_VBR;
    mfxEncParams.mfx.FrameInfo.FrameRateExtN = 30;
    mfxEncParams.mfx.FrameInfo.FrameRateExtD = 1;
    mfxEncParams.mfx.FrameInfo.FourCC        = MFX_FOURCC_I420;
    mfxEncParams.mfx.FrameInfo.ChromaFormat  = MFX_CHROMAFORMAT_YUV420;
    mfxEncParams.mfx.FrameInfo.PicStruct     = MFX_PICSTRUCT_PROGRESSIVE;
    mfxEncParams.mfx.FrameInfo.CropX         = 0;
    mfxEncParams.mfx.FrameInfo.CropY         = 0;
    mfxEncParams.mfx.FrameInfo.CropW         = 128;
    mfxEncParams.mfx.FrameInfo.CropH         = 96;
    mfxEncParams.mfx.FrameInfo.Width         = 128;
    mfxEncParams.mfx.FrameInfo.Height        = 96;
    mfxEncParams.mfx.NumSlice                = 0;
    mfxEncParams.mfx.FrameInfo.AspectRatioW  = 4;
    mfxEncParams.mfx.FrameInfo.AspectRatioH  = 3;
    mfxEncParams.mfx.CodecProfile            = MFX_PROFILE_HEVC_MAIN;
    mfxEncParams.IOPattern                   = MFX_IOPATTERN_IN_SYSTEM_MEMORY;

    sts = MFXVideoENCODE_Init(session, &mfxEncParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    //GetVideoParam reads values from the encoder context
    mfxVideoParam par;
    sts = MFXVideoENCODE_GetVideoParam(session, &par);
    ASSERT_EQ(sts, MFX_ERR_NONE);
    ASSERT_EQ(128, par.mfx.FrameInfo.Width);
    ASSERT_EQ(96, par.mfx.FrameInfo.Height);
    ASSERT_EQ(MFX_RATECONTROL_VBR, par.mfx.RateControlMethod);
    ASSERT_EQ(4000, par.mfx.TargetKbps);
    ASSERT_EQ(1000, par.mfx.InitialDelayInKB);
    ASSERT_EQ(0, par.mfx.NumSlice);
    ASSERT_EQ(MFX_PROFILE_HEVC_MAIN, par.mfx.CodecProfile);
    ASSERT_EQ(MFX_TARGETUSAGE_BALANCED, par.mfx.TargetUsage);
    ASSERT_EQ(4, mfxEncParams.mfx.FrameInfo.AspectRatioW);
    ASSERT_EQ(3, mfxEncParams.mfx.FrameInfo.AspectRatioH);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(EncodeInit, EncodeParamsInReturnsInitializedAV1Context) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxEncParams               = { 0 };
    mfxEncParams.mfx.CodecId                 = MFX_CODEC_AV1;
    mfxEncParams.mfx.TargetUsage             = MFX_TARGETUSAGE_BALANCED;
    mfxEncParams.mfx.TargetKbps              = 4000;
    mfxEncParams.mfx.InitialDelayInKB        = 1000;
    mfxEncParams.mfx.RateControlMethod       = MFX_RATECONTROL_VBR;
    mfxEncParams.mfx.FrameInfo.FrameRateExtN = 30;
    mfxEncParams.mfx.FrameInfo.FrameRateExtD = 1;
    mfxEncParams.mfx.FrameInfo.FourCC        = MFX_FOURCC_I420;
    mfxEncParams.mfx.FrameInfo.ChromaFormat  = MFX_CHROMAFORMAT_YUV420;
    mfxEncParams.mfx.FrameInfo.PicStruct     = MFX_PICSTRUCT_PROGRESSIVE;
    mfxEncParams.mfx.FrameInfo.CropX         = 0;
    mfxEncParams.mfx.FrameInfo.CropY         = 0;
    mfxEncParams.mfx.FrameInfo.CropW         = 128;
    mfxEncParams.mfx.FrameInfo.CropH         = 96;
    mfxEncParams.mfx.FrameInfo.Width         = 128;
    mfxEncParams.mfx.FrameInfo.Height        = 96;
    mfxEncParams.mfx.NumSlice                = 0;
    mfxEncParams.mfx.FrameInfo.AspectRatioW  = 4;
    mfxEncParams.mfx.FrameInfo.AspectRatioH  = 3;
    mfxEncParams.IOPattern                   = MFX_IOPATTERN_IN_SYSTEM_MEMORY;

    sts = MFXVideoENCODE_Init(session, &mfxEncParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    //GetVideoParam reads values from the encoder context
    mfxVideoParam par;
    sts = MFXVideoENCODE_GetVideoParam(session, &par);
    ASSERT_EQ(sts, MFX_ERR_NONE);
    ASSERT_EQ(128, par.mfx.FrameInfo.Width);
    ASSERT_EQ(96, par.mfx.FrameInfo.Height);
    ASSERT_EQ(MFX_RATECONTROL_VBR, par.mfx.RateControlMethod);
    ASSERT_EQ(4000, par.mfx.TargetKbps);
    ASSERT_EQ(1000, par.mfx.InitialDelayInKB);
    ASSERT_EQ(0, par.mfx.NumSlice);
    ASSERT_EQ(MFX_TARGETUSAGE_BALANCED, par.mfx.TargetUsage);
    ASSERT_EQ(4, mfxEncParams.mfx.FrameInfo.AspectRatioW);
    ASSERT_EQ(3, mfxEncParams.mfx.FrameInfo.AspectRatioH);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(EncodeInit, EncodeParamsInReturnsInitializedJPEGContext) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxEncParams;
    memset(&mfxEncParams, 0, sizeof(mfxEncParams));
    mfxEncParams.mfx.CodecId                 = MFX_CODEC_JPEG;
    mfxEncParams.mfx.FrameInfo.FourCC        = MFX_FOURCC_I420;
    mfxEncParams.mfx.FrameInfo.ChromaFormat  = MFX_CHROMAFORMAT_YUV420;
    mfxEncParams.mfx.FrameInfo.CropW         = 128;
    mfxEncParams.mfx.FrameInfo.CropH         = 96;
    mfxEncParams.mfx.FrameInfo.Width         = 128;
    mfxEncParams.mfx.FrameInfo.Height        = 96;
    mfxEncParams.mfx.FrameInfo.FrameRateExtN = 30;
    mfxEncParams.mfx.FrameInfo.FrameRateExtD = 1;
    mfxEncParams.IOPattern                   = MFX_IOPATTERN_IN_SYSTEM_MEMORY;

    sts = MFXVideoENCODE_Init(session, &mfxEncParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    //GetVideoParam reads values from the encoder context
    mfxVideoParam par;
    sts = MFXVideoENCODE_GetVideoParam(session, &par);
    ASSERT_EQ(sts, MFX_ERR_NONE);
    ASSERT_EQ(128, par.mfx.FrameInfo.Width);
    ASSERT_EQ(96, par.mfx.FrameInfo.Height);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(EncodeInit, AV1CloseCrashes) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxEncParams               = { 0 };
    mfxEncParams.mfx.CodecId                 = MFX_CODEC_AV1;
    mfxEncParams.mfx.TargetUsage             = MFX_TARGETUSAGE_BALANCED;
    mfxEncParams.mfx.TargetKbps              = 4000;
    mfxEncParams.mfx.RateControlMethod       = MFX_RATECONTROL_VBR;
    mfxEncParams.mfx.FrameInfo.FrameRateExtN = 30;
    mfxEncParams.mfx.FrameInfo.FrameRateExtD = 1;
    mfxEncParams.mfx.FrameInfo.FourCC        = MFX_FOURCC_I420;
    mfxEncParams.mfx.FrameInfo.ChromaFormat  = MFX_CHROMAFORMAT_YUV420;
    mfxEncParams.mfx.FrameInfo.CropX         = 0;
    mfxEncParams.mfx.FrameInfo.CropY         = 0;
    mfxEncParams.mfx.FrameInfo.CropW         = 128;
    mfxEncParams.mfx.FrameInfo.CropH         = 96;
    mfxEncParams.mfx.FrameInfo.Width         = 128;
    mfxEncParams.mfx.FrameInfo.Height        = 96;
    mfxEncParams.IOPattern                   = MFX_IOPATTERN_IN_SYSTEM_MEMORY;

    sts = MFXVideoENCODE_Init(session, &mfxEncParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    //GetVideoParam reads values from the encoder context
    mfxVideoParam par;
    sts = MFXVideoENCODE_GetVideoParam(session, &par);
    ASSERT_EQ(sts, MFX_ERR_NONE);
    ASSERT_EQ(128, par.mfx.FrameInfo.Width);
    ASSERT_EQ(96, par.mfx.FrameInfo.Height);
    ASSERT_EQ(MFX_RATECONTROL_VBR, par.mfx.RateControlMethod);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(EncodeInit, BadAV1GopSizeReturnsInvalidParam) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxEncParams              = { 0 };
    mfxEncParams.mfx.CodecId                = MFX_CODEC_AV1;
    mfxEncParams.mfx.FrameInfo.FourCC       = MFX_FOURCC_I420;
    mfxEncParams.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
    mfxEncParams.mfx.FrameInfo.CropW        = 128;
    mfxEncParams.mfx.FrameInfo.CropH        = 96;
    mfxEncParams.mfx.FrameInfo.Width        = 128;
    mfxEncParams.mfx.FrameInfo.Height       = 96;
    mfxEncParams.mfx.GopPicSize             = 121;

    sts = MFXVideoENCODE_Init(session, &mfxEncParams);
    ASSERT_EQ(sts, MFX_ERR_INVALID_VIDEO_PARAM);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

//DecodeInit
TEST(DecodeInit, ValidParamsInReturnsErrNone) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxDecParams = { 0 };
    mfxDecParams.mfx.CodecId   = MFX_CODEC_HEVC;
    mfxDecParams.IOPattern     = MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

    mfxDecParams.mfx.FrameInfo.FourCC       = MFX_FOURCC_I420;
    mfxDecParams.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
    mfxDecParams.mfx.FrameInfo.CropW        = 128;
    mfxDecParams.mfx.FrameInfo.CropH        = 96;
    mfxDecParams.mfx.FrameInfo.Width        = 128;
    mfxDecParams.mfx.FrameInfo.Height       = 96;

    sts = MFXVideoDECODE_Init(session, &mfxDecParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(DecodeInit, MismatchedChromaFormatInReturnsErrInvalidVideoParam) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxDecParams = { 0 };
    mfxDecParams.mfx.CodecId   = MFX_CODEC_HEVC;
    mfxDecParams.IOPattern     = MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

    mfxDecParams.mfx.FrameInfo.FourCC       = MFX_FOURCC_I420;
    mfxDecParams.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV444;
    mfxDecParams.mfx.FrameInfo.CropW        = 128;
    mfxDecParams.mfx.FrameInfo.CropH        = 96;
    mfxDecParams.mfx.FrameInfo.Width        = 128;
    mfxDecParams.mfx.FrameInfo.Height       = 96;

    sts = MFXVideoDECODE_Init(session, &mfxDecParams);
    ASSERT_EQ(sts, MFX_ERR_INVALID_VIDEO_PARAM);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(DecodeInit, InValidFourCCInReturnsErrInvalidVideoParam) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxDecParams = { 0 };
    mfxDecParams.mfx.CodecId   = MFX_CODEC_HEVC;
    mfxDecParams.IOPattern     = MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

    mfxDecParams.mfx.FrameInfo.FourCC       = 1;
    mfxDecParams.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
    mfxDecParams.mfx.FrameInfo.CropW        = 128;
    mfxDecParams.mfx.FrameInfo.CropH        = 96;
    mfxDecParams.mfx.FrameInfo.Width        = 128;
    mfxDecParams.mfx.FrameInfo.Height       = 96;

    sts = MFXVideoDECODE_Init(session, &mfxDecParams);
    ASSERT_EQ(sts, MFX_ERR_INVALID_VIDEO_PARAM);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(DecodeInit, MissingIOPatternReturnsInvalidVideoParam) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxDecParams = { 0 };
    mfxDecParams.mfx.CodecId   = MFX_CODEC_HEVC;

    mfxDecParams.mfx.FrameInfo.FourCC       = MFX_FOURCC_I420;
    mfxDecParams.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
    mfxDecParams.mfx.FrameInfo.CropW        = 128;
    mfxDecParams.mfx.FrameInfo.CropH        = 96;
    mfxDecParams.mfx.FrameInfo.Width        = 128;
    mfxDecParams.mfx.FrameInfo.Height       = 96;

    sts = MFXVideoDECODE_Init(session, &mfxDecParams);
    ASSERT_EQ(sts, MFX_ERR_INVALID_VIDEO_PARAM);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(DecodeInit, InvalidParamsInReturnsInvalidVideoParam) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxDecParams = { 0 };
    mfxDecParams.mfx.CodecId   = MFX_CODEC_HEVC;
    mfxDecParams.IOPattern     = MFX_IOPATTERN_OUT_VIDEO_MEMORY;

    mfxDecParams.mfx.FrameInfo.FourCC       = MFX_FOURCC_I420;
    mfxDecParams.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
    mfxDecParams.mfx.FrameInfo.CropW        = 128;
    mfxDecParams.mfx.FrameInfo.CropH        = 96;
    mfxDecParams.mfx.FrameInfo.Width        = 128;
    mfxDecParams.mfx.FrameInfo.Height       = 96;

    sts = MFXVideoDECODE_Init(session, &mfxDecParams);
    ASSERT_EQ(sts, MFX_ERR_INVALID_VIDEO_PARAM);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

//profile/level incompatible with resolution
TEST(DecodeInit, DISABLED_IncompatibleParamsInReturnsIncompatibleVideoParam) {
    FAIL() << "Test not implemented";
}

TEST(DecodeInit, NullParamsInReturnsErrNull) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoDECODE_Init(session, nullptr);
    ASSERT_EQ(sts, MFX_ERR_NULL_PTR);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(DecodeInit, NullSessionInReturnsInvalidHandle) {
    mfxStatus sts = MFXVideoDECODE_Init(0, nullptr);
    ASSERT_EQ(sts, MFX_ERR_INVALID_HANDLE);
}

TEST(DecodeInit, DoubleInitReturnsUndefinedBehavior) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxDecParams = { 0 };
    mfxDecParams.mfx.CodecId   = MFX_CODEC_HEVC;
    mfxDecParams.IOPattern     = MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

    mfxDecParams.mfx.FrameInfo.FourCC       = MFX_FOURCC_I420;
    mfxDecParams.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
    mfxDecParams.mfx.FrameInfo.CropW        = 128;
    mfxDecParams.mfx.FrameInfo.CropH        = 96;
    mfxDecParams.mfx.FrameInfo.Width        = 128;
    mfxDecParams.mfx.FrameInfo.Height       = 96;

    sts = MFXVideoDECODE_Init(session, &mfxDecParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoDECODE_Init(session, &mfxDecParams);
    ASSERT_EQ(sts, MFX_ERR_UNDEFINED_BEHAVIOR);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

//VPPInit
TEST(VPPInit, ValidParamsInReturnsErrNone) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxVPPParams = { 0 };

    // Input data
    mfxVPPParams.vpp.In.FourCC        = MFX_FOURCC_I420;
    mfxVPPParams.vpp.In.ChromaFormat  = MFX_CHROMAFORMAT_YUV420;
    mfxVPPParams.vpp.In.CropW         = 128;
    mfxVPPParams.vpp.In.CropH         = 96;
    mfxVPPParams.vpp.In.FrameRateExtN = 30;
    mfxVPPParams.vpp.In.FrameRateExtD = 1;
    mfxVPPParams.vpp.In.Width         = mfxVPPParams.vpp.In.CropW;
    mfxVPPParams.vpp.In.Height        = mfxVPPParams.vpp.In.CropH;
    // Output data
    mfxVPPParams.vpp.Out        = mfxVPPParams.vpp.In;
    mfxVPPParams.vpp.Out.FourCC = MFX_FOURCC_BGRA;
    mfxVPPParams.IOPattern      = MFX_IOPATTERN_IN_SYSTEM_MEMORY | MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

    sts = MFXVideoVPP_Init(session, &mfxVPPParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(VPPInit, InvalidParamsInReturnsInvalidVideoParam) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxVPPParams = { 0 };

    // Input data
    mfxVPPParams.vpp.In.FourCC        = MFX_FOURCC_I420;
    mfxVPPParams.vpp.In.CropW         = 128;
    mfxVPPParams.vpp.In.CropH         = 96;
    mfxVPPParams.vpp.In.FrameRateExtN = 30;
    mfxVPPParams.vpp.In.FrameRateExtD = 1;
    mfxVPPParams.vpp.In.Width         = mfxVPPParams.vpp.In.CropW;
    mfxVPPParams.vpp.In.Height        = mfxVPPParams.vpp.In.CropH;
    // Output data
    mfxVPPParams.vpp.Out       = mfxVPPParams.vpp.In;
    mfxVPPParams.vpp.In.FourCC = MFX_FOURCC_BGRA;
    mfxVPPParams.IOPattern     = MFX_IOPATTERN_IN_SYSTEM_MEMORY | MFX_IOPATTERN_OUT_VIDEO_MEMORY;

    sts = MFXVideoVPP_Init(session, &mfxVPPParams);
    ASSERT_EQ(sts, MFX_ERR_INVALID_VIDEO_PARAM);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(VPPInit, MissingIOPatternReturnsInvalidVideoParam) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxVPPParams = { 0 };

    // Input data
    mfxVPPParams.vpp.In.FourCC        = MFX_FOURCC_I420;
    mfxVPPParams.vpp.In.CropW         = 128;
    mfxVPPParams.vpp.In.CropH         = 96;
    mfxVPPParams.vpp.In.FrameRateExtN = 30;
    mfxVPPParams.vpp.In.FrameRateExtD = 1;
    mfxVPPParams.vpp.In.Width         = mfxVPPParams.vpp.In.CropW;
    mfxVPPParams.vpp.In.Height        = mfxVPPParams.vpp.In.CropH;
    // Output data
    mfxVPPParams.vpp.Out       = mfxVPPParams.vpp.In;
    mfxVPPParams.vpp.In.FourCC = MFX_FOURCC_BGRA;

    sts = MFXVideoVPP_Init(session, &mfxVPPParams);
    ASSERT_EQ(sts, MFX_ERR_INVALID_VIDEO_PARAM);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(VPPInit, NullParamsInReturnsErrNull) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoVPP_Init(session, nullptr);
    ASSERT_EQ(sts, MFX_ERR_NULL_PTR);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(VPPInit, NullSessionInReturnsInvalidHandle) {
    mfxStatus sts = MFXVideoVPP_Init(0, nullptr);
    ASSERT_EQ(sts, MFX_ERR_INVALID_HANDLE);
}

TEST(VPPInit, DoubleInitReturnsUndefinedBehavior) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxVPPParams = { 0 };

    // Input data
    mfxVPPParams.vpp.In.FourCC        = MFX_FOURCC_I420;
    mfxVPPParams.vpp.In.ChromaFormat  = MFX_CHROMAFORMAT_YUV420;
    mfxVPPParams.vpp.In.CropW         = 128;
    mfxVPPParams.vpp.In.CropH         = 96;
    mfxVPPParams.vpp.In.FrameRateExtN = 30;
    mfxVPPParams.vpp.In.FrameRateExtD = 1;
    mfxVPPParams.vpp.In.Width         = mfxVPPParams.vpp.In.CropW;
    mfxVPPParams.vpp.In.Height        = mfxVPPParams.vpp.In.CropH;
    // Output data
    mfxVPPParams.vpp.Out       = mfxVPPParams.vpp.In;
    mfxVPPParams.vpp.In.FourCC = MFX_FOURCC_BGRA;
    mfxVPPParams.IOPattern     = MFX_IOPATTERN_IN_SYSTEM_MEMORY | MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

    sts = MFXVideoVPP_Init(session, &mfxVPPParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoVPP_Init(session, &mfxVPPParams);
    ASSERT_EQ(sts, MFX_ERR_UNDEFINED_BEHAVIOR);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

/* Reset overview

    This function stops the current operation and restores internal structures for a new operation, 
    possibly with new parameters.
    
    For decode:
     * It recovers the decoder from errors.
    * It restarts decoding from a new position
    The function resets the old sequence header (sequence parameter set in H.264, or sequence header in MPEG-2 and VC-1). The decoder will expect a new sequence header 
    before it decodes the next frame and will skip any bitstream before encountering the new sequence header.

   MFX_ERR_NONE  The function completed successfully. \n
   MFX_ERR_INVALID_VIDEO_PARAM  The function detected invalid video parameters. These parameters may be out of the valid range, or the combination of them
                                resulted in incompatibility. Incompatibility not resolved. \n

   MFX_WRN_INCOMPATIBLE_VIDEO_PARAM  The function detected some video parameters were incompatible with others; incompatibility resolved.

mfxStatus MFX_CDECL MFXVideoENCODE_Reset(mfxSession session, mfxVideoParam *par);
mfxStatus MFX_CDECL MFXVideoDECODE_Reset(mfxSession session, mfxVideoParam *par);
mfxStatus MFX_CDECL MFXVideoVPP_Reset(mfxSession session, mfxVideoParam *par);
*/

//EncodeReset
TEST(EncodeReset, ValidParamsInReturnsErrNone) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxEncParams               = { 0 };
    mfxEncParams.mfx.CodecId                 = MFX_CODEC_JPEG;
    mfxEncParams.mfx.FrameInfo.FourCC        = MFX_FOURCC_I420;
    mfxEncParams.mfx.FrameInfo.ChromaFormat  = MFX_CHROMAFORMAT_YUV420;
    mfxEncParams.mfx.FrameInfo.CropW         = 128;
    mfxEncParams.mfx.FrameInfo.CropH         = 96;
    mfxEncParams.mfx.FrameInfo.Width         = 128;
    mfxEncParams.mfx.FrameInfo.Height        = 96;
    mfxEncParams.mfx.FrameInfo.FrameRateExtN = 30;
    mfxEncParams.mfx.FrameInfo.FrameRateExtD = 1;
    mfxEncParams.IOPattern                   = MFX_IOPATTERN_IN_SYSTEM_MEMORY;

    sts = MFXVideoENCODE_Init(session, &mfxEncParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoENCODE_Reset(session, &mfxEncParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(EncodeReset, InvalidParamsInReturnsInvalidVideoParam) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxEncParams               = { 0 };
    mfxEncParams.mfx.CodecId                 = MFX_CODEC_JPEG;
    mfxEncParams.mfx.FrameInfo.FourCC        = MFX_FOURCC_I420;
    mfxEncParams.mfx.FrameInfo.ChromaFormat  = MFX_CHROMAFORMAT_YUV420;
    mfxEncParams.mfx.FrameInfo.CropW         = 128;
    mfxEncParams.mfx.FrameInfo.CropH         = 96;
    mfxEncParams.mfx.FrameInfo.Width         = 128;
    mfxEncParams.mfx.FrameInfo.Height        = 96;
    mfxEncParams.mfx.FrameInfo.FrameRateExtN = 30;
    mfxEncParams.mfx.FrameInfo.FrameRateExtD = 1;
    mfxEncParams.IOPattern                   = MFX_IOPATTERN_IN_SYSTEM_MEMORY;

    sts = MFXVideoENCODE_Init(session, &mfxEncParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxEncParams.IOPattern = MFX_IOPATTERN_IN_VIDEO_MEMORY;
    sts                    = MFXVideoENCODE_Reset(session, &mfxEncParams);
    ASSERT_EQ(sts, MFX_ERR_INVALID_VIDEO_PARAM);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

//profile/level incompatible with resolution
TEST(EncodeReset, DISABLED_IncompatibleParamsInReturnsIncompatibleVideoParam) {
    FAIL() << "Test not implemented";
}

TEST(EncodeReset, NullParamsInReturnsErrNull) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxEncParams               = { 0 };
    mfxEncParams.mfx.CodecId                 = MFX_CODEC_JPEG;
    mfxEncParams.mfx.FrameInfo.FourCC        = MFX_FOURCC_I420;
    mfxEncParams.mfx.FrameInfo.ChromaFormat  = MFX_CHROMAFORMAT_YUV420;
    mfxEncParams.mfx.FrameInfo.CropW         = 128;
    mfxEncParams.mfx.FrameInfo.CropH         = 96;
    mfxEncParams.mfx.FrameInfo.Width         = 128;
    mfxEncParams.mfx.FrameInfo.Height        = 96;
    mfxEncParams.mfx.FrameInfo.FrameRateExtN = 30;
    mfxEncParams.mfx.FrameInfo.FrameRateExtD = 1;
    mfxEncParams.IOPattern                   = MFX_IOPATTERN_IN_SYSTEM_MEMORY;

    sts = MFXVideoENCODE_Init(session, &mfxEncParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoENCODE_Reset(session, nullptr);
    ASSERT_EQ(sts, MFX_ERR_NULL_PTR);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(EncodeReset, NullSessionInReturnsInvalidHandle) {
    mfxStatus sts = MFXVideoENCODE_Reset(0, nullptr);
    ASSERT_EQ(sts, MFX_ERR_INVALID_HANDLE);
}

TEST(EncodeReset, DISABLED_UninitializedEncodeReturnsErrNotInitialized) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxEncParams              = { 0 };
    mfxEncParams.mfx.CodecId                = MFX_CODEC_JPEG;
    mfxEncParams.mfx.FrameInfo.FourCC       = MFX_FOURCC_I420;
    mfxEncParams.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
    mfxEncParams.mfx.FrameInfo.CropW        = 128;
    mfxEncParams.mfx.FrameInfo.CropH        = 96;
    mfxEncParams.mfx.FrameInfo.Width        = 128;
    mfxEncParams.mfx.FrameInfo.Height       = 96;
    mfxEncParams.IOPattern                  = MFX_IOPATTERN_IN_SYSTEM_MEMORY;

    sts = MFXVideoENCODE_Reset(session, &mfxEncParams);
    ASSERT_EQ(sts, MFX_ERR_NOT_INITIALIZED);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

//DecodeReset
TEST(DecodeReset, ValidParamsInReturnsErrNone) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxDecParams = { 0 };
    mfxDecParams.mfx.CodecId   = MFX_CODEC_HEVC;
    mfxDecParams.IOPattern     = MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

    mfxDecParams.mfx.FrameInfo.FourCC       = MFX_FOURCC_I420;
    mfxDecParams.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
    mfxDecParams.mfx.FrameInfo.CropW        = 128;
    mfxDecParams.mfx.FrameInfo.CropH        = 96;
    mfxDecParams.mfx.FrameInfo.Width        = 128;
    mfxDecParams.mfx.FrameInfo.Height       = 96;

    sts = MFXVideoDECODE_Init(session, &mfxDecParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoDECODE_Reset(session, &mfxDecParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(DecodeReset, InvalidParamsInReturnsInvalidVideoParam) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxDecParams = { 0 };
    mfxDecParams.mfx.CodecId   = MFX_CODEC_HEVC;
    mfxDecParams.IOPattern     = MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

    mfxDecParams.mfx.FrameInfo.FourCC = MFX_FOURCC_I420;
    mfxDecParams.mfx.FrameInfo.CropW  = 128;
    mfxDecParams.mfx.FrameInfo.CropH  = 96;
    mfxDecParams.mfx.FrameInfo.Width  = 128;
    mfxDecParams.mfx.FrameInfo.Height = 96;

    sts = MFXVideoDECODE_Init(session, &mfxDecParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxDecParams.IOPattern = MFX_IOPATTERN_OUT_VIDEO_MEMORY;
    sts                    = MFXVideoDECODE_Reset(session, &mfxDecParams);
    ASSERT_EQ(sts, MFX_ERR_INVALID_VIDEO_PARAM);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

//profile/level incompatible with resolution
TEST(DecodeReset, DISABLED_IncompatibleParamsInReturnsIncompatibleVideoParam) {
    FAIL() << "Test not implemented";
}

TEST(DecodeReset, NullParamsInReturnsErrNull) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxDecParams = { 0 };
    mfxDecParams.mfx.CodecId   = MFX_CODEC_HEVC;
    mfxDecParams.IOPattern     = MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

    mfxDecParams.mfx.FrameInfo.FourCC       = MFX_FOURCC_I420;
    mfxDecParams.mfx.FrameInfo.CropW        = 128;
    mfxDecParams.mfx.FrameInfo.CropH        = 96;
    mfxDecParams.mfx.FrameInfo.Width        = 128;
    mfxDecParams.mfx.FrameInfo.Height       = 96;
    mfxDecParams.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;

    sts = MFXVideoDECODE_Init(session, &mfxDecParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoDECODE_Reset(session, nullptr);
    ASSERT_EQ(sts, MFX_ERR_NULL_PTR);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(DecodeReset, NullSessionInReturnsInvalidHandle) {
    mfxStatus sts = MFXVideoDECODE_Reset(0, nullptr);
    ASSERT_EQ(sts, MFX_ERR_INVALID_HANDLE);
}

TEST(DecodeReset, DISABLED_UninitializedDecodeReturnsErrNotInitialized) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxDecParams = { 0 };
    mfxDecParams.mfx.CodecId   = MFX_CODEC_HEVC;
    mfxDecParams.IOPattern     = MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

    mfxDecParams.mfx.FrameInfo.FourCC = MFX_FOURCC_I420;
    mfxDecParams.mfx.FrameInfo.CropW  = 128;
    mfxDecParams.mfx.FrameInfo.CropH  = 96;
    mfxDecParams.mfx.FrameInfo.Width  = 128;
    mfxDecParams.mfx.FrameInfo.Height = 96;

    sts = MFXVideoDECODE_Reset(session, &mfxDecParams);
    ASSERT_EQ(sts, MFX_ERR_NOT_INITIALIZED);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

//VPPReset
TEST(VPPReset, ValidParamsInReturnsErrNone) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxVPPParams = { 0 };

    // Input data
    mfxVPPParams.vpp.In.FourCC        = MFX_FOURCC_I420;
    mfxVPPParams.vpp.In.ChromaFormat  = MFX_CHROMAFORMAT_YUV420;
    mfxVPPParams.vpp.In.CropW         = 128;
    mfxVPPParams.vpp.In.CropH         = 96;
    mfxVPPParams.vpp.In.FrameRateExtN = 30;
    mfxVPPParams.vpp.In.FrameRateExtD = 1;
    mfxVPPParams.vpp.In.Width         = mfxVPPParams.vpp.In.CropW;
    mfxVPPParams.vpp.In.Height        = mfxVPPParams.vpp.In.CropH;
    // Output data
    mfxVPPParams.vpp.Out       = mfxVPPParams.vpp.In;
    mfxVPPParams.vpp.In.FourCC = MFX_FOURCC_BGRA;
    mfxVPPParams.IOPattern     = MFX_IOPATTERN_IN_SYSTEM_MEMORY | MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

    sts = MFXVideoVPP_Init(session, &mfxVPPParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoVPP_Reset(session, &mfxVPPParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(VPPReset, InvalidParamsInReturnsInvalidVideoParam) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxVPPParams = { 0 };

    // Input data
    mfxVPPParams.vpp.In.FourCC        = MFX_FOURCC_I420;
    mfxVPPParams.vpp.In.ChromaFormat  = MFX_CHROMAFORMAT_YUV420;
    mfxVPPParams.vpp.In.CropW         = 128;
    mfxVPPParams.vpp.In.CropH         = 96;
    mfxVPPParams.vpp.In.FrameRateExtN = 30;
    mfxVPPParams.vpp.In.FrameRateExtD = 1;
    mfxVPPParams.vpp.In.Width         = mfxVPPParams.vpp.In.CropW;
    mfxVPPParams.vpp.In.Height        = mfxVPPParams.vpp.In.CropH;
    // Output data
    mfxVPPParams.vpp.Out       = mfxVPPParams.vpp.In;
    mfxVPPParams.vpp.In.FourCC = MFX_FOURCC_BGRA;
    mfxVPPParams.IOPattern     = MFX_IOPATTERN_IN_SYSTEM_MEMORY | MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

    sts = MFXVideoVPP_Init(session, &mfxVPPParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVPPParams.IOPattern = MFX_IOPATTERN_IN_SYSTEM_MEMORY | MFX_IOPATTERN_OUT_VIDEO_MEMORY;

    sts = MFXVideoVPP_Reset(session, &mfxVPPParams);
    ASSERT_EQ(sts, MFX_ERR_INCOMPATIBLE_VIDEO_PARAM);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(VPPReset, NullParamsInReturnsErrNull) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoVPP_Reset(session, nullptr);
    ASSERT_EQ(sts, MFX_ERR_NULL_PTR);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(VPPReset, NullSessionInReturnsInvalidHandle) {
    mfxStatus sts = MFXVideoVPP_Reset(0, nullptr);
    ASSERT_EQ(sts, MFX_ERR_INVALID_HANDLE);
}

TEST(VPPReset, DISABLED_UninitializedVPPReturnsErrNotInitialized) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxVPPParams = { 0 };

    // Input data
    mfxVPPParams.vpp.In.FourCC        = MFX_FOURCC_I420;
    mfxVPPParams.vpp.In.CropW         = 128;
    mfxVPPParams.vpp.In.CropH         = 96;
    mfxVPPParams.vpp.In.FrameRateExtN = 30;
    mfxVPPParams.vpp.In.FrameRateExtD = 1;
    mfxVPPParams.vpp.In.Width         = mfxVPPParams.vpp.In.CropW;
    mfxVPPParams.vpp.In.Height        = mfxVPPParams.vpp.In.CropH;
    // Output data
    mfxVPPParams.vpp.Out       = mfxVPPParams.vpp.In;
    mfxVPPParams.vpp.In.FourCC = MFX_FOURCC_BGRA;
    mfxVPPParams.IOPattern     = MFX_IOPATTERN_IN_SYSTEM_MEMORY | MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

    sts = MFXVideoVPP_Reset(session, &mfxVPPParams);
    ASSERT_EQ(sts, MFX_ERR_NOT_INITIALIZED);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}
/* Close overview

Terminates current operation and de-allocates any internal memory/structures.
MFX_ERR_NONE  The function completed successfully.

*/

TEST(EncodeClose, InitializedEncodeReturnsErrNone) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxEncParams               = { 0 };
    mfxEncParams.mfx.CodecId                 = MFX_CODEC_JPEG;
    mfxEncParams.mfx.FrameInfo.FourCC        = MFX_FOURCC_I420;
    mfxEncParams.mfx.FrameInfo.ChromaFormat  = MFX_CHROMAFORMAT_YUV420;
    mfxEncParams.mfx.FrameInfo.CropW         = 128;
    mfxEncParams.mfx.FrameInfo.CropH         = 96;
    mfxEncParams.mfx.FrameInfo.Width         = 128;
    mfxEncParams.mfx.FrameInfo.Height        = 96;
    mfxEncParams.mfx.FrameInfo.FrameRateExtN = 30;
    mfxEncParams.mfx.FrameInfo.FrameRateExtD = 1;
    mfxEncParams.IOPattern                   = MFX_IOPATTERN_IN_SYSTEM_MEMORY;

    sts = MFXVideoENCODE_Init(session, &mfxEncParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoENCODE_Close(session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXClose(session);
    ASSERT_EQ(sts, MFX_ERR_NONE);
}

TEST(EncodeClose, UninitializedEncodeReturnsErrNotInitialized) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoENCODE_Close(session);
    ASSERT_EQ(sts, MFX_ERR_NOT_INITIALIZED);

    sts = MFXClose(session);
    ASSERT_EQ(sts, MFX_ERR_NONE);
}

TEST(EncodeClose, NullSessionInReturnsInvalidHandle) {
    mfxStatus sts = MFXVideoENCODE_Close(0);
    ASSERT_EQ(sts, MFX_ERR_INVALID_HANDLE);
}

TEST(EncodeClose, DoubleCloseReturnsNotInitialized) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxEncParams               = { 0 };
    mfxEncParams.mfx.CodecId                 = MFX_CODEC_JPEG;
    mfxEncParams.mfx.FrameInfo.FourCC        = MFX_FOURCC_I420;
    mfxEncParams.mfx.FrameInfo.ChromaFormat  = MFX_CHROMAFORMAT_YUV420;
    mfxEncParams.mfx.FrameInfo.CropW         = 128;
    mfxEncParams.mfx.FrameInfo.CropH         = 96;
    mfxEncParams.mfx.FrameInfo.Width         = 128;
    mfxEncParams.mfx.FrameInfo.Height        = 96;
    mfxEncParams.mfx.FrameInfo.FrameRateExtN = 30;
    mfxEncParams.mfx.FrameInfo.FrameRateExtD = 1;
    mfxEncParams.IOPattern                   = MFX_IOPATTERN_IN_SYSTEM_MEMORY;

    sts = MFXVideoENCODE_Init(session, &mfxEncParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoENCODE_Close(session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoENCODE_Close(session);
    ASSERT_EQ(sts, MFX_ERR_NOT_INITIALIZED);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(DecodeClose, InitializedEncodeReturnsErrNone) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxDecParams = { 0 };
    mfxDecParams.mfx.CodecId   = MFX_CODEC_HEVC;
    mfxDecParams.IOPattern     = MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

    mfxDecParams.mfx.FrameInfo.FourCC       = MFX_FOURCC_I420;
    mfxDecParams.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
    mfxDecParams.mfx.FrameInfo.CropW        = 128;
    mfxDecParams.mfx.FrameInfo.CropH        = 96;
    mfxDecParams.mfx.FrameInfo.Width        = 128;
    mfxDecParams.mfx.FrameInfo.Height       = 96;

    sts = MFXVideoDECODE_Init(session, &mfxDecParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoDECODE_Close(session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(DecodeClose, UninitializedDecodeReturnsErrNotInitialized) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoDECODE_Close(session);
    ASSERT_EQ(sts, MFX_ERR_NOT_INITIALIZED);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(DecodeClose, NullSessionInReturnsInvalidHandle) {
    mfxStatus sts = MFXVideoDECODE_Close(0);
    ASSERT_EQ(sts, MFX_ERR_INVALID_HANDLE);
}

TEST(DecodeClose, DoubleCloseReturnsErrNotInitialized) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxDecParams = { 0 };
    mfxDecParams.mfx.CodecId   = MFX_CODEC_HEVC;
    mfxDecParams.IOPattern     = MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

    mfxDecParams.mfx.FrameInfo.FourCC       = MFX_FOURCC_I420;
    mfxDecParams.mfx.FrameInfo.ChromaFormat = MFX_CHROMAFORMAT_YUV420;
    mfxDecParams.mfx.FrameInfo.CropW        = 128;
    mfxDecParams.mfx.FrameInfo.CropH        = 96;
    mfxDecParams.mfx.FrameInfo.Width        = 128;
    mfxDecParams.mfx.FrameInfo.Height       = 96;

    sts = MFXVideoDECODE_Init(session, &mfxDecParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoDECODE_Close(session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoDECODE_Close(session);
    ASSERT_EQ(sts, MFX_ERR_NOT_INITIALIZED);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(VPPClose, InitializedVPPReturnsErrNone) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxVPPParams = { 0 };

    // Input data
    mfxVPPParams.vpp.In.FourCC        = MFX_FOURCC_I420;
    mfxVPPParams.vpp.In.ChromaFormat  = MFX_CHROMAFORMAT_YUV420;
    mfxVPPParams.vpp.In.CropW         = 128;
    mfxVPPParams.vpp.In.CropH         = 96;
    mfxVPPParams.vpp.In.FrameRateExtN = 30;
    mfxVPPParams.vpp.In.FrameRateExtD = 1;
    mfxVPPParams.vpp.In.Width         = mfxVPPParams.vpp.In.CropW;
    mfxVPPParams.vpp.In.Height        = mfxVPPParams.vpp.In.CropH;
    // Output data
    mfxVPPParams.vpp.Out       = mfxVPPParams.vpp.In;
    mfxVPPParams.vpp.In.FourCC = MFX_FOURCC_BGRA;
    mfxVPPParams.IOPattern     = MFX_IOPATTERN_IN_SYSTEM_MEMORY | MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

    sts = MFXVideoVPP_Init(session, &mfxVPPParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoVPP_Close(session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(VPPClose, DISABLED_UninitializedVPPReturnsErrNotInitialized) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoVPP_Close(session);
    ASSERT_EQ(sts, MFX_ERR_NOT_INITIALIZED);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}

TEST(VPPClose, NullSessionInReturnsInvalidHandle) {
    mfxStatus sts = MFXVideoVPP_Close(0);
    ASSERT_EQ(sts, MFX_ERR_INVALID_HANDLE);
}

TEST(VPPClose, DISABLED_DoubleCloseReturnsErrNotInitialized) {
    mfxVersion ver = {};
    mfxSession session;
    mfxStatus sts = MFXInit(MFX_IMPL_SOFTWARE, &ver, &session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    mfxVideoParam mfxVPPParams = { 0 };

    // Input data
    mfxVPPParams.vpp.In.FourCC        = MFX_FOURCC_I420;
    mfxVPPParams.vpp.In.CropW         = 128;
    mfxVPPParams.vpp.In.CropH         = 96;
    mfxVPPParams.vpp.In.FrameRateExtN = 30;
    mfxVPPParams.vpp.In.FrameRateExtD = 1;
    mfxVPPParams.vpp.In.Width         = mfxVPPParams.vpp.In.CropW;
    mfxVPPParams.vpp.In.Height        = mfxVPPParams.vpp.In.CropH;
    // Output data
    mfxVPPParams.vpp.Out       = mfxVPPParams.vpp.In;
    mfxVPPParams.vpp.In.FourCC = MFX_FOURCC_BGRA;
    mfxVPPParams.IOPattern     = MFX_IOPATTERN_IN_SYSTEM_MEMORY | MFX_IOPATTERN_OUT_SYSTEM_MEMORY;

    sts = MFXVideoVPP_Init(session, &mfxVPPParams);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoVPP_Close(session);
    ASSERT_EQ(sts, MFX_ERR_NONE);

    sts = MFXVideoVPP_Close(session);
    ASSERT_EQ(sts, MFX_ERR_NOT_INITIALIZED);

    sts = MFXClose(session);
    EXPECT_EQ(sts, MFX_ERR_NONE);
}