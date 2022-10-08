
set_project("MetaDot.Runtime")

add_rules("plugin.vsxmake.autoupdate")

set_languages("clatest", "cxxlatest")
set_arch("x64")

add_rules("mode.debug", "mode.release")

if is_mode("debug") then
    add_defines("CET_DEBUG")
    set_optimize("none")
elseif is_mode("release") then
    add_defines("NDEBUG")
    set_optimize("fastest")
end

include_dir_list = {
    "Source",
    "Source/Vendor/SDL2/include",
    "Source/Engine",
    "Source/Vendor",
    "Source/Vendor/imgui",
    "Source/Vendor/stb",
    "Source/Vendor/miniz",
    "Source/Vendor/glad/include",
    "Source/Vendor/enet",
    "Source/Vendor/box2d/include",
    "Source/Vendor/json/include",

    }

defines_list = {
    "WIN32",
    "WIN64",
    "_WIN32",
    "_WIN64",
    "_WINDOWS",
    "NOMINMAX",
    "UNICODE",
    "_UNICODE",
    "_CRT_SECURE_NO_DEPRECATE",
    "_CRT_SECURE_NO_WARNINGS",
    "_CRT_NONSTDC_NO_DEPRECATE",
    "_SCL_SECURE_NO_WARNINGS",
    "WIN32_LEAN_AND_MEAN",
    "_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING",
    "IMGUI_IMPL_OPENGL_LOADER_GLAD",
    "IMGUI_IMPL_OPENGL_LOADER_CUSTOM",
    "SDL_GPU_DISABLE_GLES",
}

link_list = {
    "DbgHelp",
    "winmm",
    "opengl32",
    "kernel32",
    "user32",
    "gdi32",
    "iphlpapi",
    "Shlwapi",
    "wsock32",
    "ws2_32",
    "shell32",
    "advapi32",
    "imm32",
    "bcrypt",
    "Avrt",
    "dwmapi",
    "Version",
    "Usp10",
    "userenv",
    "psapi",
    "setupapi",
    "ole32",
    "oleaut32",
}

add_linkdirs("Source/Vendor/SDL2/lib/x64")

add_cxxflags(
    "/wd4267", "/wd4244", "/wd4305", "/wd4018", 
    "/wd4800", "/wd5030", "/wd5222", "/wd4554",
    "/wd4002",
    "/utf-8", "/Zc:__cplusplus"
    )

add_cxflags("/bigobj")

target("vendor")
    set_kind("static")
    add_includedirs(include_dir_list)
    add_defines(defines_list)
    add_defines("SPDLOG_COMPILED_LIB")
    add_files("Source/Vendor/**.c")
    add_files("Source/Vendor/**.cpp")
	add_headerfiles("Source/Vendor/**.h")
	add_headerfiles("Source/Vendor/**.hpp")
    set_symbols("debug")

target("CppSource")
    set_kind("shared")
    set_targetdir("./output")
    add_includedirs(include_dir_list)
    add_defines(defines_list)
    add_deps("vendor")
    add_links("SDL2", link_list)
    add_files("Source/CppScript/**.cpp")
	add_headerfiles("Source/CppScript/**.h")
	add_headerfiles("Source/CppScript/**.hpp")
	add_headerfiles("Source/Shared/**.hpp")
    set_symbols("debug")

target("MetaDot")
    set_kind("binary")
    set_targetdir("./output")
    add_includedirs(include_dir_list)
    add_defines(defines_list)
    add_deps("vendor")
    add_links("SDL2", link_list)
    add_files("Source/Engine/**.c")
    add_files("Source/Engine/**.cc")
    add_files("Source/Engine/**.cpp")
	add_headerfiles("Source/Engine/**.h")
	add_headerfiles("Source/Engine/**.hpp")
	add_headerfiles("Source/Engine/**.inl")
    add_headerfiles("Source/Shared/**.hpp")
    set_symbols("debug")
