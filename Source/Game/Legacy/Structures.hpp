// Copyright(c) 2022, KaoruXun All rights reserved.


#ifndef _METADOT_STRUCTURES_HPP_
#define _METADOT_STRUCTURES_HPP_

#include <iostream>


#ifndef INC_Tiles
#include "Materials.hpp"
#endif// !INC_Tiles


#define INC_Structure

class Structure {
public:
    MaterialInstance *tiles;
    int w;
    int h;

    Structure(int w, int h, MaterialInstance *tiles);
    Structure(C_Surface *texture, Material templ);
    Structure() = default;
};

class World;

class Structures {
public:
    static Structure makeTree(World world, int x, int y);
    static Structure makeTree1(World world, int x, int y);
};

class PlacedStructure {
public:
    Structure base;
    int x;
    int y;

    PlacedStructure(Structure base, int x, int y);
    //PlacedStructure(const PlacedStructure &p2) { this->base = Structure(base); this->x = x; this->y = y; }
};

#endif