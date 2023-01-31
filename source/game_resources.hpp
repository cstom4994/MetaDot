// Copyright(c) 2022-2023, KaoruXun All rights reserved.

#ifndef _METADOT_GAMERESOURCES_HPP_
#define _METADOT_GAMERESOURCES_HPP_

#include "core/core.h"
#include "sdl_wrapper.h"

typedef struct Texture {
    C_Surface *surface;
} Texture;

typedef struct TexturePack {
    Texture *testTexture;
    Texture *dirt1Texture;
    Texture *stone1Texture;
    Texture *smoothStone;
    Texture *cobbleStone;
    Texture *flatCobbleStone;
    Texture *smoothDirt;
    Texture *cobbleDirt;
    Texture *flatCobbleDirt;
    Texture *softDirt;
    Texture *cloud;
    Texture *gold;
    Texture *goldMolten;
    Texture *goldSolid;
    Texture *iron;
    Texture *obsidian;
    Texture *caveBG;
    Texture *testAse;
} TexturePack;

void InitTexture(TexturePack *tex);
void EndTexture(TexturePack *tex);

Texture *Eng_CreateTexture(C_Surface *surface);
void Eng_DestroyTexture(Texture *tex);
Texture *LoadTexture(const char *path);
Texture *LoadTextureInternal(const char *path, U32 pixelFormat);
C_Surface *ScaleSurface(C_Surface *src, F32 x, F32 y);
Texture *LoadAsepriteTexture(const char *path);
Texture *Eng_LoadTextureData(const char *path);

#endif