﻿

#include "ImGuiHelper.hpp"

#include "Engine/Core.hpp"

#include "Engine/lib/dirent.h"

#ifdef WIN32
#define PATH_SEP '\\'
#ifndef PATH_MAX
#define PATH_MAX 260
#endif
#elif defined(LINUX) or defined(APPLE)
#include <dirent.h>
#include <sys/types.h>
#define PATH_SEP '/'
#endif

// widget
// button widget use for compose path
#define IMGUI_PATH_BUTTON ImGui::Button
// standar button
#define IMGUI_BUTTON ImGui::Button

// locales string
#define createDirButtonString "+"
#define okButtonString " OK"
#define cancelButtonString " Cancel"
#define resetButtonString "R"
#define drivesButtonString "Drives"
#define searchString "Search"
#define dirEntryString "[DIR] "
#define linkEntryString "[LINK] "
#define fileEntryString "[FILE] "
#define fileNameString "File Name : "
#define buttonResetSearchString "Reset search"
#define buttonDriveString "Drives"
#define buttonResetPathString "Reset to current directory"
#define buttonCreateDirString "Create Directory"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <utility>

ImMarkdown::ImMarkdown() {

    m_table_last_pos = ImVec2(0, 0);

    m_md.abi_version = 0;
    m_md.syntax = nullptr;
    m_md.debug_log = nullptr;

    m_md.flags = MD_FLAG_TABLES | MD_FLAG_UNDERLINE | MD_FLAG_STRIKETHROUGH;
    m_md.enter_block = [](MD_BLOCKTYPE type, void *detail, void *userdata) {
        return ((ImMarkdown *) userdata)->block(type, detail, true);
    };

    m_md.leave_block = [](MD_BLOCKTYPE type, void *detail, void *userdata) {
        return ((ImMarkdown *) userdata)->block(type, detail, false);
    };

    m_md.enter_span = [](MD_SPANTYPE type, void *detail, void *userdata) {
        return ((ImMarkdown *) userdata)->span(type, detail, true);
    };

    m_md.leave_span = [](MD_SPANTYPE type, void *detail, void *userdata) {
        return ((ImMarkdown *) userdata)->span(type, detail, false);
    };

    m_md.text = [](MD_TEXTTYPE type, const MD_CHAR *text, MD_SIZE size, void *userdata) {
        return ((ImMarkdown *) userdata)->text(type, text, text + size);
    };
}


void ImMarkdown::BLOCK_UL(const MD_BLOCK_UL_DETAIL *d, bool e) {
    if (e) {
        m_list_stack.push_back(list_info{false, d->mark, 0});
    } else {
        m_list_stack.pop_back();
        if (m_list_stack.empty()) ImGui::NewLine();
    }
}

void ImMarkdown::BLOCK_OL(const MD_BLOCK_OL_DETAIL *d, bool e) {
    if (e) {
        m_list_stack.push_back(list_info{true, d->mark_delimiter, d->start});
    } else {
        m_list_stack.pop_back();
        if (m_list_stack.empty()) ImGui::NewLine();
    }
}

void ImMarkdown::BLOCK_LI(const MD_BLOCK_LI_DETAIL *, bool e) {
    if (e) {
        ImGui::NewLine();

        list_info &nfo = m_list_stack.back();
        if (nfo.is_ol) {
            ImGui::Text("%d%c", nfo.cur_ol++, nfo.delim);
            ImGui::SameLine();
        } else {
            if (nfo.delim == '*') {
                float cx = ImGui::GetCursorPosX();
                cx -= ImGui::GetStyle().FramePadding.x * 2;
                ImGui::SetCursorPosX(cx);
                ImGui::Bullet();
            } else {
                ImGui::Text("%c", nfo.delim);
                ImGui::SameLine();
            }
        }

        ImGui::Indent();
    } else {
        ImGui::Unindent();
    }
}

void ImMarkdown::BLOCK_HR(bool e) {
    if (!e) {
        ImGui::NewLine();
        ImGui::Separator();
    }
}

void ImMarkdown::BLOCK_H(const MD_BLOCK_H_DETAIL *d, bool e) {
    if (e) {
        m_hlevel = d->level;
        ImGui::NewLine();
    } else {
        m_hlevel = 0;
    }

    set_font(e);

    if (!e) {
        if (d->level <= 2) {
            ImGui::NewLine();
            ImGui::Separator();
        }
    }
}

void ImMarkdown::BLOCK_DOC(bool) {
}

void ImMarkdown::BLOCK_QUOTE(bool) {
}
void ImMarkdown::BLOCK_CODE(const MD_BLOCK_CODE_DETAIL *, bool) {
}

void ImMarkdown::BLOCK_HTML(bool) {
}

void ImMarkdown::BLOCK_P(bool) {
    if (!m_list_stack.empty()) return;
    ImGui::NewLine();
}

void ImMarkdown::BLOCK_TABLE(const MD_BLOCK_TABLE_DETAIL *, bool e) {
    if (e) {
        m_table_row_pos.clear();
        m_table_col_pos.clear();

        m_table_last_pos = ImGui::GetCursorPos();
    } else {

        ImGui::NewLine();
        m_table_last_pos.y = ImGui::GetCursorPos().y;

        m_table_col_pos.push_back(m_table_last_pos.x);
        m_table_row_pos.push_back(m_table_last_pos.y);

        const ImVec2 wp = ImGui::GetWindowPos();
        const ImVec2 sp = ImGui::GetStyle().ItemSpacing;
        const float wx = wp.x + sp.x / 2;
        const float wy = wp.y - sp.y / 2 - ImGui::GetScrollY();

        for (int i = 0; i < m_table_col_pos.size(); ++i) {
            m_table_col_pos[i] += wx;
        }

        for (int i = 0; i < m_table_row_pos.size(); ++i) {
            m_table_row_pos[i] += wy;
        }

        const ImColor c = ImGui::GetStyle().Colors[ImGuiCol_Separator];
        ////////////////////////////////////////////////////////////////////////

        ImDrawList *dl = ImGui::GetWindowDrawList();

        const float xmin = m_table_col_pos.front();
        const float xmax = m_table_col_pos.back();
        for (int i = 0; i < m_table_row_pos.size(); ++i) {
            const float p = m_table_row_pos[i];
            dl->AddLine(ImVec2(xmin, p), ImVec2(xmax, p), c, i == 1 ? 2.0f : 1.0f);
        }

        const float ymin = m_table_row_pos.front();
        const float ymax = m_table_row_pos.back();
        for (int i = 0; i < m_table_col_pos.size(); ++i) {
            const float p = m_table_col_pos[i];
            dl->AddLine(ImVec2(p, ymin), ImVec2(p, ymax), c);
        }
    }
}

void ImMarkdown::BLOCK_THEAD(bool e) {
    m_is_table_header = e;
    set_font(e);
}

void ImMarkdown::BLOCK_TBODY(bool e) {
    m_is_table_body = e;
}

void ImMarkdown::BLOCK_TR(bool e) {
    ImGui::SetCursorPosY(m_table_last_pos.y);

    if (e) {
        m_table_next_column = 0;
        ImGui::NewLine();
        m_table_row_pos.push_back(ImGui::GetCursorPosY());
    }
}

void ImMarkdown::BLOCK_TH(const MD_BLOCK_TD_DETAIL *d, bool e) {
    BLOCK_TD(d, e);
}

void ImMarkdown::BLOCK_TD(const MD_BLOCK_TD_DETAIL *, bool e) {
    if (e) {

        if (m_table_next_column < m_table_col_pos.size()) {
            ImGui::SetCursorPosX(m_table_col_pos[m_table_next_column]);
        } else {
            m_table_col_pos.push_back(m_table_last_pos.x);
        }

        ++m_table_next_column;

        ImGui::Indent(m_table_col_pos[m_table_next_column - 1]);
        ImGui::SetCursorPos(
                ImVec2(m_table_col_pos[m_table_next_column - 1], m_table_row_pos.back()));

    } else {
        const ImVec2 p = ImGui::GetCursorPos();
        ImGui::Unindent(m_table_col_pos[m_table_next_column - 1]);
        ImGui::SetCursorPosX(p.x);
        if (p.y > m_table_last_pos.y) m_table_last_pos.y = p.y;
    }
    ImGui::TextUnformatted("");
    ImGui::SameLine();
}

////////////////////////////////////////////////////////////////////////////////
void ImMarkdown::set_href(bool e, const MD_ATTRIBUTE &src) {
    if (e) {
        m_href.assign(src.text, src.size);
    } else {
        m_href.clear();
    }
}

void ImMarkdown::set_font(bool e) {
    if (e) {
        ImGui::PushFont(get_font());
    } else {
        ImGui::PopFont();
    }
}

void ImMarkdown::set_color(bool e) {
    if (e) {
        ImGui::PushStyleColor(ImGuiCol_Text, get_color());
    } else {
        ImGui::PopStyleColor();
    }
}

void ImMarkdown::line(ImColor c, bool under) {
    ImVec2 mi = ImGui::GetItemRectMin();
    ImVec2 ma = ImGui::GetItemRectMax();

    if (!under) {
        ma.y -= ImGui::GetFontSize() / 2;
    }

    mi.y = ma.y;

    ImGui::GetWindowDrawList()->AddLine(mi, ma, c, 1.0f);
}

void ImMarkdown::SPAN_A(const MD_SPAN_A_DETAIL *d, bool e) {
    set_href(e, d->href);
    set_color(e);
}


void ImMarkdown::SPAN_EM(bool e) {
    m_is_em = e;
    set_font(e);
}

void ImMarkdown::SPAN_STRONG(bool e) {
    m_is_strong = e;
    set_font(e);
}


void ImMarkdown::SPAN_IMG(const MD_SPAN_IMG_DETAIL *d, bool e) {
    m_is_image = e;

    set_href(e, d->src);

    if (e) {

        image_info nfo;
        if (get_image(nfo)) {

            const float scale = ImGui::GetIO().FontGlobalScale;
            nfo.size.x *= scale;
            nfo.size.y *= scale;


            ImVec2 const csz = ImGui::GetContentRegionAvail();
            if (nfo.size.x > csz.x) {
                const float r = nfo.size.y / nfo.size.x;
                nfo.size.x = csz.x;
                nfo.size.y = csz.x * r;
            }

            ImGui::Image(nfo.texture_id, nfo.size, nfo.uv0, nfo.uv1, nfo.col_tint, nfo.col_border);

            if (ImGui::IsItemHovered()) {

                //if (d->title.size) {
                //	ImGui::SetTooltip("%.*s", (int)d->title.size, d->title.text);
                //}

                if (ImGui::IsMouseReleased(0)) {
                    open_url();
                }
            }
        }
    }
}

