#include "imgui_markdown.h"

// A button with only text visible.
bool ImGui::TextButton(const char* str_id, const char* txt) {
	bool out = false;
	ImVec2 pos = GetCursorPos();
	ImVec2 txt_sz = CalcTextSize(txt);
	ImGui::Text(txt);
	ImGui::SetCursorPos(pos);
	ImGui::InvisibleButton(str_id, txt_sz);
	if (IsItemClicked()) { out = true; }
	return out;
}

void ImGui::DrawCharItalic(char chr) {
	ImRotateStart();
	ImGui::Text("%c", chr);
	ImRotateEnd(-12, ImElemCenter());
}

void ImGui::DrawCharShaky(char chr, float offset) {
	//ImRotateStart();
	ImTranslationStart();
	ImGui::Text("%c", chr);
	ImTranslationEnd(ImVec2{ offset * 1.1f, offset * 1.1f });
	//ImRotateEnd(offset * 5, ImElemCenter() + ImVec2(offset * 2, offset * 2));
}

void ImGui::ShakyText(const char* text) {
	float font_sz = GetCurrentContext()->Font->FontSize;
	int i = 0;
	while (*text) {
		float offset = sin(alib_time() + (*text) + (i ^ 0x84fd * 128));
		DrawCharShaky(*text, offset);
		if (*((text++) + 1)) {
			ImGui::SameLine(0, 0);
		}
		i++;
	}
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::CalcTextSize(text).x);
}

void ImGui::ItalicText(const char* text) {
	float font_sz = GetCurrentContext()->Font->FontSize;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImGui::GetStyle().ItemSpacing * ImVec2(0.9f, 0.9f));
	while (*text) {
		DrawCharItalic(*text);
		if (*((text++) + 1)) {
			ImGui::SameLine(0, 0);
		}
	}
	ImGui::PopStyleVar();
}

void ImGui::ItalicText(const char* text, const char* end) {
	//SetCursorPosX(GetCursorPosX())
	float font_sz = GetCurrentContext()->Font->FontSize;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImGui::GetStyle().ItemSpacing * ImVec2(0.9f, 0.9f));
	while (text < end) {
		DrawCharItalic(*text);
		ImGui::SameLine();
		if (*((text++) + 1)) {
		}
	}
	ImGui::PopStyleVar();
}

// Underlines last instance of an item, but is mostly used to underline text.
void ImGui::UnderlineLast()
{
	ImVec2 min = GetItemRectMin();
	ImVec2 max = GetItemRectMax();
	min.y = max.y;
	float underline_width = GetFontSize() * 0.1f;
	GetWindowDrawList()->AddLine(
		min, max, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)), underline_width);
}

void ImGui::StrikethroughLast()
{
	ImVec2 min = GetItemRectMin();
	ImVec2 max = GetItemRectMax();
	min.y = max.y = max.y - GetItemRectSize().y * 0.5f;
	float strikethrough_width = GetFontSize() * 0.05f;
	GetWindowDrawList()->AddLine(
		min, max, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Text]), strikethrough_width);
}

// May not work with all fonts! Optimized to work with the default ImGui font!
// use fontSizeOffset to change how much of the font size is moved for each pass.
void ImGui::BoldText(const char* text, float fontSizeOffset) {
	ImVec2 begin = ImGui::GetCursorPos();
	float d = ImGui::GetFontSize() * (fontSizeOffset * 0.85f);
	//bool enableFontShadow = ImGui::custom_UseFontShadow;
	//if (enableFontShadow) {
	//ImGui::custom_UseFontShadow = false;
	//}
	for (int i = 0; i < 10; i++) {
		ImGui::PushCursorPos({ begin.x + ((i * 0.1f) * d), begin.y + (((i - 5) * 0.1f) * (d * 0.8f)) });
		ImGui::Text(text);
		ImGui::PopCursorPos();
	}
	//ImGui::custom_UseFontShadow = enableFontShadow;
	ImGui::Text(text);
}

void ImGui::UnderlineText(const char* text)
{
	std::string line;
	std::stringstream ss(text);
	while (std::getline(ss, line)) {
		if (line == "") {
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y);
		}
		if (line != "")
			ImGui::TextWrapped(line.c_str());
		ImGui::UnderlineLast();
	}
}

void ImGui::UnderlineText(const char* text, const char* text_end)
{
	std::string line;
	std::stringstream ss(std::string(text, text_end));
	while (std::getline(ss, line)) {
		if (line == "") {
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y);
		}
		if (line != "")
			ImGui::TextWrapped(line.c_str());
		ImGui::UnderlineLast();
	}
}

