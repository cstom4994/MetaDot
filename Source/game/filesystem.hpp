// Copyright(c) 2022, KaoruXun All rights reserved.

#ifndef _METADOT_FILESYSTEM_HPP_
#define _METADOT_FILESYSTEM_HPP_

#include "core/core.hpp"
#include "core/debug_impl.hpp"
#include "core/macros.h"
#include "game/utils.hpp"
#include <filesystem>

#define FUTIL_ASSERT_EXIST(stringPath)                                                             \
    METADOT_ASSERT(FUtil::exists(stringPath),                                                      \
                   ("%s", MetaEngine::Format("FILE: {0} does not exist", stringPath)))

class ResourceWorker {
private:
    static std::string s_ProjectRootPath;
    static std::string s_DataPath;

public:
    static std::string GetResourceLoc(std::string_view resPath);
    static std::string GetLocalPath(std::string_view resPath);
    static void init();
    static const std::string &getDataPath();
};

#define METADOT_RESLOC(x) ResourceWorker::GetResourceLoc(x)

#define METADOT_RESLOC_STR(x) METADOT_RESLOC(x).c_str()

namespace FUtil {
    inline bool exists(std::string_view path) {
        return std::filesystem::exists(METADOT_RESLOC(path));
    }

    //inline void assertFileExists(std::string_view path) {
    //	ASSERT(FUtil::exists(ND_RESLOC(path)), "FILE: {0} does not exist", path);
    //}
    // folder path of current executable
    const std::string &getExecutableFolderPath();

    typedef int FileSearchFlags;

    enum FileSearchFlags_ {
        FileSearchFlags_None = 0,
        FileSearchFlags_Recursive = 1 << 0,
        FileSearchFlags_OnlyDirectories = 1 << 1,
        FileSearchFlags_OnlyFiles = 1 << 2,
        // return newest folder/file with in folder
        FileSearchFlags_Newest = 1 << 3,
        // return oldest folder/file with in folder
        FileSearchFlags_Oldest = 1 << 4,
    };

    std::vector<std::string> fileList(std::string_view folder_path,
                                      FileSearchFlags flags = FileSearchFlags_None);

    // absolute path of fileName in the same folder as executable
    // or "" if filename doesn't exist
    std::string getAbsolutePath(const char *fileName);

    void cleanPathString(std::string &s);

    inline std::string cleanPathStringConst(std::string_view s) {
        auto out = std::string(s);
        cleanPathString(out);
        return out;
    }

    inline void removeSuffix(std::string &s) {
        if (s.find_last_of('.') != std::string::npos) s = s.substr(0, s.find_last_of('.'));
    }

    inline std::string removeSuffixConst(std::string_view s) {
        auto out = std::string(s);
        removeSuffix(out);
        return out;
    }

    inline bool copyFile(std::string_view src, std::string_view dest) {
        std::filesystem::copy(src, dest);
        return true;
    }

    inline std::string GetFileName(std::string &s) {
        std::filesystem::path p(s);
        return p.stem();
    }

    std::string readFileString(std::string_view path);

    uint64_t lastWriteTime(std::string_view path);
}// namespace FUtil

#endif