void ImMarkdown::SPAN_CODE(bool) {
}


void ImMarkdown::SPAN_LATEXMATH(bool) {
}

void ImMarkdown::SPAN_LATEXMATH_DISPLAY(bool) {
}

void ImMarkdown::SPAN_WIKILINK(const MD_SPAN_WIKILINK_DETAIL *, bool) {
}

void ImMarkdown::SPAN_U(bool e) {
    m_is_underline = e;
}

void ImMarkdown::SPAN_DEL(bool e) {
    m_is_strikethrough = e;
}

void ImMarkdown::render_text(const char *str, const char *str_end) {
    const float scale = ImGui::GetIO().FontGlobalScale;
    const ImGuiStyle &s = ImGui::GetStyle();

    while (!m_is_image && str < str_end) {

        const char *te = str_end;

        if (!m_is_table_header) {

            float wl = ImGui::GetContentRegionAvail().x;

            if (m_is_table_body) {
                wl = (m_table_next_column < m_table_col_pos.size() ? m_table_col_pos[m_table_next_column] : m_table_last_pos.x);
                wl -= ImGui::GetCursorPosX();
            }

            te = ImGui::GetFont()->CalcWordWrapPositionA(
                    scale, str, str_end, wl);

            if (te == str) ++te;
        }


        ImGui::TextUnformatted(str, te);

        if (!m_href.empty()) {

            ImVec4 c;
            if (ImGui::IsItemHovered()) {

                ImGui::SetTooltip("%s", m_href.c_str());

                c = s.Colors[ImGuiCol_ButtonHovered];
                if (ImGui::IsMouseReleased(0)) {
                    open_url();
                }
            } else {
                c = s.Colors[ImGuiCol_Button];
            }
            line(c, true);
        }
        if (m_is_underline) {
            line(s.Colors[ImGuiCol_Text], true);
        }
        if (m_is_strikethrough) {
            line(s.Colors[ImGuiCol_Text], false);
        }

        str = te;

        while (str < str_end && * str == ' ') ++str;
    }


    ImGui::SameLine(0.0f, 0.0f);
}


bool ImMarkdown::render_entity(const char *str, const char *str_end) {
    const size_t sz = str_end - str;
    if (strncmp(str, "&nbsp;", sz) == 0) {
        ImGui::TextUnformatted("");
        ImGui::SameLine();
        return true;
    }
    return false;
}

bool ImMarkdown::render_html(const char *str, const char *str_end) {
    const size_t sz = str_end - str;

    if (strncmp(str, "<br>", sz) == 0) {
        ImGui::NewLine();
        return true;
    }
    if (strncmp(str, "<hr>", sz) == 0) {
        ImGui::Separator();
        return true;
    }
    if (strncmp(str, "<u>", sz) == 0) {
        m_is_underline = true;
        return true;
    }
    if (strncmp(str, "</u>", sz) == 0) {
        m_is_underline = false;
        return true;
    }
    return false;
}


int ImMarkdown::text(MD_TEXTTYPE type, const char *str, const char *str_end) {
    switch (type) {
        case MD_TEXT_NORMAL:
            render_text(str, str_end);
            break;
        case MD_TEXT_CODE:
            render_text(str, str_end);
            break;
        case MD_TEXT_NULLCHAR:
            break;
        case MD_TEXT_BR:
            ImGui::NewLine();
            break;
        case MD_TEXT_SOFTBR:
            soft_break();
            break;
        case MD_TEXT_ENTITY:
            if (!render_entity(str, str_end)) {
                render_text(str, str_end);
            };
            break;
        case MD_TEXT_HTML:
            if (!render_html(str, str_end)) {
                render_text(str, str_end);
            }
            break;
        case MD_TEXT_LATEXMATH:
            render_text(str, str_end);
            break;
        default:
            break;
    }

    if (m_is_table_header) {
        const float x = ImGui::GetCursorPosX();
        if (x > m_table_last_pos.x) m_table_last_pos.x = x;
    }

    return 0;
}

int ImMarkdown::block(MD_BLOCKTYPE type, void *d, bool e) {
    switch (type) {
        case MD_BLOCK_DOC:
            BLOCK_DOC(e);
            break;
        case MD_BLOCK_QUOTE:
            BLOCK_QUOTE(e);
            break;
        case MD_BLOCK_UL:
            BLOCK_UL((MD_BLOCK_UL_DETAIL *) d, e);
            break;
        case MD_BLOCK_OL:
            BLOCK_OL((MD_BLOCK_OL_DETAIL *) d, e);
            break;
        case MD_BLOCK_LI:
            BLOCK_LI((MD_BLOCK_LI_DETAIL *) d, e);
            break;
        case MD_BLOCK_HR:
            BLOCK_HR(e);
            break;
        case MD_BLOCK_H:
            BLOCK_H((MD_BLOCK_H_DETAIL *) d, e);
            break;
        case MD_BLOCK_CODE:
            BLOCK_CODE((MD_BLOCK_CODE_DETAIL *) d, e);
            break;
        case MD_BLOCK_HTML:
            BLOCK_HTML(e);
            break;
        case MD_BLOCK_P:
            BLOCK_P(e);
            break;
        case MD_BLOCK_TABLE:
            BLOCK_TABLE((MD_BLOCK_TABLE_DETAIL *) d, e);
            break;
        case MD_BLOCK_THEAD:
            BLOCK_THEAD(e);
            break;
        case MD_BLOCK_TBODY:
            BLOCK_TBODY(e);
            break;
        case MD_BLOCK_TR:
            BLOCK_TR(e);
            break;
        case MD_BLOCK_TH:
            BLOCK_TH((MD_BLOCK_TD_DETAIL *) d, e);
            break;
        case MD_BLOCK_TD:
            BLOCK_TD((MD_BLOCK_TD_DETAIL *) d, e);
            break;
        default:
            assert(false);
            break;
    }

    return 0;
}

int ImMarkdown::span(MD_SPANTYPE type, void *d, bool e) {
    switch (type) {
        case MD_SPAN_EM:
            SPAN_EM(e);
            break;
        case MD_SPAN_STRONG:
            SPAN_STRONG(e);
            break;
        case MD_SPAN_A:
            SPAN_A((MD_SPAN_A_DETAIL *) d, e);
            break;
        case MD_SPAN_IMG:
            SPAN_IMG((MD_SPAN_IMG_DETAIL *) d, e);
            break;
        case MD_SPAN_CODE:
            SPAN_CODE(e);
            break;
        case MD_SPAN_DEL:
            SPAN_DEL(e);
            break;
        case MD_SPAN_LATEXMATH:
            SPAN_LATEXMATH(e);
            break;
        case MD_SPAN_LATEXMATH_DISPLAY:
            SPAN_LATEXMATH_DISPLAY(e);
            break;
        case MD_SPAN_WIKILINK:
            SPAN_WIKILINK((MD_SPAN_WIKILINK_DETAIL *) d, e);
            break;
        case MD_SPAN_U:
            SPAN_U(e);
            break;
        default:
            assert(false);
            break;
    }

    return 0;
}

int ImMarkdown::print(const std::string &text) {
    return md_parse(text.c_str(), text.size() + 1, &m_md, this);
}

////////////////////////////////////////////////////////////////////////////////

ImFont *ImMarkdown::get_font() const {
    return nullptr;//default font

    //Example:
#if 0
    if (m_is_table_header) {
        return g_font_bold;
    }

    switch (m_hlevel)
    {
    case 0:
        return m_is_strong ? g_font_bold : g_font_regular;
    case 1:
        return g_font_bold_large;
    default:
        return g_font_bold;
    }
#endif
};

ImVec4 ImMarkdown::get_color() const {
    if (!m_href.empty()) {
        return ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
    }
    return ImGui::GetStyle().Colors[ImGuiCol_Text];
}


bool ImMarkdown::get_image(image_info &nfo) const {
    nfo.texture_id = ImGui::GetIO().Fonts->TexID;
    nfo.size = {100, 50};
    nfo.uv0 = {0, 0};
    nfo.uv1 = {1, 1};
    nfo.col_tint = {1, 1, 1, 1};
    nfo.col_border = {0, 0, 0, 0};

    return true;
};

void ImMarkdown::open_url() const {
    //Example:

#if 0	
    if (!m_is_image) {
        SDL_OpenURL(m_href.c_str());
    }
    else {
        //image clicked
    }
#endif
}

void ImMarkdown::soft_break() {
    //Example:
#if 0
    ImGui::NewLine();
#endif
}


static std::string s_fs_root = std::string(1u, PATH_SEP);

/* Alphabetical sorting */
/*#ifdef WIN32
static int alphaSort(const void *a, const void *b)
{
    return strcoll(((dirent*)a)->d_name, ((dirent*)b)->d_name);
}
#elif defined(LINUX) or defined(APPLE)*/
inline int alphaSort(const struct dirent **a, const struct dirent **b) {
    return strcoll((*a)->d_name, (*b)->d_name);
}
//#endif

inline bool replaceString(::std::string &str, const ::std::string &oldStr, const ::std::string &newStr) {
    bool found = false;
    size_t pos = 0;
    while ((pos = str.find(oldStr, pos)) != ::std::string::npos) {
        found = true;
        str.replace(pos, oldStr.length(), newStr);
        pos += newStr.length();
    }
    return found;
}

inline std::vector<std::string> splitStringToVector(const ::std::string &text, char delimiter, bool pushEmpty) {
    std::vector<std::string> arr;
    if (!text.empty()) {
        std::string::size_type start = 0;
        std::string::size_type end = text.find(delimiter, start);
        while (end != std::string::npos) {
            std::string token = text.substr(start, end - start);
            if (!token.empty() || (token.empty() && pushEmpty))
                arr.push_back(token);
            start = end + 1;
            end = text.find(delimiter, start);
        }
        arr.push_back(text.substr(start));
    }
    return arr;
}

inline std::vector<std::string> GetDrivesList() {
    std::vector<std::string> res;

#ifdef WIN32
    DWORD mydrives = 2048;
    char lpBuffer[2048];

    DWORD countChars = GetLogicalDriveStringsA(mydrives, lpBuffer);

    if (countChars > 0) {
        std::string var = std::string(lpBuffer, countChars);
        replaceString(var, "\\", "");
        res = splitStringToVector(var, '\0', false);
    }
#endif

    return res;
}

inline bool IsDirectoryExist(const std::string &name) {
    bool bExists = false;

    if (!name.empty()) {
        DIR *pDir = nullptr;
        pDir = opendir(name.c_str());
        if (pDir != nullptr) {
            bExists = true;
            (void) closedir(pDir);
        }
    }

    return bExists;// this is not a directory!
}