void ImGui::StrikethroughText(const char* text, const char* text_end)
{
	std::string line;
	std::stringstream ss(std::string(text, text_end));
	while (std::getline(ss, line)) {
		if (line == "") {
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y);
		}
		if (line != "")
			ImGui::TextWrapped(line.c_str());
		ImGui::StrikethroughLast();
	}
}

void ImGui::StrikethroughText(const char* text)
{
	std::string line;
	std::stringstream ss(text);
	while (std::getline(ss, line)) {
		if (line == "") {
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y);
		}
		if (line != "")
			ImGui::TextWrapped(line.c_str());
		ImGui::StrikethroughLast();
	}
}

void ImGui::MarkdownText(const char* text, const char* text_end, alib_MARKDOWN md) {
	std::string line;
	std::stringstream ss(std::string(text, text_end));
	while (std::getline(ss, line)) {
		if (line == "") {
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y);
		}
		if (line != "") {

			if (md.shaky) {
				ImGui::ShakyText(line.c_str());
			}
			else if (md.italic) {
				ImGui::ItalicText(line.c_str());
			}
			else if (md.bold) {
				ImGui::BoldText(line.c_str());
			}
			else {
				ImGui::TextUnformatted(line.c_str());
			}
			if (md.underscore) {
				ImGui::UnderlineLast();
			}
			if (md.strikethrough) {
				ImGui::StrikethroughLast();
			}
		}
	}
}

void ImGui::CodeBlockLast() {
	ImVec2 min = GetItemRectMin() - ImGui::GetStyle().ItemSpacing * ImVec2{ 0.15f, 0.15f };
	ImVec2 max = GetItemRectMax() + ImGui::GetStyle().ItemSpacing * ImVec2{ 0.15f, 0.15f };
	ImVec4 col = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
	float _w = col.w;
	col.w *= 0.25f;
	ImGui::GetWindowDrawList()->AddRectFilled(min, max, ImGui::GetColorU32(col), ImGui::GetStyle().ItemSpacing.y * 0.75f);
	col.w = _w;
	ImGui::GetWindowDrawList()->AddRect(min, max, ImGui::GetColorU32(col), ImGui::GetStyle().ItemSpacing.y * 0.75f, 0, ImGui::GetStyle().ItemSpacing.y * 0.05f);
}

void ImGui::CodeBlockText(const char* text) {
	ImGui::PushStyleColor(ImGuiCol_Text, { 0,0,0,0 });
	ImVec2 cspos = ImGui::GetCursorPos();
	ImGui::Text(text);
	ImGui::PopStyleColor();
	ImGui::CodeBlockLast();
	ImGui::SetCursorPos(cspos);
	ImVec4 _c = ImGui::GetStyleColorVec4(ImGuiCol_Text);
	_c.w *= 0.5f;
	ImGui::PushStyleColor(ImGuiCol_Text, _c);
	ImGui::Text(text);
	ImGui::PopStyleColor();

}

// Quote bar inherits color from ImGuiCol_Button
// Puts bar before last instance of an item
void ImGui::QuoteLast() {
	float bar_off = GetStyle().ItemSpacing.x * 0.5f;
	ImVec2 min = GetItemRectMin();
	ImVec2 max = GetItemRectMax();
	DrawLine({ min.x - bar_off, min.y }, { min.x - bar_off, max.y }, GetColorU32(ImGuiCol_Button), bar_off * 0.75f);
}

// Quote bar inherits color from ImGuiCol_Button
void ImGui::QuoteText(const char* text) {
	ImGui::TextWrapped(text);
	ImGui::QuoteLast();
}

void ImGui::UrlHTTPText(const char* url, std::function<void(const char* url)> url_opener, const char* url_text, bool underlined) {
	ImVec2 pos = ImGui::GetCursorPos();
	if (*url_text == '\0') {
		url_text = url;
	}
	ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
	ImVec4 _c = ImGui::GetStyleColorVec4(ImGuiCol_Text);
	_c.x *= 0.75;
	_c.y *= 0.75;
	ImGui::PushStyleColor(ImGuiCol_Text, _c);
	bool clicked = ImGui::TextButton("##__impl_ImGui_UtlHttpTextBtn", url_text);
	if (underlined) {
		ImGui::UnderlineLast();
	}
	ImGui::PopStyleColor(2);
	if (clicked) {
		url_opener(url);
	}
}
