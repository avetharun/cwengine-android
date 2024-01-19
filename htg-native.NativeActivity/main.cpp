// dear imgui: standalone example application for Android + OpenGL ES 3
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
#define ALIB_ANDROID

void LuaInit(android_app* app);

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION
#include "input.h"

#include <nlohmann/json/json.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui_texture.h"
#include "imgui/imgui_uielement.h"
#include "imgui/imgui_format.h"
#include "imgui/imgui_markdown.h"
#include "imgui/imgui_impl_android.h"
#include "imgui/imgui_impl_opengl3.h"

#include "utils.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/abt/cwerror.h"


#include "rsm_hooks.h"
#include "rsm_ui_man.h"
#include "rsm_renderer.h"
#include "rsm_lua.h"
#include "ani.h"
#include <zlib.h>
#include <android/choreographer.h>
#include "include/Audio.h"

#include "linit.hpp"
#include "aodui.h"
#include "include/apk_helpers.h"

#include "include/renderer.h"

// Data
static EGLDisplay           g_EglDisplay = EGL_NO_DISPLAY;
static EGLSurface           g_EglSurface = EGL_NO_SURFACE;
static EGLContext           g_EglContext = EGL_NO_CONTEXT;
static bool                 g_Initialized = false;
static char                 g_LogTag[] = "AODMan";

static void handleAppCmd(struct android_app* app, int32_t appCmd);
bool ImGui::LoadTextureFromAPK(const char* filename, GLuint* out_texture, int* out_width, int* out_height) {
    size_t bytes_len = 0;
    LOGV("[FTH] LoadFromAPK file from LoadTextureFromAPK: %s", filename);
    if (AssetExists(filename)) {
        std::string file = ReadAPKFileBytes(filename, &bytes_len);
        LOGV("[FTH] LoadFromAPK numBytes %d", file.length());
        if (file.length() <= 0) {
            LOGV("[FTH] LoadFromAPK file length was zero. Assume an invalid texture!", filename);
            return 0;
        }
        return LoadTextureFromMemory((unsigned char *)file.data(), bytes_len, out_texture, out_width, out_height);
    }
    return 0;
}

const char* test_asset_bytes;
jobject _tv;
float _time = 0.0f;
namespace Icon {
    rsm::ImageAsset app{};
};
struct RSMI : rsm::GenericHook {
    static inline int btn_id = 0;
    static inline rsm::Renderer::Camera activecamera;
    struct T {
        static inline nlohmann::json TranslationKeys{};
        static inline bool hasTranslations;
        static void setd(std::string translation_dict_name = "en-us") {
            std::string fname = "translations/" + translation_dict_name + ".json";
            size_t translation_file_len = 0;
            if (!AssetExists(fname.c_str())) {
                cwError::serrof("Translation file %s not found!", fname.c_str());
                return;
            }
            const char* data = ReadAssetBytes(fname.c_str(), &translation_file_len);
            if (!data) { cwError::serrof("Unable to unpack translation file."); return; }

            TranslationKeys = nlohmann::json::parse(data, data + translation_file_len);

        }
        static std::string getk(std::string key) {
            if (TranslationKeys.size() == 0) { return "No translation keys."; }
            if (!TranslationKeys.contains(key)) { return ("No translation key found for ") + key; }
            return alib_j_getstr(TranslationKeys.at(key));
        }
    };
    static inline rsm::Renderer::Shader shader;
    static inline GLuint VBO = 0;
    static inline GLuint VAO = 0;
    void InitPost() {
        ImGui::SetCurrentFont(rsm::Fonts::default_font);
        T::setd("en-us");
    }
    static inline ani::SharedPreferences getSharedPreferences(const char* name = "alib:mocord") {
        return ani::SharedPreferences(ANIEnv::EnsureEnv(ANIEnv::getVM()), ANIEnv::appClass(), name);
    }
    void PreUpdate() {

    }
    void Render() {
    }
    