inline bool CreateDirectoryIfNotExist(const std::string &name) {
    bool res = false;

    if (!name.empty()) {
        if (!IsDirectoryExist(name)) {
            res = true;

#ifdef WIN32
            CreateDirectoryA(name.c_str(), nullptr);
#elif defined(LINUX) or defined(APPLE)
            char buffer[PATH_MAX] = {};
            snprintf(buffer, PATH_MAX, "mkdir -p %s", name.c_str());
            const int dir_err = std::system(buffer);
            if (dir_err == -1) {
                std::cout << "Error creating directory " << name << std::endl;
                res = false;
            }
#endif
        }
    }

    return res;
}

inline bool stringComparator(
        const FileInfoStruct &a,
        const FileInfoStruct &b) {
    bool res;
    if (a.type != b.type) res = (a.type < b.type);
    else
        res = (a.fileName < b.fileName);
    return res;
}

struct PathStruct
{
    std::string path;
    std::string name;
    std::string ext;

    bool isOk;

    PathStruct() {
        isOk = false;
    }
};

inline PathStruct ParsePathFileName(const std::string &vPathFileName) {
    PathStruct res;

    if (!vPathFileName.empty()) {
        std::string pfn = vPathFileName;
        std::string separator(1u, PATH_SEP);
        replaceString(pfn, "\\", separator);
        replaceString(pfn, "/", separator);

        size_t lastSlash = pfn.find_last_of(separator);
        if (lastSlash != std::string::npos) {
            res.name = pfn.substr(lastSlash + 1);
            res.path = pfn.substr(0, lastSlash);
            res.isOk = true;
        }

        size_t lastPoint = pfn.find_last_of('.');
        if (lastPoint != std::string::npos) {
            if (!res.isOk) {
                res.name = pfn;
                res.isOk = true;
            }
            res.ext = pfn.substr(lastPoint + 1);
            replaceString(res.name, "." + res.ext, "");
        }
    }

    return res;
}

inline void AppendToBuffer(char *vBuffer, size_t vBufferLen, const std::string &vStr) {
    std::string st = vStr;
    size_t len = vBufferLen - 1u;
    size_t slen = strlen(vBuffer);

    if (!st.empty() && st != "\n") {
        replaceString(st, "\n", "");
        replaceString(st, "\r", "");
    }
    vBuffer[slen] = '\0';
    std::string str = std::string(vBuffer);
    //if (!str.empty()) str += "\n";
    str += vStr;
    if (len > str.size()) len = str.size();
#ifdef MSVC
    strncpy_s(vBuffer, vBufferLen, str.c_str(), len);
#else
    strncpy(vBuffer, str.c_str(), len);
#endif
    vBuffer[len] = '\0';
}

inline void ResetBuffer(char *vBuffer) {
    vBuffer[0] = '\0';
}

char ImGuiFileDialog::FileNameBuffer[MAX_FILE_DIALOG_NAME_BUFFER] = "";
char ImGuiFileDialog::DirectoryNameBuffer[MAX_FILE_DIALOG_NAME_BUFFER] = "";
char ImGuiFileDialog::SearchBuffer[MAX_FILE_DIALOG_NAME_BUFFER] = "";
int ImGuiFileDialog::FilterIndex = 0;

ImGuiFileDialog::ImGuiFileDialog() {
    m_AnyWindowsHovered = false;
    IsOk = false;
    m_ShowDialog = false;
    m_ShowDrives = false;
    m_CreateDirectoryMode = false;
    dlg_optionsPane = nullptr;
    dlg_optionsPaneWidth = 250;
    dlg_filters = "";
}

ImGuiFileDialog::~ImGuiFileDialog() = default;

void ImGuiFileDialog::OpenDialog(const std::string &vKey, const char *vName, const char *vFilters,
                                 const std::string &vPath, const std::string &vDefaultFileName,
                                 const std::function<void(std::string, UserDatas, bool *)> &vOptionsPane, const size_t &vOptionsPaneWidth,
                                 const int &vCountSelectionMax, UserDatas vUserDatas) {
    if (m_ShowDialog)// if already opened, quit
        return;

    dlg_key = vKey;
    dlg_name = std::string(vName);
    dlg_filters = vFilters;
    dlg_path = vPath;
    dlg_defaultFileName = vDefaultFileName;
    dlg_optionsPane = std::move(vOptionsPane);
    dlg_userDatas = vUserDatas;
    dlg_optionsPaneWidth = vOptionsPaneWidth;
    dlg_CountSelectionMax = vCountSelectionMax;

    dlg_defaultExt = "";

    SetPath(m_CurrentPath);
    CheckFilter();

    m_ShowDialog = true;
}

void ImGuiFileDialog::OpenDialog(const std::string &vKey, const char *vName, const char *vFilters,
                                 const std::string &vFilePathName,
                                 const std::function<void(std::string, UserDatas, bool *)> &vOptionsPane, const size_t &vOptionsPaneWidth,
                                 const int &vCountSelectionMax, UserDatas vUserDatas) {
    if (m_ShowDialog)// if already opened, quit
        return;

    dlg_key = vKey;
    dlg_name = std::string(vName);
    dlg_filters = vFilters;

    auto ps = ParsePathFileName(vFilePathName);
    if (ps.isOk) {
        dlg_path = ps.path;
        dlg_defaultFileName = vFilePathName;
        dlg_defaultExt = "." + ps.ext;
    } else {
        dlg_path = ".";
        dlg_defaultFileName = "";
        dlg_defaultExt = "";
    }

    dlg_optionsPane = std::move(vOptionsPane);
    dlg_userDatas = vUserDatas;
    dlg_optionsPaneWidth = vOptionsPaneWidth;
    dlg_CountSelectionMax = vCountSelectionMax;

    SetPath(m_CurrentPath);
    CheckFilter();

    m_ShowDialog = true;
}

void ImGuiFileDialog::OpenDialog(const std::string &vKey, const char *vName, const char *vFilters,
                                 const std::string &vFilePathName, const int &vCountSelectionMax, UserDatas vUserDatas) {
    if (m_ShowDialog)// if already opened, quit
        return;

    dlg_key = vKey;
    dlg_name = std::string(vName);
    dlg_filters = vFilters;

    auto ps = ParsePathFileName(vFilePathName);
    if (ps.isOk) {
        dlg_path = ps.path;
        dlg_defaultFileName = vFilePathName;
        dlg_defaultExt = "." + ps.ext;
    } else {
        dlg_path = ".";
        dlg_defaultFileName = "";
        dlg_defaultExt = "";
    }

    dlg_optionsPane = nullptr;
    dlg_userDatas = vUserDatas;
    dlg_optionsPaneWidth = 0;
    dlg_CountSelectionMax = vCountSelectionMax;

    SetPath(m_CurrentPath);
    CheckFilter();

    m_ShowDialog = true;
}

void ImGuiFileDialog::OpenDialog(const std::string &vKey, const char *vName, const char *vFilters,
                                 const std::string &vPath, const std::string &vDefaultFileName, const int &vCountSelectionMax, UserDatas vUserDatas) {
    if (m_ShowDialog)// if already opened, quit
        return;

    dlg_key = vKey;
    dlg_name = std::string(vName);
    dlg_filters = vFilters;
    dlg_path = vPath;
    dlg_defaultFileName = vDefaultFileName;
    dlg_optionsPane = nullptr;
    dlg_userDatas = vUserDatas;
    dlg_optionsPaneWidth = 0;
    dlg_CountSelectionMax = vCountSelectionMax;

    dlg_defaultExt = "";

    SetPath(m_CurrentPath);
    CheckFilter();

    m_ShowDialog = true;
}

