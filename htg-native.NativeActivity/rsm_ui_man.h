#ifndef __rosemary_project_ui_mgr_h_
#define __rosemary_project_ui_mgr_h_
#include "rsm_hooks.h"
#include <set>
#include <luaaa.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

struct DebugConsole {
	static inline ImGuiTextBuffer debugWindowConsoleText = {};
	// Deletes buffer in sizes of 512 if the size of the buffer exceeds 4012
	static void pushuf(std::string cstr) {
		if (&debugWindowConsoleText == nullptr) { return; }
		debugWindowConsoleText.append(cstr.c_str());
	}
	static void pushf(std::string fmt, ...) {
		if (&debugWindowConsoleText == nullptr) { return; }
		va_list args;
		va_start(args, fmt);
		debugWindowConsoleText.appendfv(fmt.c_str(), args);
		va_end(args);
	}
	static void cwErrorHandler(const char* errs, uint32_t errid) {
		// errid not used
		std::string _errs = errs;
		pushuf(_errs + "\n[/#/]");
	}
};

struct GlobalState {
	static inline bool luaInitialized = false;
	// Keyboard height in pixels
	static inline int KeyboardHeight;
	// Height of the status (notification) bar in pixels
	static inline int StatusBarHeight;
	static inline bool statusbar_visible = false;
	static inline int DisplayHeightTotal = 0;
	static inline bool keyboard_visible = false;
	static inline ImVec2 screen_size = {};
	static inline ImVec2 ui_begin = {};
	static inline ImVec2 nav_size = {};
	static inline bool ShowSettingsWindow = false;
	static inline bool ShowMainWindow = true;
	static inline bool ShowDebugWindow = false;
	static inline bool ShowCameraWindow = false;
	static inline bool ShowCharSelectWindow = false;
	static inline bool ShowCharAddWindow = false;
	static inline bool ShowLicenseWindow = false;
	static inline bool ShowCreditsWindow = false;
	// Processing time in microseconds
	static inline long double ProcessingTime = 0.0L;
	// Settings
	static inline lua_State* LuaState = nullptr;
	static inline float GUIScale = 1.0f;
	struct LuaScreen : rsm::GenericHook {
		std::string name = "";
		void_0a_f render_gl = []() {};
		void_0a_f render_ui = []() {};
		void_0a_f update = []() {};
		void_0a_f init = []() {};
		void Start() { init(); }
		void Update() { update(); }
		void Render() { render_gl(); }
		void_1i_f key_pressed = [](int i) {};
		void_1i_f key_held = [](int i) {};
		void_1i_f key_released = [](int i) {};
		void_2i_f screen_tapped = [](int x, int y) {};
		void_4i_f screen_touch_movement = [](int px, int py, int dx, int dy) {};
		// Key pressed on physical or virtual keyboard
		void KeyPressed(int scancode) { key_pressed(scancode); }
		// Runs whenever a key is held down every frame
		void KeyHeld(int scancode) { key_held(scancode); }
		// Key released on physical or virtual keyboard
		void KeyReleased(int scancode) { key_released(scancode); }
		// Touchscreen touched
		void TouchSent(int x, int y) { screen_tapped(x, y); }
		// Touchscreen touch moved
		void TouchMove(int delta_x, int delta_y, int last_x, int last_y) { screen_touch_movement(last_x, last_y, delta_x, delta_y); }
		void PreUI() { render_ui(); }
	};
};


namespace rsm {
	static const ImColor BLURPLE = ImU32(0x5865F2);
	static const ImColor WHITE = ImU32(0xf1f1f1);
	static const ImColor GREEN = ImU32(0x57F287);
	static const ImColor YELLOW = ImU32(0xFEE75C);
	static const ImColor RED = ImU32(0xED4245);
	static const ImColor BLACK = ImU32(0x23272A);
	static const ImColor FUSCHIA = ImU32(0xEB459E);
	struct ColorPair {
		ImColor foreground;
		ImColor background;
		// Normally unused, for tertiary colors.
		ImColor tertiary {};
	};
	static const ColorPair BLURPLE_WHITE{ WHITE, BLURPLE };
	static const ColorPair BLURPLE_BLACK{ BLACK, BLURPLE };
	static const ColorPair BLURPLE_GREEN{ GREEN, BLURPLE };
	static const ColorPair BLURPLE_YELLOW{ YELLOW, BLURPLE };
	struct Fonts { 
		static inline ImFont* symbols = nullptr;
		static inline ImFont* default_font = nullptr;
		static inline ImFont* jp = nullptr;
		static inline ImFont* kr = nullptr;
		static inline ImFont* proggy = nullptr;
	};
	// Layout for a given scrne
	struct Layout {
		// Disable projecting the globalState::ui_begin vec onto the element
		bool DisableGlobalUIBegin;
		bool top, bottom, left, right, center;
		ImVec2 CalcElement(ImVec2 elem_size) {
			ImVec2 pos;
			if (top) {
				pos.y = 0;
			}
			if (bottom) {
				pos.y = GlobalState::screen_size.y - elem_size.y - ImGui::GetStyle().ItemSpacing.x;
			}
			if (left) {
				pos.x = 0;
			}
			if (right) {
				pos.x = GlobalState::screen_size.x - elem_size.x - ImGui::GetStyle().ItemSpacing.x;
			}
			if (!DisableGlobalUIBegin) {
				pos = ImVec2{ pos.x + GlobalState::ui_begin.x, pos.y + GlobalState::ui_begin.y };
			}
			return {};
		}
		bool Button(const char* label) {
			ImVec2 cspos = CalcElement(ImGui::CalcTextSize(label));
			ImGui::SetCursorScreenPos(cspos);
			return ImGui::Button(label);
		}
		void TextV(const char* fmt, va_list args) {
			const char* text = alib_strfmtv(fmt, args);
			ImVec2 cspos = CalcElement(ImGui::CalcTextSize(text));
			ImGui::SetCursorScreenPos(cspos);
			ImGui::TextV(fmt, args);
		}
		void Text(const char* fmt, ...) {
			va_list args;
			va_start(args, fmt);
			ImGui::SameLine();
			this->TextV(fmt, args);
			va_end(args);
			ImGui::NewLine();
		}
	};
};

#endif