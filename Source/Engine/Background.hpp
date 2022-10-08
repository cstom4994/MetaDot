// Copyright(c) 2019 - 2022, KaoruXun All rights reserved.


#include "Engine/render/renderer_gpu.h"

#include <vector>
#include <unordered_map>
#include <string>

class BackgroundLayer {
public:
    std::vector<SDL_Surface*> surface;
    std::vector<METAENGINE_Render_Image*> texture;
    float parralaxX;
    float parralaxY;
    float moveX;
    float moveY;
    BackgroundLayer(SDL_Surface* texture, float parallaxX, float parallaxY, float moveX, float moveY);
    void init();
};

class Background {
public:
    Uint32 solid;
    std::vector<BackgroundLayer> layers;
    Background(Uint32 solid, std::vector<BackgroundLayer> layers);
    void init();
};

class Backgrounds {
private:
    std::unordered_map<std::string, Background*> m_backgrounds;
public:
    Backgrounds() = default;
    ~Backgrounds();

    void Push(std::string name, Background* bg);
    Background* Get(std::string name);
    //static Background TEST_OVERWORLD;
};
