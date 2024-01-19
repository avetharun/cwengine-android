#include "imgui_format.h"
#include "imgui_markdown.h"
#include "../ani.h"

std::stack<ImVec2> posStack;
const alib_MARKDOWN MARKDOWN_NONE;
using namespace ImGui;
const char* ColorMarkerStart = "[/#";
const char* FormatMarkerEnd = "/]";
void ImGui::PushCursorPos(ImVec2 pos) {
	posStack.push(ImGui::GetCursorPos());
	ImGui::SetCursorPos(pos);
}

void ImGui::PopCursorPos() {
	ImGui::SetCursorPos(posStack.top());
	posStack.pop();
}

bool ImGui::__ProcessInlineHexColorImpl(const char* start, const char* end, ImVec4& color)
{
	const int hexCount = (int)(end - start);
	if (hexCount == 6 || hexCount == 8)
	{
		char hex[9];
		strncpy(hex, start, hexCount);
		hex[hexCount] = 0;

		unsigned int hexColor = 0;
		if (sscanf(hex, "%x", &hexColor) > 0)
		{
			color.x = static_cast<float>((hexColor & 0x00FF0000) >> 16) / 255.0f;
			color.y = static_cast<float>((hexColor & 0x0000FF00) >> 8) / 255.0f;
			color.z = static_cast<float>((hexColor & 0x000000FF)) / 255.0f;
			color.w = 1.0f;
			if (hexCount == 8)
			{
				color.w = static_cast<float>((hexColor & 0xFF000000) >> 24) / 255.0f;
			}

			return true;
		}
	}

	return false;
}

void ImGui::TextWrappedUnformatted(const char* text_begin, const char* text_end)
{
	bool need_backup = (GetCurrentWindow()->DC.TextWrapPos < 0.0f);  // Keep existing wrap position if one is already set
	if (need_backup) {
		PushTextWrapPos(0.0f);
	}
	TextEx(text_begin, text_end, ImGuiTextFlags_NoWidthForLargeClippedText); // Skip formatting
	if (need_backup) {
		PopTextWrapPos();
	}
}

bool ImGui::__ProcessInlineJsonStyle(const char* start, const char* end, StyledString* md)
{
	try {
		// Parse the JSON string
		nlohmann::json jsonStyle = nlohmann::json::parse(std::string(start, end));

		// Extract color, style, and text from the JSON
		if (jsonStyle.contains("color")) {
			ImVec4 textColor;
			auto colStr = jsonStyle["color"].get<std::string>();
			__ProcessInlineHexColorImpl(colStr.c_str(), colStr.c_str() + colStr.size(), textColor);
			md->style.r = textColor.x / 255;
			md->style.g = textColor.x / 255;
			md->style.b = textColor.x / 255;
			md->style.a = textColor.x / 255;
		}
		if (jsonStyle.contains("scale") && jsonStyle["scale"].is_number()) {
			md->fontScale = jsonStyle["scale"];
		}
		else {
			// Use previous font scale
			md->fontScale = -1;
		}
		if (jsonStyle.contains("shadow")) {
			ImVec4 textColor;
			auto colStr = jsonStyle["shadow"].get<std::string>();
			__ProcessInlineHexColorImpl(colStr.c_str(), colStr.c_str() + colStr.size(), textColor);
			md->shadowColor = ImColor(textColor);
		}
		if (jsonStyle.contains("style")) {
			for (const auto& style : jsonStyle["style"]) {
				if (style.is_string()) {
					std::string styleStr = style.get<std::string>();
					if (styleStr == "italic") {
						md->style.italic = true;
						md->style.hasMarkdown = true;
						continue;
					}
					if (styleStr == "underscore") {
						md->style.underscore = true;
						md->style.hasMarkdown = true;
						continue;
					}
					if (styleStr == "strikethrough") {
						md->style.strikethrough = true;
						md->style.hasMarkdown = true;
						continue;
					}
					if (styleStr == "shaky") {
						md->style.shaky = true;
						md->style.hasMarkdown = true;
						continue;
					}
					if (styleStr == "bold") {
						md->style.bold = true;
						md->style.hasMarkdown = true;
						continue;
					}
					// Add more styles as needed
				}
			}
		}

		if (jsonStyle.contains("text")) {
			md->rawText = jsonStyle["text"].get<std::string>();
		}
		if (jsonStyle.contains("link")) {
			md->link_src = jsonStyle["link"].get<std::string>();
		}
		return true;
	}
	catch (const nlohmann::json::parse_error& e) {
		// Handle JSON parsing error
		cwError::serrof(e.what());
		cwError::serrof("Json data:\n------\n%s\n------", std::string(start, end).c_str());
		return false;
	}
}

