::------------------------------------------------------------------------------
:: Copyright (C) Intel Corporation
::
:: SPDX-License-Identifier: MIT
::------------------------------------------------------------------------------
:: start of boilerplate to switch to project root ------------------------------
@echo off
SETLOCAL EnableDelayedExpansion
FOR /D %%i IN ("%~dp0\..") DO (
	set PROJ_DIR=%%~fi
)
cd %PROJ_DIR%
:: Read options -----------------------------------------------------------
SET USE_GPL=no
SET BUILD_MODE=Release

:Loop
IF "%~1"=="" GOTO Continue
  IF "%~1"=="gpl" (
    SET USE_GPL=yes
  )
  IF "%~1"=="debug" (
    SET BUILD_MODE=Debug
  )
SHIFT
GOTO Loop
:Continue

:: start of commands -----------------------------------------------------------
set build_dir=%PROJ_DIR%\_extbuild
set DEFAULT_MSYS_ROOT=C:\tools\msys64
set DEFAULT_VPL_BUILD_DEPENDENCIES=%PROJ_DIR%\_deps

if not defined VPL_BUILD_DEPENDENCIES (
  set VPL_BUILD_DEPENDENCIES=%DEFAULT_VPL_BUILD_DEPENDENCIES%
)
if defined MSYS_ROOT (
  echo MSYS_ROOT found: %MSYS_ROOT%
) else (
  set MSYS_ROOT=%DEFAULT_MSYS_ROOT%
  call echo MSYS_ROOT not found, assuming !MSYS_ROOT!
)
set GITPATH=%PATH%
set MINGWPATH=%MSYS_ROOT%\mingw64\bin;%MSYS_ROOT%\usr\local\bin;^
%MSYS_ROOT%\usr\bin;%MSYS_ROOT%\bin;c:\Windows\System32;c:\Windows;^
c:\Windows\System32\Wbem;c:\Windows\System32\WindowsPowerShell\v1.0\;^
%MSYS_ROOT%\usr\bin\site_perl;%MSYS_ROOT%\usr\bin\vendor_perl;^
%MSYS_ROOT%\usr\bin\core_perl

:: MSYS git does not play with other gits, so use users version if present
where git >NUL
if ERRORLEVEL 1 set GITPATH=%MINGWPATH%

set install_dir=%VPL_BUILD_DEPENDENCIES%

rd /s /q %build_dir% 2>NUL
rd /s /q %VPL_BUILD_DEPENDENCIES% 2>NUL

:: build FFmpeg with SVT-HEVC and SVT-AV1
md %build_dir% 2>NUL

:: checkout SVT-HEVC
cd %build_dir%
git clone --depth=1 -b v1.5.0 https://github.com/OpenVisualCloud/SVT-HEVC.git && cd SVT-HEVC
if "%BUILD_MODE%"=="Debug" (
  powershell -Command "(gc Source\Lib\Codec\EbMalloc.h) -replace '#define DEBUG_MEMORY_USAGE', '#undef DEBUG_MEMORY_USAGE' | Out-File -encoding utf8 Source\Lib\Codec\EbMalloc.h"
)

:: checkout SVT-AV1
cd %build_dir%
git clone --depth=1 -b v0.8.4 https://github.com/AOMediaCodec/SVT-AV1 && cd SVT-AV1
if "%BUILD_MODE%"=="Debug" (
  powershell -Command "(gc Source\Lib\Common\Codec\EbMalloc.h) -replace '#define DEBUG_MEMORY_USAGE', '#undef DEBUG_MEMORY_USAGE' | Out-File -encoding utf8 Source\Lib\Common\Codec\EbMalloc.h"
)

if "%USE_GPL%"=="yes" (
  :: checkout x264
  cd %build_dir%
  git clone --depth 1 -b stable https://code.videolan.org/videolan/x264.git && cd x264
)

:: checkout dav1d
cd %build_dir%
git clone --depth=1 -b 0.7.0 https://code.videolan.org/videolan/dav1d.git && cd dav1d

:: set path for build
set PATH=%MINGWPATH%

:: SVT-HEVC build and install
cd %build_dir%\SVT-HEVC
:::: to turn off log on screen, modify header file
echo powershell -Command "(gc Source\Lib\Codec\EbDefinitions.h) -replace '#define LIB_PRINTF_ENABLE                1', '#define LIB_PRINTF_ENABLE                0' | Out-File -encoding utf8 Source\Lib\Codec\EbDefinitions.h"
mkdir release && cd release
cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=%BUILD_MODE% ^
-DCMAKE_INSTALL_PREFIX=%install_dir%\ -DBUILD_SHARED_LIBS=off -DBUILD_APP=off
if ERRORLEVEL 1 exit /b 1
cmake --build . --target install -j %NUMBER_OF_PROCESSORS%

:: SVT-AV1 build and install
:: -DSVT_LOG_QUIET=1 is for turning off logs
cd %build_dir%\SVT-AV1
mkdir release && cd release
cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=%BUILD_MODE% ^
-DCMAKE_INSTALL_PREFIX=%install_dir%\ -DBUILD_SHARED_LIBS=off -DBUILD_APPS=off ^
-DCMAKE_C_FLAGS="$(CMAKE_C_FLAGS) -DSVT_LOG_QUIET=1"
if ERRORLEVEL 1 exit /b 1
cmake --build . --target install -j %NUMBER_OF_PROCESSORS%

