// Copyright(c) 2022-2023, KaoruXun All rights reserved.

#ifndef _METADOT_UI_HPP_
#define _METADOT_UI_HPP_

#include <functional>
#include <map>

#include "core/core.h"
#include "game_basic.hpp"
#include "game_resources.hpp"
#include "libs/parallel_hashmap/btree.h"
#include "meta/meta.hpp"
#include "renderer/renderer_gpu.h"
#include "renderer/renderer_opengl.h"
#include "ui/imgui/imgui_core.hpp"
#include "ui/imgui/imgui_impl.hpp"
#include "ui/ui_layout.h"

typedef enum elementType { coloredRectangle, texturedRectangle, textElement, lineElement, buttonElement, progressBarElement, windowElement } ElementType;

typedef struct UIElementState_Button {
    METAENGINE_Color hot_color;
    void (*func)(void);
} UI_Button;

typedef struct UIElementState_Window {
    layout_id layout_id;
} UI_Window;

typedef struct UIElementState_ProgressBar {
    U8 bar_type;
    F32 bar_current;
    F32 bar_limit;
    METAENGINE_Color bar_color;
    METAENGINE_Color bar_text_color;
} UI_ProgressBar;

typedef union UIElementClass {
    UI_Button button;
    UI_Window window;
    UI_ProgressBar progressbar;
} UIElementClass;

typedef struct UIMovable {
    bool moving = false;
    int ox = 0, oy = 0;
    int mx = 0, my = 0;
} UIMovable;

typedef struct UIResizable {
    bool resizable = false;
    bool resizing = false;
    int mx = 0, my = 0;
    int ow = 0, oh = 0;
    int mw = 0, mh = 0;
} UIResizable;

typedef struct UIElement {
    ElementType type;

    bool visible = true;

    UIElement* parent = nullptr;

    UIMovable movable;
    UIResizable resizable;

    int x, y, w, h;

    U8 state;
    METAENGINE_Color color;
    Texture* texture;
    std::string text;

    UIElementClass cclass;

    int textW, textH;

    float lineThickness;

} UIElement;

// static_assert(sizeof(UIElement) == 176);

typedef struct UIData {
    ImGuiCore* imguiCore = nullptr;

    // Layout caculate context
    layout_context layoutContext;

    // std::map<std::string, UIElement> elementLists = {};
    phmap::btree_map<std::string, UIElement*> elementLists = {};
} UIData;

class UISystem : public IGameSystem {
public:
    REGISTER_SYSTEM(UISystem)

    void Create() override;
    void Destory() override;
    void Reload() override;
    void RegisterLua(LuaWrapper::State& s_lua) override;

    void UIRendererInit();
    void UIRendererPostUpdate();
    void UIRendererUpdate();
    void UIRendererDraw();
    void UIRendererDrawImGui();
    void UIRendererFree();

    bool UIIsMouseOnControls();

    void DrawPoint(metadot_vec3 pos, float size, Texture* texture, U8 r, U8 g, U8 b);
    void DrawLine(metadot_vec3 min, metadot_vec3 max, float thickness, U8 r, U8 g, U8 b);
};

#endif