    void Start() {
        cwError::onError = DebugConsole::cwErrorHandler;
        // get any settings variables

    }
    void PreSwap() {

        ImGuiContext* g = ImGui::GetCurrentContext();
        float font_sz = g->FontSize;
        g->FontSize = font_sz * 0.5f;
        g->DrawListSharedData.FontSize = font_sz * 0.5f;
        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            cwError::serror("user double tapped");
        }
        g->FontSize = font_sz;
        GlobalState::screen_size = ImGui::GetIO().DisplaySize;
        GlobalState::ui_begin = { 0,(float)ani::AndroidStatusBarHeight() * 0.75f};
        if (rsm::GenericHook::__hook_stack.top()->ShowStatusBar) {
            ImVec2 end_pos = { GlobalState::screen_size.x, GlobalState::ui_begin.y + ani::AndroidStatusBarHeight() * 0.65f };
            ImGui::GetForegroundDrawList()->AddLine({ 0,end_pos.y}, end_pos, ImGui::GetColorU32(ImGuiCol_Border), 4.0f);
            ImGui::GetBackgroundDrawList()->AddRectFilled({ 0,0 }, end_pos, ImGui::GetColorU32(ImGuiCol_Button));
            GlobalState::nav_size = end_pos;
        }
    }
    void PostSwap() {
        btn_id = 0;
    }
    static bool BackArrowModal() { 
        bool retv = false;
        ImGui::PushFont(rsm::Fonts::symbols);
        float fsz_last = ImGui::GetCurrentWindow()->FontWindowScale;
        ImGui::SetWindowFontScale(1.0f);
        ImVec2 tsz = ImGui::CalcTextSize("n");
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0,0 });
        ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
        retv = ImGui::Button(alib_strfmts("n##back_btn_rsmi%d", btn_id++).c_str());
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        ImGui::PopFont();
        ImGui::SetWindowFontScale(fsz_last);
        return retv;
    }
    static bool ForwardArrowModal() {
        bool retv = false;
        ImGui::PushFont(rsm::Fonts::symbols);
        float fsz_last = ImGui::GetCurrentWindow()->FontWindowScale;
        ImGui::SetWindowFontScale(1.0f);
        ImVec2 tsz = ImGui::CalcTextSize("m");
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0,0 });
        ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
        retv = ImGui::Button(alib_strfmts("m##fwrd_btn_rsmi%d", btn_id++).c_str());
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        ImGui::PopFont();
        ImGui::SetWindowFontScale(fsz_last);
        return retv;
    }
};
RSM_HOOK_ENABLE(RSMI);

void PrefsInit(android_app* app) {
    RSMI::getSharedPreferences()
        .getFloat("gui_scale", &GlobalState::GUIScale, 1.0f);
}

lua_State* l_ = GlobalState::LuaState;

