#pragma once
#ifndef __rosemary_project_imgui_uielement_h_
#define __rosemary_project_imgui_uielement_h_
#include "imgui.h"
#include "imgui_format.h"
#include "imgui_texture.h"
#include "imgui_internal.h"

#include <string>
#include <vector>
#include <stack>
#include <array>
#include "../include/abt/cwerror.h"
#ifndef PI
#define PI 3.14159265358979323846
#endif


enum {
	ImGuiMenuButton_Hamburger,
	ImGuiMenuButton_Doner,
	ImGuiMenuButton_Bento,
	ImGuiMenuButton_Kebab,
	ImGuiMenuButton_Meatball,
	ImGuiMenuButton_Cookie
};
using namespace ImGui;
namespace ImGui {

	// Note: multicolored starts are formatted as follows BBGGRRAA. AA is optional.
	inline void TextMulticoloured(const char* fmt, ...);

	// Returns true if the rect has an area of 0
	bool IsEmptyRect(ImRect r);
	// Returns true if the rect has an area of 0
	bool IsEmptyRect(ImVec2 r);
	ImVec2 ElementLocalPositionFromStatic(ImVec2 in);
	void PushFontScale(float scale);
	void PopFontScale();
	ImRect ScaleRect(ImRect vec_in, float amount);
	struct ElementStubImpl {

		long draw_order;
		long elem_t;
		static inline int ElementOffset;
		static void RenderFinishDrawingImpl();
		static void RenderBeginImpl();
		static inline bool __RawIsBegin;
		static inline int __RawAmt;
		static inline ImVec2 __cursor_screen_pos_last;
		static inline ImVec2 ScrollEmptyAccum;
		static void RenderEndImpl();
		virtual void RenderImpl();
		static void ResetOffsets();
	};
	struct UIElementData {
		ImRGB BackgroundColor;
		float BackgroundBorderRadius;
		float ElementBorderRadius;
		ImRGB ForegroundColor;
		ImRGB TextColor;
		ImRGB HoverColor;
		ImRGB ClickedColor;
	};
	void ScrollEmptySpace();
	enum ElementsEnum : long long {
		ImGui_AllElements = B32(10000000, 00000000, 00000000, 00000000),
		ImGui_TextElement = B32(00000000, 00000000, 00000000, 00000010),
		ImGui_ButtonElement = B32(00000000, 00000000, 00000000, 00000100),
		ImGui_ImageElement = B32(00000000, 00000000, 00000000, 00001000),
		ImGui_CheckboxElement = B32(00000000, 00000000, 00000000, 00010000),
		ImGui_TextInputElement = B32(00000000, 00000000, 00000000, 00100000),
	};
	// Creates a fullscreen window via ImGui, that cannot be moved, resized, and has no background.
	void BeginFullscreen();
	void EndFullscreen();
	// Creates a window via ImGui, that cannot be moved, resized, has no background, but will be resized by elements inside it
	void BeginStatic();
	// Creates a fullscreen window via ImGui, that cannot be moved, resized, has no background, and does not take inputs.
	void BeginEmptyStatic();
	void EndEmptyStatic();
	// Allows a element to be scrolled via dragging.
	void BeginDragScrollable(const char* name, bool* _open = 0, ImGuiWindowFlags flags = 0);
	void EndDragScrollable();
	// Allows a element to be scrolled via dragging.
	void BeginDragScrollableChild(const char* name, ImVec2 size = { 0,0 }, bool border = 0, ImGuiWindowFlags flags = 0);
	void EndDragScrollableChild();
	void RectFilled(ImVec2 pos, ImVec2 size, ImColor col, float border_radii = 0);
	void RectUnfilled(ImVec2 pos, ImVec2 size, ImColor col, float border_radii = 0, float thickness = 4.0f);
	void ForegroundRectFilled(ImVec2 pos, ImVec2 size, ImColor col, float border_radii = 0);
	void ForegroundRectUnfilled(ImVec2 pos, ImVec2 size, ImColor col, float border_radii = 0, float thickness = 4.0f);
	void DrawLine(ImVec2 start, ImVec2 end, ImColor col = IM_COL32(255, 0, 0, 255), float thickness = 3.0f);
	void TextForeground(std::string text, ImVec2 pos);
	void IMTextForeground(std::string text, ImVec2 pos);
	void TextBackground(std::string text, ImVec2 pos);
	void TextPositioned(const char* text, ImVec2 pos);


	void Circle(ImVec2 center, float radius, ImColor col = IM_COL32(255, 0, 0, 255), bool filled = true);

