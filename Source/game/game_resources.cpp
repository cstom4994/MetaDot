// Copyright(c) 2022, KaoruXun All rights reserved.

#include "game_resources.hpp"
#include "core/core.hpp"
#include "core/debug_impl.hpp"
#include "core/global.hpp"
#include "engine/memory.hpp"
#include "engine/renderer/renderer_gpu.h"
#include "engine/sdl_wrapper.h"
#include "engine/scripting.hpp"
#include "game/filesystem.hpp"

#include "libs/Ase_Loader.h"
#include "libs/external/stb_image.h"
#include "lua_wrapper.hpp"
#include "scripting.hpp"
#include <string>

C_Surface *Textures::testTexture = nullptr;
C_Surface *Textures::dirt1Texture = nullptr;
C_Surface *Textures::stone1Texture = nullptr;
C_Surface *Textures::smoothStone = nullptr;
C_Surface *Textures::cobbleStone = nullptr;
C_Surface *Textures::flatCobbleStone = nullptr;
C_Surface *Textures::smoothDirt = nullptr;
C_Surface *Textures::cobbleDirt = nullptr;
C_Surface *Textures::flatCobbleDirt = nullptr;
C_Surface *Textures::softDirt = nullptr;
C_Surface *Textures::cloud = nullptr;
C_Surface *Textures::gold = nullptr;
C_Surface *Textures::goldMolten = nullptr;
C_Surface *Textures::goldSolid = nullptr;
C_Surface *Textures::iron = nullptr;
C_Surface *Textures::obsidian = nullptr;
C_Surface *Textures::caveBG = nullptr;
C_Surface *Textures::testAse = nullptr;

void Textures::Init() {
    testTexture = Textures::LoadTexture("data/assets/textures/test.png");
    dirt1Texture = Textures::LoadTexture("data/assets/textures/testDirt.png");
    stone1Texture = Textures::LoadTexture("data/assets/textures/testStone.png");
    smoothStone = Textures::LoadTexture("data/assets/textures/smooth_stone_128x.png");
    cobbleStone = Textures::LoadTexture("data/assets/textures/cobble_stone_128x.png");
    flatCobbleStone = Textures::LoadTexture("data/assets/textures/flat_cobble_stone_128x.png");
    smoothDirt = Textures::LoadTexture("data/assets/textures/smooth_dirt_128x.png");
    cobbleDirt = Textures::LoadTexture("data/assets/textures/cobble_dirt_128x.png");
    flatCobbleDirt = Textures::LoadTexture("data/assets/textures/flat_cobble_dirt_128x.png");
    softDirt = Textures::LoadTexture("data/assets/textures/soft_dirt.png");
    cloud = Textures::LoadTexture("data/assets/textures/cloud.png");
    gold = Textures::LoadTexture("data/assets/textures/gold.png");
    goldMolten = Textures::LoadTexture("data/assets/textures/moltenGold.png");
    goldSolid = Textures::LoadTexture("data/assets/textures/solidGold.png");
    iron = Textures::LoadTexture("data/assets/textures/iron.png");
    obsidian = Textures::LoadTexture("data/assets/textures/obsidian.png");
    caveBG = Textures::LoadTexture("data/assets/backgrounds/testCave.png");

    // testAse = Textures::loadAseprite("data/assets/textures/tests/3.0_one_slice.ase");
}

C_Surface *Textures::LoadTexture(std::string path) {
    return LoadTexture(path, SDL_PIXELFORMAT_ARGB8888);
}