struct ui_impl_debug : rsm::GenericHook {
    bool show_app_statistics;
    alib_average<double, 12> average_frametime, average_proctime;
    static inline int SDK_INT = 0;
    // Note: run to get base statistics!
    void Start() {
        SDK_INT = ani::Build::SDK_INT();
    }
    void Render() {
        ImGui::BeginFullscreen();
        ImGui::SetCursorPos(GlobalState::ui_begin);
        if (RSMI::BackArrowModal()) {
            this->pop();
        }
        ImGui::SameLine();
        ImGui::PushFont(rsm::Fonts::symbols);
        if (ImGui::TextButton("4##show_statistics", "4")) {
            show_app_statistics = !show_app_statistics;
        };
        ImGui::PopFont();

        if (show_app_statistics) {
            ImVec2 start = ImVec2{ 0, GlobalState::nav_size.y };
            ImVec2 end = GlobalState::screen_size;
            ImGui::SetNextWindowPos(start);
            ImGui::SetNextWindowSize(end);
            ImGui::BeginFullscreen();
            ImGui::RectFilled(start, end, ImColor(0.0f, 0.0f, 0.0f, 0.75f));
            ImGui::PushFontScale(0.9f);
            double rendertime = abs(difftime(ImGui::GetIO().DeltaTime * 1000.0f, GlobalState::ProcessingTime * .001f));
            double proctime = GlobalState::ProcessingTime * .001;
            average_frametime.add(rendertime);
            average_proctime.add(proctime);
            bool _is_vsync = alib_iswithin(ani::get_refresh_rate(), ImGui::GetIO().Framerate, 1);
            ImGui::Text(u8"Android Build Version					%d", SDK_INT);
            ImGui::Text(u8"Android Build Name						%s", ani::Build::GetBuildName(SDK_INT).c_str());
            ImGui::Text(u8"Rendertime								%4.2fms (%.0ffps) %s",average_frametime.get(), ImGui::GetIO().Framerate, _is_vsync ? "VSYNC": "");
            ImGui::Text(u8"Processing Time							%.2fms", average_proctime.get());
            ImGui::Text(u8"Screen Refresh Rate						%.0ffps", ani::get_refresh_rate());
            ImGui::Text(u8"Using WSA?								%s", (ani::isUsingWSA() ? "True" : "False"));
            ImGui::Text(u8"Using Samsung DEX?						%s", "unknown:unimplemented");
            ImGui::Text(u8"Using Desktop Mode?						%s", "unknown:unimplemented");
            ImGui::PopFontScale();
            ImGui::EndFullscreen();
        }
        else {
            float fsz_last = ImGui::GetCurrentWindow()->FontWindowScale;
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0,0 });
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
            ImGui::BeginDragScrollableChild("##debug_console_window", { GlobalState::ui_begin.x,0 }, false, (show_app_statistics) ? ImGuiWindowFlags_NoInputs : 0);
			ImGui::PushFontScale(0.9f);
            ImGui::TextMulticolored(DebugConsole::debugWindowConsoleText.c_str());
            ImGui::EndDragScrollableChild();
            ImGui::PopStyleVar(2);
			ImGui::PopFontScale();
        }
        ImGui::EndFullscreen();
    }
};
RSM_HOOK_ENABLE(ui_impl_debug);
struct ui_impl_licenses : rsm::GenericHook {
    alib_string __license_text = "[UNFETCHED]";
    void Start() {
    }
    void Render() {
        if (__license_text.compare("[UNFETCHED]") == 0) {
            __license_text = ReadAssetBytes("licenses.txt");
        }
        ImGui::BeginFullscreen();
        ImGui::SetCursorPos(GlobalState::ui_begin);
        if (RSMI::BackArrowModal()) {
            this->pop();
        }
        ImGui::BeginDragScrollableChild("##license_window_pane", { 0,0 }, true);
        float fsz_last = ImGui::GetCurrentWindow()->FontWindowScale;
        ImGui::SetWindowFontScale(0.35f);
        ImGui::TextMulticolored(__license_text);
        ImGui::SetWindowFontScale(fsz_last);
        ImGui::EndDragScrollableChild();
        ImGui::EndFullscreen();
    }
};
RSM_HOOK_ENABLE(ui_impl_licenses);
struct ui_impl_settings : rsm::GenericHook {
    char __itoa_websocket_num[16];
    int e;
	
    void Start() {
    }
    void Render() {
        ImGui::BeginFullscreen();
        ImGui::SetCursorPos(GlobalState::ui_begin);
        if (RSMI::BackArrowModal()) {
            // Commit any values from the settings dialog into shared prefs
            RSMI::getSharedPreferences()
                .edit()
                    .putFloat("ui_scale", GlobalState::GUIScale)
                .commit();
            this->pop();
        }
        
        ImGui::BeginDragScrollableChild("##settings_window_pane", { 0,0 }, true);
        ImGui::SetCursorPos({ 32, 16 });
        // Begin networking settings
        if (ImGui::CollapsingHeader(RSMI::T::getk("settings_adv_modal").c_str())) {
            ImGui::PushFontScale(0.75f);
            ImGui::Text("ui scale"); ImGui::SameLine();
            ImGui::PopFontScale();
            ImGui::SliderFloat("##ui_scale_slider", &GlobalState::GUIScale, 0.1,1.5);
        }
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 512);
        if (ImGui::TextButton("license_btn", RSMI::T::getk("view_licenses_modal").c_str())) {
            this->push(ui_impl_licenses_runner);
        }
        ImGui::UnderlineLast();

