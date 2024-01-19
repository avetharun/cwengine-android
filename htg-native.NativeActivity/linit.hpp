#include <LuaBridge3/LuaBridge.h>

#include "imgui/imgui.h"
#include "imgui/imgui_format.h"
#include "imgui/imgui_uielement.h"
#include "imgui/imgui_texture.h"


#include "utils.hpp"

#include "rsm_lua.h"
#include "ani.h"
#include "rsm_ui_man.h"
#include <string>
#include "include/abt/cwabt.hpp"


luaaa::LuaClass<ImVec2>* IMVEC2_CLS;
luaaa::LuaClass<ImRect>* IMRECT_CLS;

int SDK_INT_IMPL = 0;
// assumes ANIEnv::app() is initialized. (which in default call order, it is)
void LuaInit(struct android_app* app) {

    GlobalState::LuaState = luaL_newstate();         /* opens Lua */
    luaopen_string(GlobalState::LuaState);
    luaopen_math(GlobalState::LuaState);
    luaopen_base(GlobalState::LuaState);
    luaopen_table(GlobalState::LuaState);
    luaopen_coroutine(GlobalState::LuaState);
    /*IMVEC2_CLS = new luaaa::LuaClass<ImVec2>(GlobalState::LuaState, "Vec2");
    IMVEC2_CLS->ctor<float, float>();
    IMRECT_CLS = new luaaa::LuaClass<ImRect>(GlobalState::LuaState, "Rect");
    IMRECT_CLS->ctor<float, float, float, float>();*/

    for (int i = 0; i < LuaBindable::bindables.size(); i++) {
        LuaBindable::bindables.at(i)->Bind();
    }
    auto lG = luabridge::getGlobalNamespace(GlobalState::LuaState);
    lG.beginNamespace("App")
    .addFunction("openWebURL", [](std::string url) { ani::Networking::openURLBrowser(url.c_str()); })
    .addFunction("setCurrentScreen", [](rsm::GenericHook* screen) {rsm::GenericHook::push(screen); })
    .addFunction("popCurrentScreen", []() {rsm::GenericHook::pop(); })
    .addFunction("getKeyboardHeight", ani::AndroidKeyboardHeight)
    .addFunction("showKeyboard", ani::AndroidShowKeyboard)
    .addFunction("hideKeyboard", ani::AndroidHideKeyboard)
    .addFunction("toggleKeyboard", ani::AndroidToggleKeyboard)
    .addFunction("adbEnabled", ani::adb_enabled)
    .addFunction("refresh_rate", ani::get_refresh_rate)
    .addFunction("android_sdk", ani::Build::SDK_INT)
    .addFunction("SDK_INT", ani::Build::SDK_INT)
    .addFunction("deviceBuildName", ani::Build::GetBuildName)
    .endNamespace()
    .beginNamespace("console")
        .addFunction("print", [](std::string error_msg) {
            cwError::serrof("%s", error_msg.c_str());
        })
        .addFunction("error", [](std::string error_msg) {
            cwError::serrof("%s%s", ImRGB(240, 128, 128).tostring().c_str(), error_msg.c_str());
        })
        .addFunction("warn", [](std::string error_msg) {
            cwError::serrof("%s%s", ImRGB(240, 240, 128).tostring().c_str(), error_msg.c_str());
        })
    .endNamespace()
    .beginClass<ABT>("ABT")
        .addConstructor <void (*) (void)>()
        .addConstructor<void (*) (std::string)>()
        .addConstructor<void (*) (bool)>()
        .addConstructor<void (*) (bool, std::string)>()
        .addConstructor <void (*) (void)>()
        .addFunction("setObject", &ABT::set_object)
        .addFunction("getObject", &ABT::get_object)
        .addFunction("getKeys", &ABT::get_keys)
        .addFunction("getBool", &ABT::get_bool)
        .addFunction("setBool", &ABT::set_bool)
        .addFunction("getString", &ABT::get_string)
        .addFunction("setString", &ABT::set_string)
        .addFunction("getFloat", &ABT::get_float)
        .addFunction("setFloat", &ABT::set_float)
        .addFunction("setInt", &ABT::set_int)
        .addFunction("getInt", &ABT::get_int)
        .addFunction("read", &ABT::decompile)
        .addFunction("write", &ABT::compile);
    auto ABTModu = luaaa::LuaClass<ABT>(GlobalState::LuaState, "abt")
        .ctor()
        .ctor<bool>()
        .ctor<bool,std::string>()
        .ctor<std::string>()
        .fun("setObject", &ABT::set_object)
        .fun("getObject", &ABT::get_object)
        .fun("getKeys", &ABT::get_keys)
        .fun("getBool", &ABT::get_bool)
        .fun("setBool", &ABT::set_bool)
        .fun("getString", &ABT::get_string)
        .fun("setString", &ABT::set_string)
        .fun("getFloat", &ABT::get_float)
        .fun("setFloat", &ABT::set_float)
        .fun("setInt", &ABT::set_int)
        .fun("getInt", &ABT::get_int)
        .fun("read", &ABT::decompile)
        .fun("write", &ABT::compile)
    ;
    GlobalState::luaInitialized = true;
}