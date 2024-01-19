#ifndef __htgnative_imgui_format_h_
#define __htgnative_imgui_format_h_
#pragma warning( push )

#pragma warning( disable : 4244 )
#include "imgui.h"
#include <nlohmann/json/json.hpp>
#include "../utils.hpp"
namespace ImGui {
	void PushCursorPos(ImVec2 pos);
	void PopCursorPos();
	bool __ProcessInlineHexColorImpl(const char* start, const char* end, ImVec4& color);

	void TextWrappedUnformatted(const char* text_begin, const char* text_end);

	struct StyledString {
		float fontScale;
		ImColor shadowColor;
		StyledString();
		std::string rawText, link_src;
		alib_MARKDOWN style;
		StyledString(std::string str, alib_MARKDOWN md);
	};
	bool __ProcessInlineJsonStyle(const char* start, const char* end, StyledString* md);
	void __TextColouredFormattedImplV(const char* fmt, va_list args);
	void __TextColouredFormattedImplUF(const char* fmt);
	void __TextColouredFormattedImpl(const char* fmt, ...);
	// Note: multicolored starts are formatted as follows BBGGRRAA. AA is optional.
	void TextMulticolored(const char* fmt, ...);
	void TextMulticoloredUnformatted(const char* text);
	// Aligns text to the right of screen, regardless of if there's text there already. Returns the position of the first element.
	ImVec2 TextAlignedRight(const char* fmt, ...);
	ImVec2 CalcTextAlignedRight(const char* fmt, ...);
};
#pragma warning( pop )

#endif