        ImGui::EndDragScrollableChild();
        ImGui::EndFullscreen();
    }

};
RSM_HOOK_ENABLE(ui_impl_settings);
struct ui_impl_main : rsm::GenericHook {
	std::shared_ptr<Sprite> sprite;
	std::shared_ptr<Sprite> sprite2;
    void Start() {
        ani::AndroidSetWakeLockState(true);
        if (rsm::GenericHook::__hook_stack.size() == 0) {
            this->push(this);
        }
		if (!click) {
			click = std::make_shared<AudioSound>("click.wav");
		}
		if (!sprite) {
			sprite = Sprite("icons/app.png").get_ptr();
		}
		if (!sprite2) {
			sprite2 = Sprite("icons/ortho.png").get_ptr();
		}
		sprite->enabled = true;
		sprite2->enabled = true;
    }
	std::shared_ptr<AudioSound> click;
    void Render() {
        ImGui::BeginFullscreen();
        ImGui::SetCursorPos(GlobalState::ui_begin);
        ImGui::PushFont(rsm::Fonts::symbols);
        float fsz_last = ImGui::GetCurrentWindow()->FontWindowScale;
        ImGui::SetWindowFontScale(1.0f);
        ImVec2 tsz = ImGui::CalcTextSize("$9\"");
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0,0 });
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImGui::GetStyle().ItemInnerSpacing * 0.5f);
        ImGui::SetCursorPos(GlobalState::ui_begin);

        ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
        if (ImGui::Button("$##settings_btn")) {
            this->push(ui_impl_settings_runner);
			click->Play();
        }
        ImGui::SameLine();
        if (ImGui::Button("9##debug_btn")) {
            cwError::serrof("%sDebug window opened.", ImRGB(255, 255, 0, 0).tostring().c_str());
            this->push(ui_impl_debug_runner);
			click->Play();
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
        ImGui::PopFont();
        ImGui::SetWindowFontScale(fsz_last);

        // End topnav
        ImGui::BeginDragScrollableChild("__main_scrollable", { 0,0 }, false, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::Text("pointer = %p", sprite->Texture->imgid);
        ImGui::Text("size = %d x %d", sprite->Texture->width, sprite->Texture->height);
        //ImGui::Image((void*)(intptr_t)sprite->Texture->imgid, ImVec2(sprite->Texture->width, sprite->Texture->height));
		sprite->transform.position = { (float)sin(ImGui::GetTime() * 0.35f) * 1024.f + (Camera::GetInstance()->getViewport().x * 0.5f) - 512,(float)cos(ImGui::GetTime() * 0.1f) * 200.f + (Camera::GetInstance()->getViewport().x * 0.5f) };
		ImGui::Text("virtual pos = %.3f x %.3f", sprite->transform.position.x, sprite->transform.position.y);
		auto sP = Camera::GetInstance()->pointToScreenViewport(sprite->transform.position);
		ImGui::Text("physical pos = %.3f x %.3f", sP.x, sP.y);
		ImGui::Text("virtual size = %.3f x %.3f", Camera::GetInstance()->m_VirtualViewport.x, Camera::GetInstance()->m_VirtualViewport.y);
		ImGui::Text("physical pos = %.3f x %.3f", Camera::GetInstance()->m_GlobalViewport.x, Camera::GetInstance()->m_GlobalViewport.y);
		ImGui::TextWrapped(u8"あいうえおかき\nくけこさしすせ\nそたちつてとな\nにぬねのはひふ\nへほまみむめも\nやゆよらりるれろ");
        ImGui::EndDragScrollableChild();
        ImGui::EndFullscreen();
        //cwError::serrofUnique("abc");
        Camera::GetInstance()->m_target = &sprite2->transform.position;
		//sprite->center = true;
    }
    void PreSwap() {
    }
};
RSM_HOOK_ENABLE(ui_impl_main);
void tick() {
    if (ImGui::GetCurrentContext() == 0) {
        ImGui::SetCurrentContext(ImGui::CreateContext());
    }
    std::chrono::time_point<std::chrono::system_clock> start, end;
    if (g_EglDisplay == EGL_NO_DISPLAY)
        return;
    ImGuiIO& io = ImGui::GetIO();
    Camera::GetInstance()->m_GlobalViewport = io.DisplaySize;
	//Camera::GetInstance()->m_Offset = (io.DisplaySize / 2) - (Camera::GetInstance()->getViewport() / 2);
    start = std::chrono::system_clock::now();
    // account previous frames for the gestures. Used for delta processing.
    rsm::m_DragPositionL = rsm::DragPosition;
    rsm::m_DoubleTapPositionL = rsm::DoubleTapPosition;
    rsm::m_PinchPositionBeginL = rsm::PinchPositionBegin;
    rsm::m_PinchPositionEndL = rsm::PinchPositionEnd;

    ndk_helper::Vec2 drag; rsm::drag_detector_.GetPointer(drag);
    drag.Value(rsm::DragPosition.x, rsm::DragPosition.y);
    ndk_helper::Vec2 pinch1, pinch2; rsm::pinch_detector_.GetPointers(pinch1, pinch2);
    pinch1.Value(rsm::PinchPositionBegin.x, rsm::PinchPositionBegin.y); pinch2.Value(rsm::PinchPositionEnd.x, rsm::PinchPositionEnd.y);
    int32_t window_width = ANativeWindow_getWidth(ANIEnv::app()->window);
    int32_t window_height = ANativeWindow_getHeight(ANIEnv::app()->window);
    int32_t half_width = lroundf((float)window_width * 0.5f);
    int32_t half_height = lroundf((float)window_height * 0.5f);
    _time += io.DeltaTime / 1000.0f;
    // Open on-screen (soft) input if requested by Dear ImGui
    static bool WantTextInputLast = false;
    if ((io.WantTextInput && !WantTextInputLast) || (!io.WantTextInput && WantTextInputLast))
        ani::AndroidShowKeyboard();
    if (!io.WantTextInput && !WantTextInputLast) {
        ani::AndroidHideKeyboard();
    }
    WantTextInputLast = io.WantTextInput;

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, window_width, window_height);
    glClearColor(0x2a / 255.0f, 0x29 / 255.0f, 0x2e / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    io.DisplayFramebufferScale = {GlobalState::GUIScale, GlobalState::GUIScale };
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame();
    ImGui::NewFrame();
    RSMI_runner->PreUpdate();
    rsm::HookManager::RunPreUpdate();
    rsm::HookManager::RunUpdate();
    rsm::HookManager::RunPostUpdate();
    // Rendering
    rsm::HookManager::RunPreRender();
    ImGui::BeginFullscreen();
    Camera::GetInstance()->Update();
    BatchRenderer::Render();
    ImGui::EndFullscreen();
    rsm::HookManager::RunRender();
    rsm::HookManager::RunPreSwap();
    RSMI_runner->PreSwap();

    rsm::HookManager::RunPreUI();

    end = std::chrono::system_clock::now();
    std::chrono::microseconds elapsed_micros_pre = end - start;
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    rsm::HookManager::RunPostRender();
    eglSwapBuffers(g_EglDisplay, g_EglSurface);
    start = std::chrono::system_clock::now();
    ImGui::ElementStubImpl::ResetOffsets();
    rsm::HookManager::RunPostSwap();
    RSMI_runner->PostSwap();
    rsm::MultiFingerL = rsm::MultiFinger;
    end = std::chrono::system_clock::now();
    std::chrono::microseconds elapsed_micros = end - start;
    GlobalState::ProcessingTime = elapsed_micros.count() + elapsed_micros_pre.count();
}

