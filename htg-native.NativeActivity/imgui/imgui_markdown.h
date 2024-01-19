#pragma once
#ifndef __rosemary_project_imgui_markdown_h_
#define __rosemary_project_imgui_markdown_h_
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_uielement.h"
#include "imgui_extras.h"
#include <istream>
#include <chrono>
namespace ImGui {
	// A button with only text visible.
	bool TextButton(const char* str_id, const char* txt);
	void DrawCharItalic(char chr);
	void DrawCharShaky(char chr, float offset);
	void ShakyText(const char* text);
	void ItalicText(const char* text);
	void ItalicText(const char* text, const char* end);
	// Underlines last instance of an item, but is mostly used to underline text.
	void UnderlineLast();
	void StrikethroughLast();
	// May not work with all fonts! Optimized to work with the default ImGui font!
	// use fontSizeOffset to change how much of the font size is moved for each pass.
	void BoldText(const char* text, float fontSizeOffset = 0.035f);
	void UnderlineText(const char* text);
	void UnderlineText(const char* text, const char* text_end);
	void StrikethroughText(const char* text, const char* text_end);
	void StrikethroughText(const char* text);
	void MarkdownText(const char* text, const char* text_end, alib_MARKDOWN md);
	void CodeBlockLast();
	void CodeBlockText(const char* text);
	// Quote bar inherits color from ImGuiCol_Button
	// Puts bar before last instance of an item
	void QuoteLast();
	// Quote bar inherits color from ImGuiCol_Button
	void QuoteText(const char* text);
	void UrlHTTPText(const char* url, std::function<void(const char* url)> url_opener, const char* url_text, bool underlined);
};
#endif