bool ImGuiFileDialog::FileDialog(const std::string &vKey, ImGuiWindowFlags vFlags) {
    if (m_ShowDialog && dlg_key == vKey) {
        bool res = false;

        std::string name = dlg_name + "##" + dlg_key;

        if (m_Name != name) {
            m_FileList.clear();
            m_CurrentPath_Decomposition.clear();
            m_SelectedExt.clear();
        }

        IsOk = false;

        if (ImGui::Begin(name.c_str(), (bool *) nullptr, vFlags | ImGuiWindowFlags_NoScrollbar)) {
            m_Name = name;

            m_AnyWindowsHovered |= ImGui::IsWindowHovered();

            if (dlg_path.empty()) dlg_path = ".";

            if (m_FileList.empty() && !m_ShowDrives) {
                replaceString(dlg_defaultFileName, dlg_path, "");// local path

                if (!dlg_defaultFileName.empty()) {
                    ResetBuffer(FileNameBuffer);
                    AppendToBuffer(FileNameBuffer, MAX_FILE_DIALOG_NAME_BUFFER, dlg_defaultFileName);
                    //m_SelectedFileName = dlg_defaultFileName;

                    if (!dlg_defaultExt.empty()) {
                        m_SelectedExt = dlg_defaultExt;

                        ImGuiFileDialog::FilterIndex = 0;
                        size_t size = 0;
                        const char *p = dlg_filters;// FIXME-OPT: Avoid computing this, or at least only when combo is open
                        while (*p) {
                            size += strlen(p) + 1;
                            p += size;
                        }
                        int idx = 0;
                        auto arr = splitStringToVector(std::string(dlg_filters, size), '\0', false);
                        for (auto &it: arr) {
                            if (m_SelectedExt == it) {
                                ImGuiFileDialog::FilterIndex = idx;
                                break;
                            }
                            idx++;
                        }
                    }
                }

                ScanDir(dlg_path);
            }

            if (IMGUI_BUTTON(createDirButtonString)) {
                if (!m_CreateDirectoryMode) {
                    m_CreateDirectoryMode = true;
                    ResetBuffer(DirectoryNameBuffer);
                }
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(buttonCreateDirString);

            if (m_CreateDirectoryMode) {
                ImGui::SameLine();

                ImGui::PushItemWidth(100.0f);
                ImGui::InputText("##DirectoryFileName", DirectoryNameBuffer, MAX_FILE_DIALOG_NAME_BUFFER);
                ImGui::PopItemWidth();

                ImGui::SameLine();

                if (IMGUI_BUTTON(okButtonString)) {
                    std::string newDir = std::string(DirectoryNameBuffer);
                    if (CreateDir(newDir)) {
                        SetPath(m_CurrentPath + PATH_SEP + newDir);
                    }

                    m_CreateDirectoryMode = false;
                }

                ImGui::SameLine();

                if (IMGUI_BUTTON(cancelButtonString)) {
                    m_CreateDirectoryMode = false;
                }
            }

            ImGui::SameLine();
            ImGui::Separator();

            ImGui::SameLine();

            if (IMGUI_BUTTON(resetButtonString)) {
                SetPath(".");
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(buttonResetPathString);

            bool drivesClick = false;

#ifdef WIN32
            ImGui::SameLine();

            if (IMGUI_BUTTON(drivesButtonString)) {
                drivesClick = true;
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(buttonDriveString);
#endif

            ImGui::SameLine();

            ImGui::Separator();

            // show current path
            bool pathClick = false;
            if (!m_CurrentPath_Decomposition.empty()) {
                ImGui::SameLine();
                for (auto itPathDecomp = m_CurrentPath_Decomposition.begin();
                     itPathDecomp != m_CurrentPath_Decomposition.end(); ++itPathDecomp) {
                    if (itPathDecomp != m_CurrentPath_Decomposition.begin())
                        ImGui::SameLine();
                    if (IMGUI_PATH_BUTTON((*itPathDecomp).c_str())) {
                        ComposeNewPath(itPathDecomp);
                        pathClick = true;
                        break;
                    }
                }
            }

            ImGuiContext &g = *GImGui;
            const float itemsHeight = (g.FontSize + g.Style.FramePadding.y * 2.0f + g.Style.ItemSpacing.y * 2.0f) * 4.0f + g.Style.WindowPadding.y * 2.0f;
            ImVec2 size = ImGui::GetContentRegionMax() - ImVec2((float) dlg_optionsPaneWidth, itemsHeight);

            // search field
            if (IMGUI_BUTTON(resetButtonString "##ImGuiFileDialogSearchFiled")) {
                ResetBuffer(SearchBuffer);
                searchTag.clear();
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(buttonResetSearchString);
            ImGui::SameLine();
            ImGui::Text(searchString);
            ImGui::SameLine();
            if (ImGui::InputText("##ImGuiFileDialogSearchFiled", SearchBuffer, MAX_FILE_DIALOG_NAME_BUFFER)) {
                searchTag = SearchBuffer;
            }

            ImGui::BeginChild("##FileDialog_FileList", size);

            for (auto &it: m_FileList) {
                const FileInfoStruct &infos = it;

                bool show = true;

                std::string str = " " + infos.fileName;
                if (infos.type == 'd') str = dirEntryString + str;
                if (infos.type == 'l') str = linkEntryString + str;
                if (infos.type == 'f') str = fileEntryString + str;
                if (infos.type == 'f' && !m_SelectedExt.empty() && (infos.ext != m_SelectedExt && m_SelectedExt != ".*")) {
                    show = false;
                }
                if (!searchTag.empty() && infos.fileName.find(searchTag) == std::string::npos) {
                    show = false;
                }
                if (show) {
                    ImVec4 c;
                    bool showColor = GetFilterColor(infos.ext, &c);
                    if (showColor)
                        ImGui::PushStyleColor(ImGuiCol_Text, c);

                    bool selected = false;
                    if (m_SelectedFileNames.find(infos.fileName) != m_SelectedFileNames.end())// found
                        selected = true;

                    if (ImGui::Selectable(str.c_str(), selected)) {
                        if (infos.type == 'd') {
                            pathClick = SelectDirectory(infos);
                        } else {
                            SelectFileName(infos);
                        }
                        if (showColor)
                            ImGui::PopStyleColor();
                        break;
                    }

                    if (showColor)
                        ImGui::PopStyleColor();
                }
            }

            // changement de repertoire
            if (pathClick) {
                SetPath(m_CurrentPath);
            }

            if (drivesClick) {
                GetDrives();
            }

            ImGui::EndChild();

            bool _CanWeContinue = true;

            if (dlg_optionsPane) {
                ImGui::SameLine();

                size.x = (float) dlg_optionsPaneWidth;

                ImGui::BeginChild("##FileTypes", size);

                dlg_optionsPane(m_SelectedExt, dlg_userDatas, &_CanWeContinue);

                ImGui::EndChild();
            }

            ImGui::Text(fileNameString);

            ImGui::SameLine();

            float width = ImGui::GetContentRegionAvail().x;
            if (dlg_filters) width -= 120.0f;
            ImGui::PushItemWidth(width);
            ImGui::InputText("##FileName", FileNameBuffer, MAX_FILE_DIALOG_NAME_BUFFER);
            ImGui::PopItemWidth();

            if (dlg_filters) {
                ImGui::SameLine();

                ImGui::PushItemWidth(100.0f);
                bool comboClick = ImGui::Combo("##Filters", &FilterIndex, dlg_filters) || m_SelectedExt.empty();
                ImGui::PopItemWidth();
                if (comboClick) {
                    int itemIdx = 0;
                    const char *p = dlg_filters;
                    while (*p) {
                        if (FilterIndex == itemIdx) {
                            m_SelectedExt = std::string(p);
                            break;
                        }
                        p += strlen(p) + 1;
                        itemIdx++;
                    }
                }
            }

            if (_CanWeContinue) {
                if (IMGUI_BUTTON(okButtonString)) {
                    if ('\0' != FileNameBuffer[0]) {
                        IsOk = true;
                        res = true;
                    }
                }

                ImGui::SameLine();
            }

            if (IMGUI_BUTTON(cancelButtonString)) {
                IsOk = false;
                res = true;
            }
        }

        ImGui::End();

        return res;
    }

    return false;
}

void ImGuiFileDialog::CloseDialog(const std::string &vKey) {
    if (dlg_key == vKey) {
        dlg_key.clear();
        m_ShowDialog = false;
    }
}

std::string ImGuiFileDialog::GetFilepathName() {
    std::string result = m_CurrentPath;

    if (s_fs_root != result) {
        result += PATH_SEP;
    }

    result += GetCurrentFileName();

    return result;
}

std::string ImGuiFileDialog::GetCurrentPath() {
    return m_CurrentPath;
}

std::string ImGuiFileDialog::GetCurrentFileName() {
    std::string result = FileNameBuffer;

    size_t lastPoint = result.find_last_of('.');
    if (lastPoint != std::string::npos) {
        result = result.substr(0, lastPoint);
    }

    result += m_SelectedExt;

    return result;
}

std::string ImGuiFileDialog::GetCurrentFilter() {
    return m_SelectedExt;
}

UserDatas ImGuiFileDialog::GetUserDatas() {
    return dlg_userDatas;
}

std::map<std::string, std::string> ImGuiFileDialog::GetSelection() {
    std::map<std::string, std::string> res;

    for (auto &it: m_SelectedFileNames) {
        std::string result = m_CurrentPath;

        if (s_fs_root != result) {
            result += PATH_SEP;
        }

        result += it.first;

        res[it.first] = result;
    }

    return res;
}

void ImGuiFileDialog::SetFilterColor(const std::string &vFilter, ImVec4 vColor) {
    m_FilterColor[vFilter] = vColor;
}

bool ImGuiFileDialog::GetFilterColor(const std::string &vFilter, ImVec4 *vColor) {
    if (vColor) {
        if (m_FilterColor.find(vFilter) != m_FilterColor.end())// found
        {
            *vColor = m_FilterColor[vFilter];
            return true;
        }
    }
    return false;
    ;
}

void ImGuiFileDialog::ClearFilterColor() {
    m_FilterColor.clear();
}

bool ImGuiFileDialog::SelectDirectory(const FileInfoStruct &vInfos) {
    bool pathClick = false;

    if (vInfos.fileName == "..") {
        if (m_CurrentPath_Decomposition.size() > 1) {
            ComposeNewPath(m_CurrentPath_Decomposition.end() - 2);
            pathClick = true;
        }
    } else {
        std::string newPath;

        if (m_ShowDrives) {
            newPath = vInfos.fileName + PATH_SEP;
        } else {
#ifdef LINUX
            if (s_fs_root == m_CurrentPath) {
                newPath = m_CurrentPath + infos.fileName;
            } else {
#endif
                newPath = m_CurrentPath + PATH_SEP + vInfos.fileName;
#ifdef LINUX
            }
#endif
        }

        if (IsDirectoryExist(newPath)) {
            if (m_ShowDrives) {
                m_CurrentPath = vInfos.fileName;
                s_fs_root = m_CurrentPath;
            } else {
                m_CurrentPath = newPath;
            }
            pathClick = true;
        }
    }

    return pathClick;
}

void ImGuiFileDialog::SelectFileName(const FileInfoStruct &vInfos) {
    if (ImGui::GetIO().KeyCtrl) {
        if (dlg_CountSelectionMax == 0)// infinite selection
        {
            if (m_SelectedFileNames.find(vInfos.fileName) == m_SelectedFileNames.end())// not found +> add
            {
                AddFileNameInSelection(vInfos.fileName, true);
            } else// found +> remove
            {
                RemoveFileNameInSelection(vInfos.fileName);
            }
        } else// selection limited by size
        {
            if (m_SelectedFileNames.size() <= dlg_CountSelectionMax) {
                if (m_SelectedFileNames.find(vInfos.fileName) == m_SelectedFileNames.end())// not found +> add
                {
                    AddFileNameInSelection(vInfos.fileName, true);
                } else// found +> remove
                {
                    RemoveFileNameInSelection(vInfos.fileName);
                }
            }
        }
    } else if (ImGui::GetIO().KeyShift) {
        if (dlg_CountSelectionMax != 1) {
            m_SelectedFileNames.clear();
            // we will iterate filelist and get the last selection after the start selection
            bool startMultiSelection = false;
            std::string fileNameToSelect = vInfos.fileName;
            std::string savedLastSelectedFileName;// for invert selection mode
            for (auto &it: m_FileList) {
                const FileInfoStruct &infos = it;

                bool canTake = true;
                if (infos.type == 'f' && !m_SelectedExt.empty() && (infos.ext != m_SelectedExt && m_SelectedExt != ".*")) canTake = false;
                if (!searchTag.empty() && infos.fileName.find(searchTag) == std::string::npos) canTake = false;
                if (canTake)// if not filtered, we will take files who are filtered by the dialog
                {
                    if (infos.fileName == m_LastSelectedFileName) {
                        startMultiSelection = true;
                        AddFileNameInSelection(m_LastSelectedFileName, false);
                    } else if (startMultiSelection) {
                        if (dlg_CountSelectionMax == 0)// infinite selection
                        {
                            AddFileNameInSelection(infos.fileName, false);
                        } else// selection limited by size
                        {
                            if (m_SelectedFileNames.size() < dlg_CountSelectionMax) {
                                AddFileNameInSelection(infos.fileName, false);
                            } else {
                                startMultiSelection = false;
                                if (!savedLastSelectedFileName.empty())
                                    m_LastSelectedFileName = savedLastSelectedFileName;
                                break;
                            }
                        }
                    }

                    if (infos.fileName == fileNameToSelect) {
                        if (!startMultiSelection)// we are before the last Selected FileName, so we must inverse
                        {
                            savedLastSelectedFileName = m_LastSelectedFileName;
                            m_LastSelectedFileName = fileNameToSelect;
                            fileNameToSelect = savedLastSelectedFileName;
                            startMultiSelection = true;
                            AddFileNameInSelection(m_LastSelectedFileName, false);
                        } else {
                            startMultiSelection = false;
                            if (!savedLastSelectedFileName.empty())
                                m_LastSelectedFileName = savedLastSelectedFileName;
                            break;
                        }
                    }
                }
            }
        }
    } else {
        m_SelectedFileNames.clear();
        ResetBuffer(FileNameBuffer);
        AddFileNameInSelection(vInfos.fileName, true);
    }
}

void ImGuiFileDialog::RemoveFileNameInSelection(const std::string &vFileName) {
    m_SelectedFileNames.erase(vFileName);

    if (m_SelectedFileNames.size() == 1) {
        snprintf(FileNameBuffer, MAX_FILE_DIALOG_NAME_BUFFER, "%s", vFileName.c_str());
    } else {
        snprintf(FileNameBuffer, MAX_FILE_DIALOG_NAME_BUFFER, "%zu files Selected", m_SelectedFileNames.size());
    }
}

void ImGuiFileDialog::AddFileNameInSelection(const std::string &vFileName, bool vSetLastSelectionFileName) {
    m_SelectedFileNames[vFileName];

    if (m_SelectedFileNames.size() == 1) {
        snprintf(FileNameBuffer, MAX_FILE_DIALOG_NAME_BUFFER, "%s", vFileName.c_str());
    } else {
        snprintf(FileNameBuffer, MAX_FILE_DIALOG_NAME_BUFFER, "%zu files Selected", m_SelectedFileNames.size());
    }

    if (vSetLastSelectionFileName)
        m_LastSelectedFileName = vFileName;
}

void ImGuiFileDialog::CheckFilter() {
    bool found = false;
    int itemIdx = 0;
    const char *p = dlg_filters;
    while (*p) {
        if (m_SelectedExt == std::string(p)) {
            found = true;
            FilterIndex = itemIdx;
            break;
        }
        p += strlen(p) + 1;
        itemIdx++;
    }
    if (!found) {
        m_SelectedExt.clear();
        FilterIndex = 0;
    }
}

void ImGuiFileDialog::SetPath(const std::string &vPath) {
    m_ShowDrives = false;
    m_CurrentPath = vPath;
    m_FileList.clear();
    m_CurrentPath_Decomposition.clear();
    ScanDir(m_CurrentPath);
}

void ImGuiFileDialog::ScanDir(const std::string &vPath) {
    struct dirent **files = nullptr;
    int i = 0;
    int n = 0;
    std::string path = vPath;

#if defined(UNIX)// UNIX is LINUX or APPLE
    if (path.size() > 0) {
        if (path[0] != PATH_SEP) {
            //path = PATH_SEP + path;
        }
    }
#endif

    if (m_CurrentPath_Decomposition.empty()) {
        SetCurrentDir(path);
    }

    if (!m_CurrentPath_Decomposition.empty()) {
#ifdef WIN32
        if (path == s_fs_root) {
            path += PATH_SEP;
        }
#endif
        n = scandir(path.c_str(), &files, nullptr, alphaSort);
        if (n > 0) {
            m_FileList.clear();

            for (i = 0; i < n; i++) {
                struct dirent *ent = files[i];

                FileInfoStruct infos;

                infos.fileName = ent->d_name;
                if (("." != infos.fileName) /* && (".." != infos.fileName)*/) {
                    switch (ent->d_type) {
                        case DT_REG:
                            infos.type = 'f';
                            break;
                        case DT_DIR:
                            infos.type = 'd';
                            break;
                        case DT_LNK:
                            infos.type = 'l';
                            break;
                    }

                    if (infos.type == 'f') {
                        size_t lpt = infos.fileName.find_last_of('.');
                        if (lpt != std::string::npos) {
                            infos.ext = infos.fileName.substr(lpt);
                        }
                    }

                    m_FileList.push_back(infos);
                }
            }

            for (i = 0; i < n; i++) {
                METAENGINE_FREE(files[i]);
            }
            METAENGINE_FREE(files);
        }

        std::sort(m_FileList.begin(), m_FileList.end(), stringComparator);
    }
}

void ImGuiFileDialog::SetCurrentDir(const std::string &vPath) {
    std::string path = vPath;
#ifdef WIN32
    if (s_fs_root == path)
        path += PATH_SEP;
#endif
    DIR *dir = opendir(path.c_str());
    char real_path[PATH_MAX];

    if (nullptr == dir) {
        path = ".";
        dir = opendir(path.c_str());
    }

    if (nullptr != dir) {
#ifdef WIN32
        size_t numchar = GetFullPathNameA(path.c_str(), PATH_MAX - 1, real_path, nullptr);
#elif defined(LINUX) or defined(APPLE)
        char *numchar = realpath(path.c_str(), real_path);
#endif
        if (numchar != 0) {
            m_CurrentPath = real_path;
            if (m_CurrentPath[m_CurrentPath.size() - 1] == PATH_SEP) {
                m_CurrentPath = m_CurrentPath.substr(0, m_CurrentPath.size() - 1);
            }
            m_CurrentPath_Decomposition = splitStringToVector(m_CurrentPath, PATH_SEP, false);
#if defined(UNIX)// UNIX is LINUX or APPLE
            m_CurrentPath_Decomposition.insert(m_CurrentPath_Decomposition.begin(), std::string(1u, PATH_SEP));
#endif
            if (!m_CurrentPath_Decomposition.empty()) {
#ifdef WIN32
                s_fs_root = m_CurrentPath_Decomposition[0];
#endif
            }
        }

        closedir(dir);
    }
}

bool ImGuiFileDialog::CreateDir(const std::string &vPath) {
    bool res = false;

    if (!vPath.empty()) {
        std::string path = m_CurrentPath + PATH_SEP + vPath;

        res = CreateDirectoryIfNotExist(path);
    }

    return res;
}

void ImGuiFileDialog::ComposeNewPath(std::vector<std::string>::iterator vIter) {
    m_CurrentPath = "";

    while (true) {
        if (!m_CurrentPath.empty()) {
#ifdef WIN32
            m_CurrentPath = *vIter + PATH_SEP + m_CurrentPath;
#elif defined(LINUX) or defined(APPLE)
            if (*vIter == s_fs_root) {
                m_CurrentPath = *vIter + m_CurrentPath;
            } else {
                m_CurrentPath = *vIter + PATH_SEP + m_CurrentPath;
            }
#endif
        } else {
            m_CurrentPath = *vIter;
        }

        if (vIter == m_CurrentPath_Decomposition.begin()) {
#if defined(UNIX)// UNIX is LINUX or APPLE
            if (m_CurrentPath[0] != PATH_SEP)
                m_CurrentPath = PATH_SEP + m_CurrentPath;
#endif
            break;
        }

        vIter--;
    }
}

void ImGuiFileDialog::GetDrives() {
    auto res = GetDrivesList();
    if (!res.empty()) {
        m_CurrentPath = "";
        m_CurrentPath_Decomposition.clear();
        m_FileList.clear();
        for (auto &re: res) {
            FileInfoStruct infos;
            infos.fileName = re;
            infos.type = 'd';

            if (!infos.fileName.empty()) {
                m_FileList.push_back(infos);
            }
        }
        m_ShowDrives = true;
    }
}


/**
   Dear ImGui with IME on-the-spot translation routines.
   author: TOGURO Mikito , mit@shalab.net
*/

#include <commctrl.h>
#include <tchar.h>
#include <windows.h>

#include <algorithm>
#include <cassert>

#include "imgui.h"
#include "imgui_internal.h"

#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib")

void ImGUIIMMCommunication::operator()() {
    ImGuiIO &io = ImGui::GetIO();

    static ImVec2 window_pos = ImVec2();
    static ImVec2 window_pos_pivot = ImVec2();


    static ImGuiID candidate_window_root_id = 0;

    static ImGuiWindow *lastTextInputNavWindow = nullptr;
    static ImGuiID lastTextInputActiveId = 0;
    static ImGuiID lastTextInputFocusId = 0;

    if (!(candidate_window_root_id &&
          ((ImGui::GetCurrentContext()->NavWindow ? ImGui::GetCurrentContext()->NavWindow->RootWindow->ID : 0u) == candidate_window_root_id))) {

        window_pos = ImVec2(ImGui::GetCurrentContext()->PlatformImeData.InputPos.x + 1.0f,
                            ImGui::GetCurrentContext()->PlatformImeData.InputPos.y);//
        window_pos_pivot = ImVec2(0.0f, 0.0f);

        const ImGuiContext *const currentContext = ImGui::GetCurrentContext();
        IM_ASSERT(currentContext || !"ImGui::GetCurrentContext() return nullptr.");
        if (currentContext) {
            if (!ImGui::IsMouseClicked(0)) {
                if ((currentContext->WantTextInputNextFrame != -1) ? (!!(currentContext->WantTextInputNextFrame)) : false) {
                    if ((!!currentContext->NavWindow) &&
                        (currentContext->NavWindow->RootWindow->ID != candidate_window_root_id) &&
                        (ImGui::GetActiveID() != lastTextInputActiveId)) {
                        OutputDebugStringW(L"update lastTextInputActiveId\n");
                        lastTextInputNavWindow = ImGui::GetCurrentContext()->NavWindow;
                        lastTextInputActiveId = ImGui::GetActiveID();
                        lastTextInputFocusId = ImGui::GetFocusID();
                    }
                } else {
                    if (lastTextInputActiveId != 0) {
                        if (currentContext->WantTextInputNextFrame) {
                            OutputDebugStringW(L"update lastTextInputActiveId disabled\n");
                        } else {
                            OutputDebugStringW(L"update lastTextInputActiveId disabled update\n");
                        }
                    }
                    lastTextInputNavWindow = nullptr;
                    lastTextInputActiveId = 0;
                    lastTextInputFocusId = 0;
                }
            }
        }
    }

    ImVec2 target_screen_pos = ImVec2(0.0f, 0.0f);// IME Candidate List Window position.

    if (this->is_open) {
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        if (ImGui::Begin("IME Composition Window", nullptr,
                         ImGuiWindowFlags_Tooltip |
                                 ImGuiWindowFlags_NoNav |
                                 ImGuiWindowFlags_NoDecoration |
                                 ImGuiWindowFlags_NoInputs |
                                 ImGuiWindowFlags_AlwaysAutoResize |
                                 ImGuiWindowFlags_NoSavedSettings)) {

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.78125f, 1.0f, 0.1875f, 1.0f));
            ImGui::Text(static_cast<bool>(comp_conved_utf8) ? comp_conved_utf8.get() : "");
            ImGui::PopStyleColor();

            if (static_cast<bool>(comp_target_utf8)) {
                ImGui::SameLine(0.0f, 0.0f);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.203125f, 0.91796875f, 0.35546875f, 1.0f));

                target_screen_pos = ImGui::GetCursorScreenPos();
                target_screen_pos.y += ImGui::GetTextLineHeightWithSpacing();

                ImGui::Text(static_cast<bool>(comp_target_utf8) ? comp_target_utf8.get() : "");
                ImGui::PopStyleColor();
            }
            if (static_cast<bool>(comp_unconv_utf8)) {
                ImGui::SameLine(0.0f, 0.0f);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.78125f, 1.0f, 0.1875f, 1.0f));
                ImGui::Text(static_cast<bool>(comp_unconv_utf8) ? comp_unconv_utf8.get() : "");
                ImGui::PopStyleColor();
            }
            /*
              ImGui::SameLine();
              ImGui::Text("%u %u",
              candidate_window_root_id ,
              ImGui::GetCurrentContext()->NavWindow ? ImGui::GetCurrentContext()->NavWindow->RootWindow->ID : 0u);
            */
            ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
        }
        ImGui::End();
        ImGui::PopStyleVar();

        /* Draw Candidate List */
        if (show_ime_candidate_list && !candidate_list.list_utf8.empty()) {

            std::vector<const char *> listbox_items = {};

            IM_ASSERT(candidate_window_num);
            int candidate_page = ((int) candidate_list.selection) / candidate_window_num;
            int candidate_selection = ((int) candidate_list.selection) % candidate_window_num;

            auto begin_ite = std::begin(candidate_list.list_utf8);
            std::advance(begin_ite, candidate_page * candidate_window_num);
            auto end_ite = begin_ite;
            {
                auto the_end = std::end(candidate_list.list_utf8);
                for (int i = 0; end_ite != the_end && i < candidate_window_num; ++i) {
                    std::advance(end_ite, 1);
                }
            }

            std::for_each(begin_ite, end_ite,
                          [&](auto &item) {
                              listbox_items.push_back(item.c_str());
                          });

            const float candidate_window_height =
                    ((ImGui::GetStyle().FramePadding.y * 2) +
                     ((ImGui::GetTextLineHeightWithSpacing()) * ((int) std::size(listbox_items) + 2)));

            if (io.DisplaySize.y < (target_screen_pos.y + candidate_window_height)) {
                target_screen_pos.y -=
                        ImGui::GetTextLineHeightWithSpacing() + candidate_window_height;
            }

            ImGui::SetNextWindowPos(target_screen_pos, ImGuiCond_Always, window_pos_pivot);

            if (ImGui::Begin("##Overlay-IME-Candidate-List-Window",
                             &show_ime_candidate_list,
                             ImGuiWindowFlags_NoMove |
                                     ImGuiWindowFlags_NoDecoration |
                                     ImGuiWindowFlags_AlwaysAutoResize |
                                     ImGuiWindowFlags_NoInputs |
                                     ImGuiWindowFlags_NoSavedSettings |
                                     ImGuiWindowFlags_NoFocusOnAppearing |
                                     ImGuiWindowFlags_NoNav)) {
                if (ImGui::ListBoxHeader("##IMECandidateListWindow",
                                         static_cast<int>(std::size(listbox_items)),
                                         static_cast<int>(std::size(listbox_items)))) {

                    int i = 0;
                    for (const char *&listbox_item: listbox_items) {
                        if (ImGui::Selectable(listbox_item, (i == candidate_selection))) {

                            /* candidate list selection */

                            if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
                                !ImGui::IsAnyItemActive() &&
                                !ImGui::IsMouseClicked(0)) {
                                if (lastTextInputActiveId && lastTextInputFocusId) {
                                    ImGui::SetActiveID(lastTextInputActiveId, lastTextInputNavWindow);
                                    ImGui::SetFocusID(lastTextInputFocusId, lastTextInputNavWindow);
                                }
                            }

                            /*
                                我想做 ImmNotifyIME (hImc, NI_SELECTCANDIDATESTR, 0, Candidate_page * Candidate_window_num + i);
                              由于 Vista ImmNotifyIME 不支持 NI_SELECTCANDIDATESTR。
                              @ 请参阅 Microsoft 的 IMM32 兼容性信息.doc

                              因此，DXUTguiIME.cpp（曾经使用过的 DXUT 的 gui IME 处理单元现在被视为已弃用。
                              您可以在 https://github.com/microsoft/DXUT 查看
                              有问题的代码是 https://github.com/microsoft/DXUT/blob/master/Optional/DXUTguiIME.cpp）
                              我确认过了

                              从 L.380 单击候选列表时有一个代码

                              原因是 SendKey 用于通过发送箭头光标键从候选列表中进行选择。
                              （你是什么意思 ？！）

                              基于此，使用 SendKey 创建代码。
                            */
                            {
                                if (candidate_selection == i) {
                                    OutputDebugStringW(L"complete\n");
                                    this->request_candidate_list_str_commit = 1;
                                } else {
                                    const BYTE nVirtualKey = (candidate_selection < i) ? VK_DOWN : VK_UP;
                                    const size_t nNumToHit = abs(candidate_selection - i);
                                    for (size_t hit = 0; hit < nNumToHit; ++hit) {
                                        keybd_event(nVirtualKey, 0, 0, 0);
                                        keybd_event(nVirtualKey, 0, KEYEVENTF_KEYUP, 0);
                                    }
                                    this->request_candidate_list_str_commit = (int) nNumToHit;
                                }
                            }
                        }
                        ++i;
                    }
                    ImGui::ListBoxFooter();
                }
                ImGui::Text("%d/%d",
                            candidate_list.selection + 1, static_cast<int>(std::size(candidate_list.list_utf8)));
#if defined(_DEBUG)
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "%s",
#if defined(UNICODE)
                                   u8"DEBUG (UNICODE)"
#else
                                   u8"DEBUG (MBCS)"
#endif /* defined( UNICODE ) */
                );