void shutdown()
{
    if (!g_Initialized)
        return;

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplAndroid_Shutdown();
	//ImGui::Shutdown(); // This line of code often causes a segfault, which is on ImGui's end. Not going to bother, memory leaks my beloved! (the font is already destroyed at this point)
    try {
        if (g_EglDisplay != EGL_NO_DISPLAY)
        {
            eglMakeCurrent(g_EglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

            if (g_EglContext != EGL_NO_CONTEXT)
                //eglDestroyContext(g_EglDisplay, g_EglContext);

            if (g_EglSurface != EGL_NO_SURFACE)
                eglDestroySurface(g_EglDisplay, g_EglSurface);

            eglTerminate(g_EglDisplay);
        }
    }
    catch (std::exception E) {
        cwError::serrof("Error upon destroying EGL context: %s", E.what());
    }

    g_EglDisplay = EGL_NO_DISPLAY;
    g_EglContext = EGL_NO_CONTEXT;
    g_EglSurface = EGL_NO_SURFACE;
    ANativeWindow_release(ANIEnv::app()->window);

    g_Initialized = false;
}
void rsm::Renderer::TransformPosition(ndk_helper::Vec2& vec) {
    ImGuiIO io = ImGui::GetIO();
        vec = ndk_helper::Vec2(2.0f, 2.0f) * vec
            / ndk_helper::Vec2(io.DisplaySize.x,
                io.DisplaySize.y) - ndk_helper::Vec2(1.f, 1.f);

}

void init(struct android_app* app)
{
    if (g_Initialized)
        return;
    ANativeWindow_acquire(app->window);

    // Initialize EGL
    // This is mostly boilerplate code for EGL...
    {
        g_EglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (g_EglDisplay == EGL_NO_DISPLAY)
            __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "[FTH] %s", "eglGetDisplay(EGL_DEFAULT_DISPLAY) returned EGL_NO_DISPLAY");

        if (eglInitialize(g_EglDisplay, 0, 0) != EGL_TRUE)
            __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "[FTH] %s", "eglInitialize() returned with an error");

        const EGLint egl_attributes[] = { EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, EGL_DEPTH_SIZE, 24, EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE };
        EGLint num_configs = 0;
        if (eglChooseConfig(g_EglDisplay, egl_attributes, nullptr, 0, &num_configs) != EGL_TRUE)
            __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "[FTH] %s", "eglChooseConfig() returned with an error");
        if (num_configs == 0)
            __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "[FTH] %s", "eglChooseConfig() returned 0 matching config");

        // Get the first matching config
        EGLConfig egl_config;
        eglChooseConfig(g_EglDisplay, egl_attributes, &egl_config, 1, &num_configs);
        EGLint egl_format;
        eglGetConfigAttrib(g_EglDisplay, egl_config, EGL_NATIVE_VISUAL_ID, &egl_format);
        ANativeWindow_setBuffersGeometry(app->window, 0, 0, egl_format);

        const EGLint egl_context_attributes[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
        g_EglContext = eglCreateContext(g_EglDisplay, egl_config, EGL_NO_CONTEXT, egl_context_attributes);

        if (g_EglContext == EGL_NO_CONTEXT)
            __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "[FTH] %s", "eglCreateContext() returned EGL_NO_CONTEXT");

        g_EglSurface = eglCreateWindowSurface(g_EglDisplay, egl_config, app->window, NULL);
        eglMakeCurrent(g_EglDisplay, g_EglSurface, g_EglSurface, g_EglContext);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

        rsm::HookManager::RunInitGL();
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    auto c = ImGui::CreateContext();
    ImGui::SetCurrentContext(c);
    ImGuiIO& io = ImGui::GetIO();

    // Disable loading/saving of .ini file from disk.
    // FIXME: Consider using LoadIniSettingsFromMemory() / SaveIniSettingsToMemory() to save in appropriate location for Android.
    io.IniFilename = NULL;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplAndroid_Init(ANIEnv::app()->window);
    ImGui_ImplOpenGL3_Init("#version 300 es");
    cwError::onError = DebugConsole::cwErrorHandler;
    PrefsInit(app);
    float dpiConstant = 0.015f;
    float fontDPIConstant = 4.f;
    float fixedDPI = ani::getDPI() * GlobalState::GUIScale * dpiConstant;
    // Load Fonts
    ImVector<ImWchar> ranges;
    ImFontGlyphRangesBuilder builder;
    LOGI("Font Pixel width: %f", 2048 / (fixedDPI * fontDPIConstant));
    static size_t whitney_font_sz = 0; static void* whitney_font_data = (void*)ReadAssetBytes("fonts/HG_KK_LT_CYR_GRK_HNG.ttf", &whitney_font_sz);
	LOGI("[FTH] Font size: %d", whitney_font_sz);

	static ImFontConfig config;
	static const ImWchar* _ranges = new ImWchar[2]{ 0x0020, 0x00ff };
	config.MergeMode = true;
	rsm::Fonts::default_font = io.Fonts->AddFontFromMemoryTTF(whitney_font_data, whitney_font_sz, 2 * (fixedDPI * fontDPIConstant), __null, io.Fonts->GetGlyphRangesDefault());
	io.Fonts->AddFontFromMemoryTTF(whitney_font_data, whitney_font_sz, 2 * (fixedDPI * fontDPIConstant), &config, io.Fonts->GetGlyphRangesJapanese());


    // pack as many chars as we can into the symbols font
	static size_t symbol_font_sz{}; static void* symbols_font_data = (void*)ReadAssetBytes("symbols.ttf", &symbol_font_sz);
    rsm::Fonts::symbols = io.Fonts->AddFontFromMemoryTTF(symbols_font_data, symbol_font_sz, 2 * (fixedDPI * fontDPIConstant), __null, _ranges);
	//rsm::Fonts::symbols = rsm::Fonts::default_font;

    io.Fonts->Build();

    ImGui::GetStyle().ScaleAllSizes(fixedDPI);

    g_Initialized = true;
    size_t uimglen = 0;
    // Initialize Android Native Interface
    ImGui::GetStyle().ScrollbarSize = 12;
    rsm::doubletap_detector_ = {};
    rsm::drag_detector_ = {};
    rsm::pinch_detector_ = {};
    io.MouseDoubleClickMaxDist = 128/*px*/;
    // run starter hooks
    Sprite::resetCache();
    rsm::HookManager::RunInitPost();
    rsm::HookManager::RunStart();
    rsm::HookManager::RunPostStart();
	auto t = ANIEnv::getRootView();
}

