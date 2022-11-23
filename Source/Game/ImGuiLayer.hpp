// Copyright(c) 2022, KaoruXun All rights reserved.

#ifndef _METADOT_IMGUILAYER_HPP_
#define _METADOT_IMGUILAYER_HPP_

#include "Engine/AudioEngine/AudioEngine.h"
#include "Engine/Platforms/SDLWrapper.hpp"
#include "Engine/UserInterface/IMGUI/ImGuiBase.hpp"
#include "Game/DebugImpl.hpp"
#include "Libs/ImGui/TextEditor.h"

class Game;
class Material;
class WorldMeta;

namespace MetaEngine {

    enum ImGuiWindowTags {

        UI_None = 0,
        UI_MainMenu = 1 << 0,
        UI_GCManager = 1 << 1,
    };

    class ImGuiLayer {
    private:
        struct ImGuiWin
        {
            std::string name;
            bool *opened;
        };

        std::vector<ImGuiWin> m_wins;

        C_Window *window;
        void *gl_context;

        ImGuiContext *m_imgui = nullptr;

        TextEditor editor;
        const char *fileToEdit = "data/lua/vec.lua";

    public:
        ImGuiLayer();
        ~ImGuiLayer() = default;
        void Init(C_Window *window, void *gl_context);
        void onDetach();
        void begin();
        void end();
        void Render(Game *game);
        void registerWindow(std::string_view windowName, bool *opened);
        ImVec2 GetNextWindowsPos(ImGuiWindowTags tag, ImVec2 pos);

        ImGuiContext *getImGuiCtx() {
            METADOT_ASSERT(m_imgui, "Miss Fucking ImGuiContext");
            return m_imgui;
        }
    };
}// namespace MetaEngine


namespace MetaEngine {
    void GameUI_Draw(Game *game);

    namespace InternalGUI {

        class DebugUI {
        public:
            static void Draw(Game *game);
        };

        class DebugCheatsUI {
        public:
            static bool visible;
            static std::vector<METAENGINE_Render_Image *> images;

            static void Setup();

            static void Draw(Game *game);
        };

        class DebugDrawUI {
        public:
            static bool visible;
            static int selIndex;
            static std::vector<METAENGINE_Render_Image *> images;

            static Material *selectedMaterial;
            static uint8 brushSize;

            static void Setup();

            static void Draw(Game *game);
        };

        class MainMenuUI {
        public:
            static bool visible;

            static int state;

            static bool setup;

            static METAENGINE_Render_Image *title;

            static bool connectButtonEnabled;

            static ImVec2 pos;

            static std::vector<std::tuple<std::string, WorldMeta>> worlds;

            static long long lastRefresh;

            static void RefreshWorlds(Game *game);

            static void Setup();

            static void Draw(Game *game);

            static void DrawMainMenu(Game *game);

            static void DrawSingleplayer(Game *game);
            static void DrawMultiplayer(Game *game);

            static void DrawCreateWorld(Game *game);

            static void DrawOptions(Game *game);
        };

        class IngameUI {
        public:
            static bool visible;

            static int state;

            static bool setup;

            static void Setup();

            static void Draw(Game *game);

            static void DrawIngame(Game *game);

            static void DrawOptions(Game *game);
        };

        class CreateWorldUI {
        public:
            static bool setup;
            static char worldNameBuf[32];

            static METAENGINE_Render_Image *materialTestWorld;
            static METAENGINE_Render_Image *defaultWorld;

            static bool createWorldButtonEnabled;

            static std::string worldFolderLabel;

            static int selIndex;

            static void Setup();
            static void Reset(Game *game);

            static void Draw(Game *game);

            static void inputChanged(std::string text, Game *game);
        };

        class OptionsUI {
            // static std::map<std::string, FMOD::Studio::Bus *> busMap;

        public:
            static int item_current_idx;
            static bool vsync;
            static bool minimizeOnFocus;

            static void Draw(Game *game);
            static void DrawGeneral(Game *game);
            static void DrawVideo(Game *game);
            static void DrawAudio(Game *game);
            static void DrawInput(Game *game);
        };

    }// namespace InternalGUI
}// namespace MetaEngine

#endif