C_Surface *Textures::LoadTexture(std::string path, U32 pixelFormat) {

    // https://wiki.libsdl.org/SDL_CreateRGBSurfaceFrom

    // the color format you request stb_image to output,
    // use STBI_rgb if you don't want/need the alpha channel
    int req_format = STBI_rgb_alpha;
    int width, height, orig_format;
    unsigned char *data =
            stbi_load(METADOT_RESLOC_STR(path), &width, &height, &orig_format, req_format);
    if (data == NULL) {
        std::cout << "Loading image failed: " << stbi_failure_reason()
                  << " ::" << METADOT_RESLOC_STR(path) << std::endl;
    }

    // Set up the pixel format color masks for RGB(A) byte arrays.
    // Only STBI_rgb (3) and STBI_rgb_alpha (4) are supported here!
    U32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    int shift = (req_format == STBI_rgb) ? 8 : 0;
    rmask = 0xff000000 >> shift;
    gmask = 0x00ff0000 >> shift;
    bmask = 0x0000ff00 >> shift;
    amask = 0x000000ff >> shift;
#else// little endian, like x86
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = (req_format == STBI_rgb) ? 0 : 0xff000000;
#endif

    int depth, pitch;
    if (req_format == STBI_rgb) {
        depth = 24;
        pitch = 3 * width;// 3 bytes per pixel * pixels per row
    } else {              // STBI_rgb_alpha (RGBA)
        depth = 32;
        pitch = 4 * width;
    }

    C_Surface *loadedSurface = SDL_CreateRGBSurfaceFrom((void *) data, width, height, depth, pitch,
                                                        rmask, gmask, bmask, amask);

    //stbi_image_free(data);

    METADOT_ASSERT_E(loadedSurface);

    return loadedSurface;
}

C_Surface *Textures::loadAseprite(std::string path) {

    Ase_Output *ase = Ase_Load(METADOT_RESLOC(path));

    if (NULL == ase) return nullptr;

    SDL_PixelFormatEnum pixel_format;
    if (ase->bpp == 1) {
        pixel_format = SDL_PIXELFORMAT_INDEX8;
    } else if (ase->bpp == 4) {
        pixel_format = SDL_PIXELFORMAT_RGBA32;
    } else {
        METADOT_ERROR("Test %d BPP not supported!", ase->bpp);
    }

    C_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(
            ase->pixels, ase->frame_width * ase->num_frames, ase->frame_height, ase->bpp * 8,
            ase->bpp * ase->frame_width * ase->num_frames, pixel_format);
    if (!surface) METADOT_ERROR("Surface could not be created!, %s", SDL_GetError());
    SDL_SetPaletteColors(surface->format->palette, (SDL_Color *) &ase->palette.entries, 0,
                         ase->palette.num_entries);
    SDL_SetColorKey(surface, SDL_TRUE, ase->palette.color_key);

    return surface;
}

C_Surface *Textures::scaleTexture(C_Surface *src, float x, float y) {
    C_Surface *dest = SDL_CreateRGBSurface(
            src->flags, src->w * x, src->h * y, src->format->BitsPerPixel, src->format->Rmask,
            src->format->Gmask, src->format->Bmask, src->format->Amask);

    C_Rect *srcR = nullptr;
    METADOT_NEW(C, srcR, C_Rect);
    srcR->w = src->w;
    srcR->h = src->h;

    C_Rect *dstR = nullptr;
    METADOT_NEW(C, dstR, C_Rect);
    dstR->w = dest->w;
    dstR->h = dest->h;

    SDL_FillRect(dest, dstR, 0x00000000);

    SDL_SetSurfaceBlendMode(
            src,
            SDL_BLENDMODE_NONE);// override instead of overlap (prevents transparent things darkening)

    SDL_BlitScaled(src, srcR, dest, dstR);

    METADOT_DELETE(C, srcR, C_Rect);
    METADOT_DELETE(C, dstR, C_Rect);

    return dest;
}

void I18N::Init() {
    auto L = global.scripts->LuaRuntime;
    METADOT_ASSERT(L, "Can't load I18N when luacore is invaild");

    Load("zh");
}

void I18N::Load(std::string lang) {
    (*global.scripts->LuaRuntime->GetWrapper())["setlocale"](lang);
}

std::string I18N::Get(std::string text) {
    return (*global.scripts->LuaRuntime->GetWrapper())["translate"](text);
}