static void handleAppCmd(struct android_app* app, int32_t appCmd)
{
    switch (appCmd)
    {
    case APP_CMD_SAVE_STATE:
        break;
    case APP_CMD_INIT_WINDOW:
        init(app);
        break;
    case APP_CMD_TERM_WINDOW:
        shutdown();
        break;
    case APP_CMD_GAINED_FOCUS:
        break;
    case APP_CMD_LOST_FOCUS:
        break;
    case APP_CMD_WINDOW_REDRAW_NEEDED:
        break;
    case APP_CMD_START:



        break;
    }
}
static int32_t handleInputEvent(struct android_app* app, AInputEvent* inputEvent)
{
	long t = AInputEvent_getType(inputEvent);
	switch (t) {
		case AINPUT_EVENT_TYPE_MOTION: {
			if (AInputEvent_getSource(inputEvent) == AINPUT_SOURCE_MOUSE) {
				long s = AMotionEvent_getButtonState(inputEvent);

				switch (AMotionEvent_getAction(inputEvent)) {
					case AMOTION_EVENT_ACTION_DOWN: break;
				}
				break;
			}
			ndk_helper::GESTURE_STATE doubleTapState = rsm::doubletap_detector_.Detect(inputEvent);
			ndk_helper::GESTURE_STATE dragState = rsm::drag_detector_.Detect(inputEvent);
			ndk_helper::GESTURE_STATE pinchState = rsm::pinch_detector_.Detect(inputEvent);
			rsm::DoubleTapped = false;
			rsm::MultiFinger = rsm::tap_camera_.pinching_;
			//Double tap detector has a priority over other detectors
			Input::Mouse::m1df = !doubleTapState;
			Input::Mouse::m3df = doubleTapState;
			if (doubleTapState == ndk_helper::GESTURE_STATE_ACTION)
			{
				rsm::DoubleTapped = rsm::rsm_DoubleTapped_First;
			}
			else
			{
				//Handle drag state
				if (dragState & ndk_helper::GESTURE_STATE_START)
				{
					//Otherwise, start dragging
					ndk_helper::Vec2 v;
					rsm::drag_detector_.GetPointer(v);
					rsm::Renderer::TransformPosition(v);
					rsm::tap_camera_.BeginDrag(v);
				}
				// ...else other possible drag states...

				//Handle pinch state
				if (pinchState & ndk_helper::GESTURE_STATE_START)
				{
					//Start new pinch
					ndk_helper::Vec2 v1;
					ndk_helper::Vec2 v2;
					rsm::pinch_detector_.GetPointers(v1, v2);
					rsm::Renderer::TransformPosition(v1);
					rsm::Renderer::TransformPosition(v2);
					rsm::tap_camera_.BeginPinch(v1, v2);
				}
				// ...else other possible pinch states...
			}
		}break;
	}

    if (AKeyEvent_getAction(inputEvent) == AKEY_EVENT_ACTION_DOWN && AInputEvent_getType(inputEvent) == AINPUT_EVENT_TYPE_KEY)
    {
        if (AKeyEvent_getKeyCode(inputEvent) == AKEYCODE_BACK) {
            // actions on back key
            if (rsm::GenericHook::__hook_stack.size() == 1) {
                return 0;
                // We're on the root element, we should instead of popping the last view, go to the home screen.
            }
            rsm::GenericHook::pop();
            return 1;
        };
        int key = AKeyEvent_getKeyCode(inputEvent);
        int metaState = AKeyEvent_getMetaState(inputEvent);
        int uniValue;
        if (metaState != 0)
            uniValue = ani::GetUnicodeChar(app, AKEY_EVENT_ACTION_DOWN, key, metaState);
        else
            uniValue = ani::GetUnicodeChar(app, AKEY_EVENT_ACTION_DOWN, key, 0);
        ImGui::GetIO().AddInputCharacterUTF16(uniValue);
		rsm::GenericHook::__hook_stack.top()->KeyPressed(key);
		Input::Keyboard::EmulateKeyboardDown(key);
	}
	else if (AKeyEvent_getAction(inputEvent) == AKEY_EVENT_ACTION_UP && AInputEvent_getType(inputEvent) == AINPUT_EVENT_TYPE_KEY) {
		int key = AKeyEvent_getKeyCode(inputEvent);
		Input::Keyboard::EmulateKeyboardUp(key);
	}
    return ImGui_ImplAndroid_HandleInputEvent(inputEvent);
}
void android_main(struct android_app* app)
{
    app->onAppCmd = handleAppCmd;
    app->onInputEvent = handleInputEvent;
    if (ANIEnv::init(app->activity->vm, "com.feintha.ANI", app->activity->clazz, app) == 0) {
        LOGF("Failed to initialize Java VM!");
    };
    //PrefsInit(app);
    while (true)
    {
        int out_events;
        struct android_poll_source* out_data;

        // Poll all events. If the app is not visible, this loop blocks until g_Initialized == true.
        while (ALooper_pollAll(g_Initialized ? 0 : 10000, NULL, &out_events, (void**)&out_data) >= 0)
        {
            // Process one event
            if (out_data != NULL)
                out_data->process(app, out_data);

            // Exit the app by returning from within the infinite loop
            if (app->destroyRequested != 0)
            {
                // shutdown() should have been called already while processing the
                // app command APP_CMD_TERM_WINDOW. But we play save here
                if (!g_Initialized)
                    shutdown();

                return;
            }
        }
        if (g_Initialized) {
            // Initiate a new frame
            tick();
			Input::Keyboard::update();
			Input::Mouse::update();
        }
    }
}
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {

    return JNI_VERSION_1_6;
}
#define ALIB_IMPLEMENTATION
#include "utils.hpp"