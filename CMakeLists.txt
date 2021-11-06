#/****************************************************************************
# Copyright (c) 2013-2014 cocos2d-x.org
# Copyright (c) 2015-2017 Chukong Technologies Inc.
#
# http://www.cocos2d-x.org
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# ****************************************************************************/

cmake_minimum_required(VERSION 3.21)

# set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
# set(CMAKE_POLICY_DEFAULT_CMP0072 NEW)

set(APP_NAME cc2dx_test)

project(${APP_NAME})

# list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
# include(DownloadProject)

set(THIRDPARTY ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)

option(LIBIGL_WITH_TRIANGLE           "" ON)
option(LIBIGL_USE_STATIC_LIBRARY      "" OFF)
option(LIBIGL_WITH_CGAL               "" OFF)
option(LIBIGL_WITH_COMISO             "" OFF)
option(LIBIGL_WITH_CORK               "" OFF)
option(LIBIGL_WITH_EMBREE             "" OFF)
option(LIBIGL_WITH_MATLAB             "" OFF)
option(LIBIGL_WITH_MOSEK              "" OFF)
option(LIBIGL_WITH_OPENGL             "" OFF)
option(LIBIGL_WITH_OPENGL_GLFW        "" OFF)
option(LIBIGL_WITH_OPENGL_GLFW_IMGUI  "" OFF)
option(LIBIGL_WITH_PNG                "" OFF)
option(LIBIGL_WITH_TETGEN             "" OFF)
option(LIBIGL_WITH_PREDICATES         "" OFF)
option(LIBIGL_WITH_XML                "" OFF)
option(LIBIGL_WITHOUT_COPYLEFT        "" OFF)
option(LIBIGL_EXPORT_TARGETS          "" OFF)
option(LIBIGL_BUILD_TESTS             "" OFF)
option(LIBIGL_BUILD_TUTORIALS         "" OFF)

list(APPEND CMAKE_MODULE_PATH "${THIRDPARTY}/libigl/cmake")
include(libigl)

# download_thirdparty(chipmunk
#     GIT_REPOSITORY https://github.com/slembcke/Chipmunk2D
#     GIT_TAG Chipmunk-7.0.1)

#     PATCH_COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/patch/Chipmunk-7.0.1_cocos.patch)

# set(BUILD_DEMOS                     OFF CACHE BOOL "" FORCE)
# set(INSTALL_DEMOS                   OFF CACHE BOOL "" FORCE)
# set(BUILD_SHARED                    OFF CACHE BOOL "" FORCE)
# set(BUILD_STATIC                    ON CACHE BOOL "" FORCE)
# set(INSTALL_STATIC                  ON CACHE BOOL "" FORCE)
# add_subdirectory(${THIRDPARTY}/chipmunk)

# download_thirdparty(cocos
#     GIT_REPOSITORY      https://github.com/cocos2d/cocos2d-x
#     GIT_TAG             v4)

# set(BUILD_SHARED_LIBS               OFF CACHE BOOL "" FORCE)
# download_thirdparty(clipper
#     GIT_REPOSITORY      https://github.com/Geri-Borbas/Clipper
#     GIT_TAG             master)
# add_subdirectory(${THIRDPARTY}/clipper/cpp)


set(BUILD_EXT_CHIPMUNK OFF CACHE BOOL "" FORCE)
set(BUILD_EXT_CLIPPER OFF CACHE BOOL "" FORCE)
# set(BUILD_EXT_RECAST OFF CACHE BOOL "" FORCE)
# BUILD_EXT_CLIPPER
# BUILD_EXT_RECAST
# BUILD_EXT_POLY2TRI

set(COCOS2DX_PROJECT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cocos-project)
# add_subdirectory(${COCOS2DX_PROJECT_PATH}/cocos2d)

# link_libraries(
#     chipmunk
# #     RecastNavigation::Detour RecastNavigation::DebugUtils RecastNavigation::DetourCrowd RecastNavigation::DetourTileCache RecastNavigation::Recast
#     polyclipping
# )

# include_directories(
#     ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chipmunk/include
# )

if(XCODE)
    if(NOT DEFINED CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET)
        SET (CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 8.0)
    endif()
endif()

if(NOT DEFINED BUILD_ENGINE_DONE) # to test cc2dx_test into root project
    set(COCOS2DX_ROOT_PATH ${COCOS2DX_PROJECT_PATH}/cocos2d)
    set(CMAKE_MODULE_PATH ${COCOS2DX_ROOT_PATH}/cmake/Modules/)

    include(CocosBuildSet)
    add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos ${ENGINE_BINARY_PATH}/cocos/core)
endif()

# record sources, headers, resources...
set(GAME_SOURCE)
set(GAME_HEADER)

set(GAME_RES_FOLDER
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
    )
if(APPLE OR WINDOWS)
    cocos_mark_multi_resources(common_res_files RES_TO "Resources" FOLDERS ${GAME_RES_FOLDER})
endif()