#endif /* defined( DEBUG ) */
                // #1 ここで作るウィンドウがフォーカスを持ったときには、ウィンドウの位置を変更してはいけない。
                candidate_window_root_id = ImGui::GetCurrentWindowRead()->RootWindow->ID;
                ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
            }
            ImGui::End();
        }
    }

    /*
      io.WantTextInput 根本不可靠。
      在文本输入小部件中，在另一个窗口上按下鼠标按钮以
      其他窗口和小部件在启用 WantTextInput 的情况下处于活动状态
      （特别是当涉及到 IME 候选窗口的小部件时，
      Press 焦点移动到启用了 WantTextInput 的 IME 候选窗口。
      在下一帧
      WantTextInput 已关闭，因为 IME 候选窗口未启用文本输入
      然后，这里就出现了IME失效的现象。 )

      这与直觉行为相反，所以我们会修复它，但是当禁用它时，
      如果任何小部件没有获得窗口焦点
      当 WantTextInput 为真时启用
      不对称的形状减少了不适。
    */
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow)) {
        if (io.ImeWindowHandle) {
            IM_ASSERT(IsWindow(static_cast<HWND>(io.ImeWindowHandle)));
            (void) (ImmAssociateContext(static_cast<HWND>(io.ImeWindowHandle), HIMC(0)));
        }
    }
    if (io.WantTextInput) {
        if (io.ImeWindowHandle) {
            IM_ASSERT(IsWindow(static_cast<HWND>(io.ImeWindowHandle)));
            /*
              The second argument of ImmAssociateContextEx is
              Changed from IN to _In_ between 10.0.10586.0-Windows 10 and 10.0.14393.0-Windows.

              https://abi-laboratory.pro/compatibility/Windows_10_1511_10586.494_to_Windows_10_1607_14393.0/x86_64/headers_diff/imm32.dll/diff.html

              This is strange, but HIMC is probably not zero because this change was intentional.
              However, the document states that HIMC is ignored if the flag IACE_DEFAULT is used.

              https://docs.microsoft.com/en-us/windows/win32/api/immdev/nf-immdev-immassociatecontextex

              Passing HIMC appropriately when using IACE_DEFAULT causes an error and returns to 0
            */
            VERIFY(ImmAssociateContextEx(static_cast<HWND>(io.ImeWindowHandle), HIMC(0), IACE_DEFAULT));
        }
    }

    return;
}