	// Create a loading bar, where pos = the top left most corner where bar will be placed
	// Percentage must be 0-100, otherwise it will just clamp to 100.
	// Uses colors from the Button colorset (ImGuiCol_Button for the box, ImGuiCol_ButtonActive for the indicator)
	void LoadingBar(float percentage, ImVec2 size = { 0,0 }, float border_thickness = 12.0f, float rounding = -1.0f);
	// Create a loading circle
	// Percentage must be 0-100, otherwise it will just clamp to 100.
	// Uses colors from the Button colorset (ImGuiCol_Button for the background, ImGuiCol_ButtonActive for the indicator)
	void LoadingCircle(float percentage, float radius = 0);
	// Create a loading circle  that's hollow (see: https://img.icons8.com/office/480/spinning-circle.png)
	// Percentage must be 0-100, otherwise it will just clamp to 100.
	// Uses colors from the Button colorset (ImGuiCol_Button for the background, ImGuiCol_ButtonActive for the indicator)
	void LoadingCircleHollow(float percentage, float radius = 0);
	void InputTextPoositioned(const char* label, char* buf, size_t buf_size, ImVec2 position, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = 0, void* user_data = 0);
	void ImagePositioned(ImTextureID texid, ImVec2 begin, ImVec2 size, ImVec2 uv0, ImVec2 uv1);
	void IMImagePositioned(ImTextureID texid, ImVec2 begin, ImVec2 size, ImVec2 uv0, ImVec2 uv1);
	void ForegroundImagePositioned(ImTextureID texid, ImVec2 begin, ImVec2 size, ImVec2 uv0, ImVec2 uv1);
	// An arrow drawn to look like a "back arrow" from android. Defaults to  pointing left.
	// See: https://i.stack.imgur.com/avYnD.png
	// Inherits colors from ImGuiCol_ButtonActive
	bool AndroidArrowButton(const char* str_id, ImGuiDir direction = ImGuiDir_Left, float thickness = 6.5f);
	ImVec2 GetDefaultFABSize();
	ImVec2 GetDefaultFABLocation();
	// Styled as a FAB from android
	// See:  https://developer.android.com/training/material/images/fab.png
	// Inherits color from ImGuiCol_Button, ImGuiCol_ButtonActive (Unpressed, pressed)
	// Everything is shown in foreground, so don't worry about using this inside an imgui window.
	bool FloatingActionButton(const char* label, bool* v = NULL);
	typedef int ImGuiMenuButton;
	// If btn_type == ..Cookie:
	//		will draw a elipsis-like shape
	// See: https://i.stack.imgur.com/YIcbV.png
	// Inherits colors from ImGuiCol_ButtonActive
	bool MenuButton(const char* str_id, bool* v, ImVec2 pos, ImGuiMenuButton btn_type = ImGuiMenuButton_Hamburger);
	struct MarkdownVars {
		struct MarkdownHeader {
			int y_off;
		};
		// header name (first N characters) : header data
		std::vector<std::pair<std::string, MarkdownHeader>> headdir;
		float element_border_width;

	};
	ImVec2 WrappedSize();

	ImVec2 CalcVerticalTextSize(const char* text, const char* text_end = NULL, bool hide_text_after_double_hash = false, float wrap_width = -1.0f);

	/// Draws vertical text. The position is the bottom left of the text rect.
	inline void AddTextVertical(ImDrawList* DrawList, const char* text, ImVec2 pos, ImU32 text_color);
	ImVec2 ImElemCenter();
	void ImRotateEnd(float a, ImVec2 center);
	void ImRotateStart();
	void ImTranslationEnd(ImVec2 translation);
	void ImTranslationStart();


	// Android styled thumb switch 
	// See: https://miro.medium.com/max/420/1*ccfEt-tGF1bDO6sV_yNc8w.gif
	void ThumbSwitch(const char* str_id, bool* v, const char* fmt = "\0", ...);

	struct ToastMessageStack {
		struct ToastMessageStackVar {
			float millis_final;
			float millis_current;
			std::string text;
		};
		static inline std::stack<ToastMessageStackVar> stack = {};
		size_t cursor = 0;
		 bool has_init = false;
		static inline ImGuiContextHook* hook = new ImGuiContextHook();
		
		static void init();
		static void push(const char* text, float millis);
	};
	// Creates an android-style toast message that lasts N milliseconds
	void ToastMessage(const char* text, float milliseconds);

	struct TextElement : ElementStubImpl {

		UIElementData  ElementData;
		enum TextElementFlags : char {
			UseBackground = B8(00000001)
		};
		std::string str;
		float size;
		ImVec2 position;
		TextElementFlags flags;
		void RenderImpl();
		TextElement(std::string __text, const ImVec2& pos);
	};
	struct ButtonElement : ElementStubImpl {
		UIElementData ElementData;
		int __elemi;
		std::string str;
		ImVec2 position;
		void RenderImpl();
		ButtonElement(std::string button_text, const ImVec2& pos);
	};
	struct UIRendererUtils {
		template <ElementsEnum ElemT = ImGui_AllElements>
		// Template is the element you want, as a bitmask. Otherwise if empty, if in == AllElements, return true
		static bool isAllElementsOr(ElementsEnum in);
	};// Simple helper function to load an image into a OpenGL texture with common settings
	bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
	bool LoadTextureFromMemory(unsigned char* data, size_t length, GLuint* out_texture, int* out_width, int* out_height);
	bool LoadTextureFromAPK(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
};


#endif