# add cross-platforms source files and header files 
file(GLOB_RECURSE GAME_SOURCE CONFIGURE_DEPENDS src/*.cpp src/*.cc ${COCOS2DX_ROOT_PATH}/external/recast/Recast/*.cpp)
file(GLOB_RECURSE GAME_HEADER CONFIGURE_DEPENDS src/*.hpp src/*.h src/*.hh)

if(ANDROID)
    # change APP_NAME to the share library name for Android, it's value depend on AndroidManifest.xml
    set(APP_NAME MyGame)
    list(APPEND GAME_SOURCE
         ${COCOS2DX_PROJECT_PATH}/proj.android/app/jni/hellocpp/main.cpp
         )
elseif(LINUX)
    list(APPEND GAME_SOURCE
         ${COCOS2DX_PROJECT_PATH}/proj.linux/main.cpp
         )
elseif(WINDOWS)
    list(APPEND GAME_HEADER
         ${COCOS2DX_PROJECT_PATH}/proj.win32/main.h
         ${COCOS2DX_PROJECT_PATH}/proj.win32/resource.h
         )
    list(APPEND GAME_SOURCE
         ${COCOS2DX_PROJECT_PATH}/proj.win32/main.cpp
         ${COCOS2DX_PROJECT_PATH}/proj.win32/game.rc
         ${common_res_files}
         )
elseif(APPLE)
    if(IOS)
        list(APPEND GAME_HEADER
             ${COCOS2DX_PROJECT_PATH}/proj.ios_mac/ios/AppController.h
             ${COCOS2DX_PROJECT_PATH}/proj.ios_mac/ios/RootViewController.h
             )
        set(APP_UI_RES
            ${COCOS2DX_PROJECT_PATH}/proj.ios_mac/ios/LaunchScreen.storyboard
            ${COCOS2DX_PROJECT_PATH}/proj.ios_mac/ios/LaunchScreenBackground.png
            ${COCOS2DX_PROJECT_PATH}/proj.ios_mac/ios/Images.xcassets
            )
        list(APPEND GAME_SOURCE
             ${COCOS2DX_PROJECT_PATH}/proj.ios_mac/ios/main.m
             ${COCOS2DX_PROJECT_PATH}/proj.ios_mac/ios/AppController.mm
             ${COCOS2DX_PROJECT_PATH}/proj.ios_mac/ios/RootViewController.mm
             ${COCOS2DX_PROJECT_PATH}/proj.ios_mac/ios/Prefix.pch
             ${APP_UI_RES}
             )
    elseif(MACOSX)
        set(APP_UI_RES
            ${COCOS2DX_PROJECT_PATH}/proj.ios_mac/mac/Icon.icns
            ${COCOS2DX_PROJECT_PATH}/proj.ios_mac/mac/Info.plist
            )
        list(APPEND GAME_SOURCE
             ${COCOS2DX_PROJECT_PATH}/proj.ios_mac/mac/main.cpp
             ${COCOS2DX_PROJECT_PATH}/proj.ios_mac/mac/Prefix.pch
             ${APP_UI_RES}
             )
    endif()
    list(APPEND GAME_SOURCE ${common_res_files})
endif()

# mark app complie info and libs info
set(all_code_files
    ${GAME_HEADER}
    ${GAME_SOURCE}
    )
if(NOT ANDROID)
    add_executable(${APP_NAME} ${all_code_files})
else()
    add_library(${APP_NAME} SHARED ${all_code_files})
    add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos/platform/android ${ENGINE_BINARY_PATH}/cocos/platform)
    target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)
endif()

# add_subdirectory(thirdparty/triangle)

# message(STATUS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libigl/cmake)

# list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libigl/cmake)
# set(LIBIGL_WITH_TRIANGLE ON)
# include(libigl)


target_link_libraries(${APP_NAME} PRIVATE
    cocos2d
    igl::core igl::triangle
    chipmunk
    polyclipping
    ext_recast
    # delaunator
)

target_compile_features(${APP_NAME} PUBLIC cxx_std_17)

target_include_directories(${APP_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${COCOS2DX_ROOT_PATH}
        ${COCOS2DX_ROOT_PATH}/cocos/audio/include/
)

# mark app resources
setup_cocos_app_config(${APP_NAME})
if(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")
    set_xcode_property(${APP_NAME} INSTALL_PATH "\$(LOCAL_APPS_DIR)")

    if(MACOSX)
        set_xcode_property(${APP_NAME} INFOPLIST_FILE "${COCOS2DX_PROJECT_PATH}/proj.ios_mac/mac/Info.plist")
    elseif(IOS)
        set_xcode_property(${APP_NAME} INFOPLIST_FILE "${COCOS2DX_PROJECT_PATH}/proj.ios_mac/ios/Info.plist")
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
    endif()

    # For code-signing, set the DEVELOPMENT_TEAM:
    #set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "GRLXXXX2K9")
elseif(WINDOWS)
    cocos_copy_target_dll(${APP_NAME})
endif()

if(LINUX OR WINDOWS)
    cocos_get_resource_path(APP_RES_DIR ${APP_NAME})
    cocos_copy_target_res(${APP_NAME} LINK_TO ${APP_RES_DIR} FOLDERS ${GAME_RES_FOLDER})
endif()