ImGUIIMMCommunication::IMMCandidateList
ImGUIIMMCommunication::IMMCandidateList::cocreate(const CANDIDATELIST *const src, const size_t src_size) {
    IM_ASSERT(nullptr != src);
    IM_ASSERT(sizeof(CANDIDATELIST) <= src->dwSize);
    IM_ASSERT(src->dwSelection < src->dwCount);

    IMMCandidateList dst{};
    if (!(sizeof(CANDIDATELIST) < src->dwSize)) {
        return dst;
    }
    if (!(src->dwSelection < src->dwCount)) {
        return dst;
    }
    const char *const baseaddr = reinterpret_cast<const char *>(src);

    for (size_t i = 0; i < src->dwCount; ++i) {
        const wchar_t *const item = reinterpret_cast<const wchar_t *>(baseaddr + src->dwOffset[i]);
        const int require_byte = WideCharToMultiByte(CP_UTF8, 0, item, -1, nullptr, 0, NULL, NULL);
        if (0 < require_byte) {
            std::unique_ptr<char[]> utf8buf{new char[require_byte]};
            if (require_byte == WideCharToMultiByte(CP_UTF8, 0, item, -1, utf8buf.get(), require_byte, NULL, NULL)) {
                dst.list_utf8.emplace_back(utf8buf.get());
                continue;
            }
        }
        dst.list_utf8.emplace_back("??");
    }
    dst.selection = src->dwSelection;
    return dst;
}