void ImGui::__TextColouredFormattedImplV(const char* fmt, va_list args) {
	char tempStr[4096];
	size_t len = vsnprintf(tempStr, 4095, fmt, args);
	tempStr[len] = '\0';


	bool pushedColorStyle = false;
	bool hasStyle = false;
	const char* textStart = tempStr;
	const char* textCur = tempStr;
	size_t len_start = strlen(ColorMarkerStart);
	size_t len_end = strlen(FormatMarkerEnd);

	int jsonBegin = 0;
	int jsonEnd = 0;
	std::vector<StyledString> kvs;
	alib_string curString = std::string(tempStr, len);
	size_t lenLeft = len;
	while (textCur < (tempStr + sizeof(tempStr)) && *textCur != '\0')
	{
		if (alib_streqn(textCur, ColorMarkerStart, len_start))
		{
			// Print accumulated text
			if (textCur != textStart)
			{
				ImGui::TextUnformatted(textStart, textCur);
				ImGui::SameLine(0.0f, 0.0f);
			}

			// Process color code
			const char* colorStart = textCur + strlen(ColorMarkerStart);
			do
			{
				lenLeft--;
				++textCur;
			} while (*textCur != '\0' && !alib_streqn(textCur, FormatMarkerEnd, len_end));


			// Change color
			if (pushedColorStyle)
			{
				ImGui::PopStyleColor();
				pushedColorStyle = false;
			}

			ImVec4 textColor;
			if (__ProcessInlineHexColorImpl(colorStart, textCur, textColor))
			{
				ImGui::PushStyleColor(ImGuiCol_Text, textColor);
				pushedColorStyle = true;
			}

			textStart = textCur + strlen(ColorMarkerStart) - 1;
		}
		else if (*textCur == '\n')
		{
			// Print accumulated text an go to next line
			ImGui::TextUnformatted(textStart, textCur);
			textStart = textCur + strlen(FormatMarkerEnd) - 1;
		}
		else if (*textCur == '{') {
			const char* jStart = textCur - 1;
			while (*textCur != '\0' && *textCur != '}' && textCur++);
			// Parse the JSON string and extract color, style, and text
			StyledString md;
			if (__ProcessInlineJsonStyle(jStart, textCur + 1, &md)) {
				// Add the styled string to the vector
				kvs.emplace_back(md);
				hasStyle = true;
			}

			int i = 0;
			// Process the StyledString vector
			for (const auto& kv : kvs) {
				i++;
				if (kv.style.hasColor) {
					ImVec4 textColor;
					textColor.x = kv.style.r;
					textColor.y = kv.style.g;
					textColor.z = kv.style.b;
					textColor.w = kv.style.a;
					ImGui::PushStyleColor(ImGuiCol_Text, textColor);
				}
				if (kv.fontScale > 0) {
					ImGui::PushFontScale(kv.fontScale);
				}
				//ImGui::PushFontShadow(kv.shadowColor);
				if (kv.link_src.size() > 0) {
					ImRotateStart();
					ImVec2 pos = ImGui::GetCursorPos();
					ImGui::UrlHTTPText(kv.link_src.c_str(), ani::Networking::openURLBrowser, kv.rawText.c_str(), false);
					if (kv.style.bold) {
						float d = ImGui::GetFontSize() * (0.025f * 0.85f);
						for (int i = 0; i < 10; i++) {
							ImGui::PushCursorPos({ pos.x + ((i * 0.1f) * d), pos.y + (((i - 5) * 0.1f) * (d * 0.8f)) });
							ImGui::Text(kv.rawText.c_str());
							ImGui::PopCursorPos();
						}
					}
					if (kv.style.italic) {
						ImRotateEnd(-12, ImElemCenter());
					}
					if (kv.style.strikethrough) {
						ImGui::StrikethroughLast();
					}
					if (kv.style.underscore) {
						ImGui::UnderlineLast();
					}
				}
				else {
					ImGui::MarkdownText(kv.rawText.c_str(), kv.rawText.c_str() + kv.rawText.size(), kv.style);
				}
				//ImGui::PopFontShadow();
				if (kv.fontScale > 0) {
					ImGui::PopFontScale();
				}
				if (i != kvs.size()) { ImGui::SameLine(0, 0); }

				if (kv.style.hasColor) {
					ImGui::PopStyleColor();
				}
			}
			kvs.clear();
			textStart = textCur + 1;
		}
		++textCur;
		lenLeft--;
	}

	if (textCur != textStart)
	{
		ImGui::TextUnformatted(textStart, textCur);
	}
	else
	{
		ImGui::NewLine();
	}

	if (pushedColorStyle)
	{
		ImGui::PopStyleColor();
	}
}