if "%USE_GPL%"=="yes" (
  :: x264 build and install
  cd %build_dir%\x264
  bash -c './configure --prefix=${install_dir} --enable-static --enable-pic'
  make -j %NUMBER_OF_PROCESSORS% && make install
)

:: dav1d build and install
cd %build_dir%\dav1d
meson build --prefix %install_dir%\ --libdir %install_dir%\lib --buildtype release --default-library=static -Denable_avx512=false
ninja -C build && cd build
ninja install

cd %build_dir%
:: set path for git
set PATH=%GITPATH%

:: checkout ffmpeg
git clone --depth=1 -b n4.3.1 https://github.com/FFmpeg/FFmpeg ffmpeg && cd ffmpeg

:: patch of SVT-HEVC and SVT-AV1 ffmpeg plugin
git config user.email "bootstrap@localhost"
git config user.name "bootstrap"
set patch=0001-lavc-svt_hevc-add-libsvt-hevc-encoder-wrapper.patch
git am %build_dir%\SVT-HEVC\ffmpeg_plugin\%patch%
if ERRORLEVEL 1 exit /b 1
set patch=0001-Add-ability-for-ffmpeg-to-run-svt-av1.patch
git am %build_dir%\SVT-AV1\ffmpeg_plugin\%patch%
if ERRORLEVEL 1 exit /b 1

if "%USE_GPL%"=="yes" (
  set enable_x264options=--enable-gpl --enable-libx264 --enable-encoder=libx264
) else (
  set enable_x264options=
)

:: set path for build
set PATH=%MINGWPATH%

set PKG_CONFIG_PATH=%install_dir%\lib\pkgconfig;%PKG_CONFIG_PATH%
set install_dir=%install_dir:\=/%
bash -c './configure ^
--extra-cflags="-fPIC" ^
--extra-ldflags="-fPIC" ^
--arch=x86_64 ^
--target-os=mingw64 ^
--prefix=${install_dir} ^
--disable-shared ^
--enable-static ^
--enable-pic ^
--disable-network ^
--disable-everything ^
--disable-doc ^
--disable-manpages ^
--disable-hwaccels ^
--disable-appkit ^
--disable-alsa ^
--disable-avfoundation ^
--disable-iconv ^
--disable-lzma ^
--disable-sdl2 ^
--disable-securetransport ^
--disable-xlib ^
--disable-zlib ^
--disable-amf ^
--disable-audiotoolbox ^
--disable-cuvid ^
--disable-d3d11va ^
--disable-dxva2 ^
--disable-nvdec ^
--disable-nvenc ^
--disable-v4l2-m2m ^
--disable-videotoolbox ^
${enable_x264options}  ^
--enable-indev=lavfi ^
--enable-protocol=file ^
--enable-bsf=h264_mp4toannexb ^
--enable-bsf=hevc_mp4toannexb ^
--enable-bsf=mjpeg2jpeg  ^
--enable-bsf=mjpega_dump_header  ^
--enable-decoder=rawvideo ^
--enable-encoder=rawvideo ^
--enable-demuxer=rawvideo ^
--enable-muxer=rawvideo ^
--enable-demuxer=mjpeg  ^
--enable-muxer=null ^
--enable-decoder=wrapped_avframe ^
--enable-encoder=wrapped_avframe ^
--enable-muxer=h264 ^
--enable-muxer=mpeg2video ^
--enable-muxer=mjpeg ^
--enable-muxer=hevc ^
--enable-muxer=ivf ^
--enable-muxer=image2   ^
--enable-demuxer=image2  ^
--enable-filter=testsrc ^
--enable-filter=testsrc2 ^
--enable-filter=rgbtestsrc ^
--enable-filter=yuvtestsrc ^
--enable-demuxer=h264 ^
--enable-parser=h264 ^
--enable-decoder=h264 ^
--enable-demuxer=hevc ^
--enable-decoder=hevc ^
--enable-parser=hevc ^
--enable-demuxer=ivf ^
--enable-parser=av1 ^
--enable-parser=mjpeg  ^
--enable-decoder=mpeg2video ^
--enable-encoder=mpeg2video ^
--enable-decoder=mjpeg ^
--enable-encoder=mjpeg ^
--enable-filter=scale ^
--enable-filter=crop   ^
--enable-filter=overlay   ^
--enable-filter=split   ^
--enable-filter=drawbox   ^
--enable-filter=psnr ^
--enable-filter=ssim ^
--enable-filter=select ^
--enable-filter=concat ^
--enable-libsvthevc ^
--enable-encoder=libsvt_hevc ^
--enable-libsvtav1 ^
--enable-encoder=libsvt_av1 ^
--enable-libdav1d ^
--enable-decoder=libdav1d'

make -j %NUMBER_OF_PROCESSORS% && make install

:: export build dependency environment
(
endlocal
  if not defined VPL_BUILD_DEPENDENCIES (
     set VPL_BUILD_DEPENDENCIES=%DEFAULT_VPL_BUILD_DEPENDENCIES%
  )
)