bool ImGUIIMMCommunication::update_candidate_window(HWND hWnd) {
    IM_ASSERT(IsWindow(hWnd));
    bool result = false;
    HIMC const hImc = ImmGetContext(hWnd);
    if (hImc) {
        DWORD dwSize = ImmGetCandidateListW(hImc, 0, NULL, 0);

        if (dwSize) {
            IM_ASSERT(sizeof(CANDIDATELIST) <= dwSize);
            if (sizeof(CANDIDATELIST) <= dwSize) {

                std::vector<char> candidatelist((size_t) dwSize);
                if ((DWORD) (std::size(candidatelist) * sizeof(typename decltype(candidatelist)::value_type)) == ImmGetCandidateListW(hImc, 0,
                                                                                                                                      reinterpret_cast<CANDIDATELIST *>(candidatelist.data()),
                                                                                                                                      (DWORD) (std::size(candidatelist) * sizeof(typename decltype(candidatelist)::value_type)))) {
                    const CANDIDATELIST *const cl = reinterpret_cast<CANDIDATELIST *>(candidatelist.data());
                    candidate_list = std::move(IMMCandidateList::cocreate(cl, dwSize));
                    result = true;
#if 0  /* for IMM candidate window debug BEGIN*/
                    {
                        wchar_t dbgbuf[1024];
                        _snwprintf_s(dbgbuf, sizeof(dbgbuf) / sizeof(dbgbuf[0]),
                            L" dwSize = %d , dwCount = %d , dwSelection = %d\n",
                            cl->dwSize,
                            cl->dwCount,
                            cl->dwSelection);
                        OutputDebugStringW(dbgbuf);
                        for (DWORD i = 0; i < cl->dwCount; ++i) {
                            _snwprintf_s(dbgbuf, sizeof(dbgbuf) / sizeof(dbgbuf[0]),
                                L"%d%s: %s\n",
                                i, (cl->dwSelection == i ? L"*" : L" "),
                                (wchar_t*)(candidatelist.data() + cl->dwOffset[i]));
                            OutputDebugStringW(dbgbuf);
                        }
                    }
#endif /* for IMM candidate window debug END */
                }
            }
        }
        VERIFY(ImmReleaseContext(hWnd, hImc));
    }
    return result;
}

LRESULT
ImGUIIMMCommunication::imm_communication_subClassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                                      UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_DESTROY: {
            VERIFY(ImmAssociateContextEx(hWnd, HIMC(0), IACE_DEFAULT));
            if (!RemoveWindowSubclass(hWnd, reinterpret_cast<SUBCLASSPROC>(uIdSubclass), uIdSubclass)) {
                IM_ASSERT(!"RemoveWindowSubclass() failed\n");
            }
        } break;
        default:
            if (dwRefData) {
                return imm_communication_subClassProc_implement(hWnd, uMsg, wParam, lParam,
                                                                uIdSubclass, *reinterpret_cast<ImGUIIMMCommunication *>(dwRefData));
            }
    }
    return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT
ImGUIIMMCommunication::imm_communication_subClassProc_implement(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                                                UINT_PTR uIdSubclass, ImGUIIMMCommunication &comm) {
    switch (uMsg) {
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
            if (comm.is_open) {
                return 0;
            }
            break;

        case WM_IME_SETCONTEXT: { /* 各ビットを落とす */
            lParam &= ~(ISC_SHOWUICOMPOSITIONWINDOW |
                        (ISC_SHOWUICANDIDATEWINDOW) |
                        (ISC_SHOWUICANDIDATEWINDOW << 1) |
                        (ISC_SHOWUICANDIDATEWINDOW << 2) |
                        (ISC_SHOWUICANDIDATEWINDOW << 3));
        }
            return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
        case WM_IME_STARTCOMPOSITION: {
            /* 此消息通知应用程序请求“IME”显示转换窗口。
            应用程序在处理转换字符串的显示时必须处理此消息。
            如果您让 DefWindowProc 函数处理此消息，则该消息将被传递到默认 IME 窗口。
            （即自己绘制转换窗口时，不要传给DefWindowPrc）
        */
            comm.is_open = true;
        }
            return 1;
        case WM_IME_ENDCOMPOSITION: {
            comm.is_open = false;
        }
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        case WM_IME_COMPOSITION: {
            HIMC const hImc = ImmGetContext(hWnd);
            if (hImc) {
                if (lParam & GCS_RESULTSTR) {
                    comm.comp_conved_utf8 = nullptr;
                    comm.comp_target_utf8 = nullptr;
                    comm.comp_unconv_utf8 = nullptr;
                    comm.show_ime_candidate_list = false;
                }
                if (lParam & GCS_COMPSTR) {

                    const DWORD compstr_length_in_byte = ImmGetCompositionStringW(hImc, GCS_COMPSTR, nullptr, 0);
                    switch (compstr_length_in_byte) {
                        case IMM_ERROR_NODATA:
                        case IMM_ERROR_GENERAL:
                            break;
                        default: {
                            size_t const buf_length_in_wchar = (size_t(compstr_length_in_byte) / sizeof(wchar_t)) + 1;
                            IM_ASSERT(0 < buf_length_in_wchar);
                            std::unique_ptr<wchar_t[]> buf{new wchar_t[buf_length_in_wchar]};
                            if (buf) {
                                //std::fill( &buf[0] , &buf[buf_length_in_wchar-1] , L'\0' );
                                const LONG buf_length_in_byte = LONG(buf_length_in_wchar * sizeof(wchar_t));
                                const DWORD l = ImmGetCompositionStringW(hImc, GCS_COMPSTR,
                                                                         (LPVOID) (buf.get()), buf_length_in_byte);

                                const DWORD attribute_size = ImmGetCompositionStringW(hImc, GCS_COMPATTR, NULL, 0);
                                std::vector<char> attribute_vec(attribute_size, 0);
                                const DWORD attribute_end =
                                        ImmGetCompositionStringW(hImc, GCS_COMPATTR, attribute_vec.data(), (DWORD) std::size(attribute_vec));
                                IM_ASSERT(attribute_end == (DWORD) (std::size(attribute_vec)));
                                {
                                    std::wstring comp_converted;
                                    std::wstring comp_target;
                                    std::wstring comp_unconveted;
                                    size_t begin = 0;
                                    size_t end = 0;

                                    for (end = begin; end < attribute_end; ++end) {
                                        if ((ATTR_TARGET_CONVERTED == attribute_vec[end] ||
                                             ATTR_TARGET_NOTCONVERTED == attribute_vec[end])) {
                                            break;
                                        } else {
                                            comp_converted.push_back(buf[end]);
                                        }
                                    }

                                    for (begin = end; end < attribute_end; ++end) {
                                        if (!(ATTR_TARGET_CONVERTED == attribute_vec[end] ||
                                              ATTR_TARGET_NOTCONVERTED == attribute_vec[end])) {
                                            break;
                                        } else {
                                            comp_target.push_back(buf[end]);
                                        }
                                    }

                                    for (; end < attribute_end; ++end) {
                                        comp_unconveted.push_back(buf[end]);
                                    }

#if 0
                            {
                                wchar_t dbgbuf[1024];
                                _snwprintf_s(dbgbuf, sizeof(dbgbuf) / sizeof(dbgbuf[0]),
                                    L"attribute_size = %d \"%s[%s]%s\"\n",
                                    attribute_size,
                                    comp_converted.c_str(),
                                    comp_target.c_str(),
                                    comp_unconveted.c_str());
                                OutputDebugStringW(dbgbuf);
                            }
#endif
                                    // 准备一个 lambda 函数以将每个函数转换为 UTF-8
                                    /*
                                将 std::wstring 转换为 std::unique_ptr <char[]> 作为 UTF - 8 空终止字符串
                                如果参数是空字符串，则输入 nullptr
                                */
                                    auto to_utf8_pointer = [](const std::wstring &arg) -> std::unique_ptr<char[]> {
                                        if (arg.empty()) {
                                            return std::unique_ptr<char[]>(nullptr);
                                        }
                                        const int require_byte = WideCharToMultiByte(CP_UTF8, 0, arg.c_str(), -1, nullptr, 0, NULL, NULL);
                                        if (0 == require_byte) {
                                            const DWORD lastError = GetLastError();
                                            (void) (lastError);
                                            IM_ASSERT(ERROR_INSUFFICIENT_BUFFER != lastError);
                                            IM_ASSERT(ERROR_INVALID_FLAGS != lastError);
                                            IM_ASSERT(ERROR_INVALID_PARAMETER != lastError);
                                            IM_ASSERT(ERROR_NO_UNICODE_TRANSLATION != lastError);
                                        }
                                        IM_ASSERT(0 != require_byte);
                                        if (!(0 < require_byte)) {
                                            return std::unique_ptr<char[]>(nullptr);
                                        }

                                        std::unique_ptr<char[]> utf8buf{new char[require_byte]};

                                        const int conversion_result =
                                                WideCharToMultiByte(CP_UTF8, 0, arg.c_str(), -1, utf8buf.get(), require_byte, NULL, NULL);
                                        if (conversion_result == 0) {
                                            const DWORD lastError = GetLastError();
                                            (void) (lastError);
                                            IM_ASSERT(ERROR_INSUFFICIENT_BUFFER != lastError);
                                            IM_ASSERT(ERROR_INVALID_FLAGS != lastError);
                                            IM_ASSERT(ERROR_INVALID_PARAMETER != lastError);
                                            IM_ASSERT(ERROR_NO_UNICODE_TRANSLATION != lastError);
                                        }

                                        IM_ASSERT(require_byte == conversion_result);
                                        if (require_byte != conversion_result) {
                                            utf8buf.reset(nullptr);
                                        }
                                        return utf8buf;
                                    };

                                    comm.comp_conved_utf8 = to_utf8_pointer(comp_converted);
                                    comm.comp_target_utf8 = to_utf8_pointer(comp_target);
                                    comm.comp_unconv_utf8 = to_utf8_pointer(comp_unconveted);

                                    /*当 GCS_COMPSTR 更新时，Google IME 当然会 IMN_CHANGECANDIDATE
                                不会像完成一样发送。
                                所以，我请求你在这里更新候选人名单 */

                                    // 如果comp_target为空则删除
                                    if (!static_cast<bool>(comm.comp_target_utf8)) {
                                        comm.candidate_list.clear();
                                    } else {
                                        // candidate_list の取得に失敗したときは消去する
                                        if (!comm.update_candidate_window(hWnd)) {
                                            comm.candidate_list.clear();
                                        }
                                    }
                                }
                            }
                        } break;
                    }
                }
                VERIFY(ImmReleaseContext(hWnd, hImc));
            }


        }// end of WM_IME_COMPOSITION

#if defined(UNICODE)
            // 在UNICODE配置的情况下，直接用DefWindowProc吸收进IME就可以了
            return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
            // 在多字节配置中，Window 子类的过程处理它，所以需要 DefSubclassProc。
#else
            return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
#endif

        case WM_IME_NOTIFY: {
            switch (wParam) {

                case IMN_OPENCANDIDATE:
                    // 通常，在发送 IMN_OPENCANDIDATE 时会设置 show_ime_candidate_list 标志。
                    // Google IME 不发送 IMN_OPENCANDIDATE（IMN_CHANGECANDIDATE 发送）
                    // 为此，在启动 IMN_CHANGECANDIDATE 时进行更改
                    comm.show_ime_candidate_list = true;
#if 0
            if (IMN_OPENCANDIDATE == wParam) {
                OutputDebugStringW(L"IMN_OPENCANDIDATE\n");
            }
#endif
                    ;// tear down;
                case IMN_CHANGECANDIDATE: {
#if 0
            if (IMN_CHANGECANDIDATE == wParam) {
                OutputDebugStringW(L"IMN_CHANGECANDIDATE\n");
            }
#endif

                    // Google IME BEGIN 的代码 有关详细信息，请参阅有关 IMN_OPENCANDIDATE 的评论
                    if (!comm.show_ime_candidate_list) {
                        comm.show_ime_candidate_list = true;
                    }
                    // 谷歌输入法结束代码


                    HIMC const hImc = ImmGetContext(hWnd);
                    if (hImc) {
                        DWORD dwSize = ImmGetCandidateListW(hImc, 0, NULL, 0);
                        if (dwSize) {
                            IM_ASSERT(sizeof(CANDIDATELIST) <= dwSize);
                            if (sizeof(CANDIDATELIST) <= dwSize) {// dwSize は最低でも struct CANDIDATELIST より大きくなければならない

                                (void) (lParam);
                                std::vector<char> candidatelist((size_t) dwSize);
                                if ((DWORD) (std::size(candidatelist) * sizeof(typename decltype(candidatelist)::value_type)) == ImmGetCandidateListW(hImc, 0,
                                                                                                                                                      reinterpret_cast<CANDIDATELIST *>(candidatelist.data()),
                                                                                                                                                      (DWORD) (std::size(candidatelist) * sizeof(typename decltype(candidatelist)::value_type)))) {
                                    const CANDIDATELIST *const cl = reinterpret_cast<CANDIDATELIST *>(candidatelist.data());
                                    comm.candidate_list = std::move(IMMCandidateList::cocreate(cl, dwSize));

#if 0  /* for IMM candidate window debug BEGIN*/
                            {
                                wchar_t dbgbuf[1024];
                                _snwprintf_s(dbgbuf, sizeof(dbgbuf) / sizeof(dbgbuf[0]),
                                    L"lParam = %lld, dwSize = %d , dwCount = %d , dwSelection = %d\n",
                                    lParam,
                                    cl->dwSize,
                                    cl->dwCount,
                                    cl->dwSelection);
                                OutputDebugStringW(dbgbuf);
                                for (DWORD i = 0; i < cl->dwCount; ++i) {
                                    _snwprintf_s(dbgbuf, sizeof(dbgbuf) / sizeof(dbgbuf[0]),
                                        L"%d%s: %s\n",
                                        i, (cl->dwSelection == i ? L"*" : L" "),
                                        (wchar_t*)(candidatelist.data() + cl->dwOffset[i]));
                                    OutputDebugStringW(dbgbuf);
                                }
                            }
#endif /* for IMM candidate window debug END */
                                }
                            }
                        }
                        VERIFY(ImmReleaseContext(hWnd, hImc));
                    }
                }

                    IM_ASSERT(0 <= comm.request_candidate_list_str_commit);
                    if (comm.request_candidate_list_str_commit) {
                        if (comm.request_candidate_list_str_commit == 1) {
                            VERIFY(PostMessage(hWnd, WM_IMGUI_IMM32_COMMAND, WM_IMGUI_IMM32_COMMAND_COMPOSITION_COMPLETE, 0));
                        }
                        --(comm.request_candidate_list_str_commit);
                    }

                    break;
                case IMN_CLOSECANDIDATE: {
                    //OutputDebugStringW(L"IMN_CLOSECANDIDATE\n");
                    comm.show_ime_candidate_list = false;
                } break;
                default:
                    break;
            }
        }
            return ::DefWindowProc(hWnd, uMsg, wParam, lParam);

        case WM_IME_REQUEST:
            return ::DefWindowProc(hWnd, uMsg, wParam, lParam);

        case WM_INPUTLANGCHANGE:
            return ::DefWindowProc(hWnd, uMsg, wParam, lParam);

        case WM_IMGUI_IMM32_COMMAND: {
            switch (wParam) {
                case WM_IMGUI_IMM32_COMMAND_NOP:// NOP
                    return 1;
                case WM_IMGUI_IMM32_COMMAND_SUBCLASSIFY: {
                    ImGuiIO &io = ImGui::GetIO();
                    if (io.ImeWindowHandle) {
                        IM_ASSERT(IsWindow(static_cast<HWND>(io.ImeWindowHandle)));
                        VERIFY(ImmAssociateContextEx(static_cast<HWND>(io.ImeWindowHandle), nullptr, IACE_IGNORENOCONTEXT));
                    }
                }
                    return 1;
                case WM_IMGUI_IMM32_COMMAND_COMPOSITION_COMPLETE:
                    if (!static_cast<bool>(comm.comp_unconv_utf8) ||
                        '\0' == *(comm.comp_unconv_utf8.get())) {
                        /*
                   There is probably no unconverted string after the conversion target.
                   However, since there is now a cursor operation in the
                   keyboard buffer, the confirmation operation needs to be performed
                   after the cursor operation is completed.  For this purpose, an enter
                   key, which is a decision operation, is added to the end of the
                   keyboard buffer.
                */
#if 0
                /*
                  Here, the expected behavior is to perform the conversion completion
                  operation.
                  However, there is a bug that the TextInput loses the focus
                  and the conversion result is lost when the conversion
                  complete operation is performed.

                  To work around this bug, disable it.
                  This is because the movement of the focus of the widget and
                  the accompanying timing are complicated relationships.
                */
                HIMC const hImc = ImmGetContext(hWnd);
                if (hImc) {
                    VERIFY(ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_COMPLETE, 0));
                    VERIFY(ImmReleaseContext(hWnd, hImc));
                }
#else
                        keybd_event(VK_RETURN, 0, 0, 0);
                        keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);
#endif
                    } else {
                        /* Do this to close the candidate window without ending composition. */
                        /*
                  keybd_event (VK_RIGHT, 0, 0, 0);
                  keybd_event (VK_RIGHT, 0, KEYEVENTF_KEYUP, 0);

                  keybd_event (VK_LEFT, 0, 0, 0);
                  keybd_event (VK_LEFT, 0, KEYEVENTF_KEYUP, 0);
                */
                        /*
                   Since there is an unconverted string after the conversion
                   target, press the right key of the keyboard to convert the
                   next clause to IME.
                */
                        keybd_event(VK_RIGHT, 0, 0, 0);
                        keybd_event(VK_RIGHT, 0, KEYEVENTF_KEYUP, 0);
                    }
                    return 1;
                default:
                    break;
            }
        }
            return 1;
        default:
            break;
    }
    return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