void ImGui::__TextColouredFormattedImplUF(const char* fmt) {
	__TextColouredFormattedImplV(fmt, {});
}

void ImGui::__TextColouredFormattedImpl(const char* fmt, ...)
{
	va_list argPtr;
	va_start(argPtr, fmt);
	__TextColouredFormattedImplV(fmt, argPtr);
	va_end(argPtr);
}

// Note: multicolored starts are formatted as follows BBGGRRAA. AA is optional.
void ImGui::TextMulticolored(const char* fmt, ...) {
	va_list argPtr;
	va_start(argPtr, fmt);
	__TextColouredFormattedImplV(fmt, argPtr);
	va_end(argPtr);
}

void ImGui::TextMulticoloredUnformatted(const char* text) {
	__TextColouredFormattedImplUF(text);
}

// Aligns text to the right of screen, regardless of if there's text there already. Returns the position of the first element.
ImVec2 ImGui::TextAlignedRight(const char* fmt, ...) {
	va_list argPtr;
	va_start(argPtr, fmt);
	const char* tmp = alib_strfmtv(fmt, argPtr);
	ImVec2 sz = ImGui::CalcTextSize(tmp);
	ImVec2 cont_region = ImGui::GetWindowContentRegionMax();
	ImVec2 cur = ImGui::GetCursorScreenPos();
	ImVec2 pos = { cur.x + (cont_region.x - sz.x) , cur.y };
	ImGui::GetForegroundDrawList()->AddText(pos, ImGui::GetColorU32(ImGuiCol_Text), tmp);
	free((void*)tmp);
	va_end(argPtr);
	return pos;
}

ImVec2 ImGui::CalcTextAlignedRight(const char* fmt, ...) {
	va_list argPtr;
	va_start(argPtr, fmt);
	const char* tmp = alib_strfmtv(fmt, argPtr);
	ImVec2 sz = ImGui::CalcTextSize(tmp);
	ImVec2 cont_region = ImGui::GetWindowContentRegionMax();
	ImVec2 cur = ImGui::GetCursorScreenPos();
	ImVec2 pos = { cur.x + (cont_region.x - sz.x) , cur.y };
	va_end(argPtr);
	free((void*)tmp);
	return pos;
}

unsigned char ImGui::ImRGB::getr() {
	return (colour)[0];
}

unsigned char ImGui::ImRGB::getg() {
	return (colour)[1];
}

unsigned char ImGui::ImRGB::getb() {
	return (colour)[2];
}

unsigned char ImGui::ImRGB::geta() {
	return (colour)[3];
}

ImTriple<int, int, int> ImGui::ImRGB::convertHSV() {
	float h;
	float s;
	float v;
	ImGui::ColorConvertRGBtoHSV(getr(), getg(), getb(), h, s, v);
	return { (int)(h * 255),(int)(s * 255),(int)(v * 255) };
}

void ImGui::ImRGB::setr(unsigned char _r) {
	colour[0] = _r;
}

void ImGui::ImRGB::setg(unsigned char _g) {
	colour[1] = _g;
}

void ImGui::ImRGB::setb(unsigned char _b) {
	colour[2] = _b;
}

void ImGui::ImRGB::seta(unsigned char _a) {
	colour[3] = _a;
}

void ImGui::ImRGB::hue(float _h) {
	auto _hsv = convertHSV();
	_hsv.t1 += _h;
	float _r = getr() / 255.0f;
	float _g = getg() / 255.0f;
	float _b = getb() / 255.0f;
	ImGui::ColorConvertHSVtoRGB(_hsv.t1, _hsv.t2, _hsv.t3, _r, _g, _b);
	setr(_r * 255);
	setg(_g * 255);
	setb(_b * 255);
}

void ImGui::ImRGB::saturation(float _s) {
	auto _hsv = convertHSV();
	_hsv.t2 *= _s;
	float _r = getr() / 255.0f;
	float _g = getg() / 255.0f;
	float _b = getb() / 255.0f;
	ImGui::ColorConvertHSVtoRGB(_hsv.t1, _hsv.t2, _hsv.t3, _r, _g, _b);
	setr(_r * 255);
	setg(_g * 255);
	setb(_b * 255);
}

void ImGui::ImRGB::value(float _v) {
	auto _hsv = convertHSV();
	_hsv.t3 *= _v;
	float _r = getr() / 255.0f;
	float _g = getg() / 255.0f;
	float _b = getb() / 255.0f;
	ImGui::ColorConvertHSVtoRGB(_hsv.t1, _hsv.t2, _hsv.t3, _r, _g, _b);
	setr(_r * 255);
	setg(_g * 255);
	setb(_b * 255);
}

void ImGui::ImRGB::parse_hsv(float h, float s, float v) {
	value(v);
	saturation(s);
	hue(h);
}

ImGui::ImRGB::ImRGB(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	this->setr(r);
	this->setg(g);
	this->setb(b);
	this->seta(a);
}

ImGui::ImRGB::ImRGB(ImVec4 c) {
	this->setr(c.x * 255.0f);
	this->setg(c.y * 255.0f);
	this->setb(c.z * 255.0f);
	this->seta(c.w * 255.0f);
}


// Convert the color to a format understood by TextMulticolored()

std::string ImGui::ImRGB::tostring() {
	return alib_strfmts("%s%02x%02x%02x%s", ColorMarkerStart, getr(), getg(), getb(), FormatMarkerEnd);
}

// String to reset color

std::string ImGui::ImRGB::resetstr() {
	return alib_strfmts("%s%s", ColorMarkerStart, FormatMarkerEnd);
}
ImGui::ImRGB::operator ImColor() { return { getr(), getg(), getb(), geta() }; }
//operator ImVec4() { return { (float)getr(), (float)getg(), (float)getb(), (float)geta() }; }
ImGui::ImRGB::operator ImU32() { return ImGui::ColorConvertFloat4ToU32({ (float)getr(), (float)getg(), (float)getb(), (float)geta() }); }

ImGui::StyledString::StyledString() {}
ImGui::StyledString::StyledString(std::string str, alib_MARKDOWN md) {
	rawText = str;
	style = md;
}