BOOL ImGUIIMMCommunication::subclassify_impl(HWND hWnd) {
    IM_ASSERT(IsWindow(hWnd));
    if (!IsWindow(hWnd)) {
        return FALSE;
    }

    /* 在 imgui_imm32_onthespot 中进行 IME 控制，
            因为当 TextWidget 失去焦点时 io.WantTextInput 变为 true-> off
            这时候检查一下IME的状态，如果打开就关闭。
            @see ImGUIIMMCommunication :: operator () () 开头

            亲爱的ImGui的ImGui::IO::ImeWindowHandle原本用来指定CompositionWindow的位置
            我用了它，所以它符合目的

            但是，这种方法不好，因为当有多个目标操作系统窗口时它会崩溃。
    */
    ImGui::GetIO().ImeWindowHandle = static_cast<void *>(hWnd);
    if (::SetWindowSubclass(hWnd, ImGUIIMMCommunication::imm_communication_subClassProc,
                            reinterpret_cast<UINT_PTR>(ImGUIIMMCommunication::imm_communication_subClassProc),
                            reinterpret_cast<DWORD_PTR>(this))) {
        /*
           I want to close IME once by calling imgex::imm_associate_context_disable()

           However, at this point, default IMM Context may not have been initialized by User32.dll yet.
           Therefore, a strategy is to post the message and set the IMMContext after the message pump is turned on.
        */
        return PostMessage(hWnd, WM_IMGUI_IMM32_COMMAND, WM_IMGUI_IMM32_COMMAND_SUBCLASSIFY, 0u);
    }
    return FALSE;
}


ImString::ImString()
    : mData(0), mRefCount(0) {
}

ImString::ImString(size_t len)
    : mData(0), mRefCount(0) {
    reserve(len);
}

ImString::ImString(char *string)
    : mData(string), mRefCount(0) {
    ref();
}

ImString::ImString(const char *string)
    : mData(0), mRefCount(0) {
    if (string) {
        mData = ImStrdup(string);
        ref();
    }
}

ImString::ImString(const ImString &other) {
    mRefCount = other.mRefCount;
    mData = other.mData;
    ref();
}

ImString::~ImString() {
    unref();
}

char &ImString::operator[](size_t pos) {
    return mData[pos];
}

ImString::operator char *() { return mData; }

bool ImString::operator==(const char *string) {
    return strcmp(string, mData) == 0;
}

bool ImString::operator!=(const char *string) {
    return strcmp(string, mData) != 0;
}

bool ImString::operator==(const ImString &string) {
    return strcmp(string.c_str(), mData) == 0;
}

bool ImString::operator!=(const ImString &string) {
    return strcmp(string.c_str(), mData) != 0;
}

ImString &ImString::operator=(const char *string) {
    if (mData)
        unref();
    mData = ImStrdup(string);
    ref();
    return *this;
}

ImString &ImString::operator=(const ImString &other) {
    if (mData && mData != other.mData)
        unref();
    mRefCount = other.mRefCount;
    mData = other.mData;
    ref();
    return *this;
}

void ImString::reserve(size_t len) {
    if (mData)
        unref();
    mData = (char *) ImGui::MemAlloc(len + 1);
    mData[len] = '\0';
    ref();
}

char *ImString::get() {
    return mData;
}

const char *ImString::c_str() const {
    return mData;
}

bool ImString::empty() const {
    return mData == 0 || mData[0] == '\0';
}

int ImString::refcount() const {
    return *mRefCount;
}

void ImString::ref() {
    if (!mRefCount) {
        mRefCount = new int();
        (*mRefCount) = 0;
    }
    (*mRefCount)++;
}

void ImString::unref() {
    if (mRefCount) {
        (*mRefCount)--;
        if (*mRefCount == 0) {
            ImGui::MemFree(mData);
            mData = 0;
            delete mRefCount;
        }
        mRefCount = 0;
    }
}
