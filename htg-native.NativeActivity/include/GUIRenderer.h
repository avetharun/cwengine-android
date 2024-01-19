#pragma once
#if !defined(GUI_RENDERER_HPP)
#define GUI_RENDERER_HPP
#include "../utils.hpp"
#include <any>
#include "../imgui/imgui_format.h"
#include "../imgui/imgui_markdown.h"
#include "../imgui/imgui_uielement.h"
#include <stack>
#include "renderer.h"
#include "abt/cwabt.hpp"
namespace UI {
	static bool isInteracting = false;
	struct TextElement;
	typedef Sprite ImageElement;
	struct ButtonElement;
	struct GameTextBox;
	struct GameOptionsBox;
	struct GUIRenderer
	{
		static const ImGuiWindowFlags __elementWinFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;
		static void Render();
		static void RenderAnyUpdate();
	};
	size_t ui_elem_offset;
	std::string _fmt_name() {
		std::string _name__ = "##___ELEMENT_UI";
		_name__.append("__");
		_name__.append(alib_strfmt("%d_", ui_elem_offset));
		return _name__;
	}
	static std::vector<std::any*> _element_arr;
	// rect:
	// xy = pos
	// wh = scale
	struct TextElement {
		bool __render_on_update = false;
		bool enabled = true;
		ImVec2 pos;
		float size = 1;
		std::string text;
		void textfmt(const char* fmt, ...) {
			va_list args;
			va_start(args, fmt);
			size_t bufsz = 0;
			bufsz = vsnprintf(NULL, 0, fmt, args);
			char* _buf = new char[bufsz];
			vsnprintf(_buf, bufsz, fmt, args);
			va_end(args);
			text = std::string(_buf);
		}

		void Render() {
			if (!enabled) { return; }
			if (this->size <= 0) { this->size = 1.0f; }
			ImGui::PushFont(rsm::Fonts::default_font);
			ImGui::TextForeground(this->text, this->pos);
			ImGui::PopFont();
		}
		TextElement(std::string _txt) {
			_element_arr.push_back(new std::any(this));
			this->text = _txt;
		}
		TextElement() {
			_element_arr.push_back(new std::any(this));
		}
	};
	struct ButtonElement {
		bool __render_on_update = false;
		bool enabled = true;
		ImVec2 pos{0,0};
		enum ButtonElementFlags {
			NoBackground = B8(00000001),
		};
		ButtonElementFlags buttonFlags{};
		std::string text = "";
		float size = 1.0f;
		std::function<void()> onClick = [&](){};
		void textfmt(const char* fmt, ...) {
			char* x;
			va_list ap;
			va_start(ap, fmt);
			int size = vasprintf(&x, fmt, ap);
			va_end(ap);
			this->text = x;
		}
		void Render() {
			ImGui::PushFont(rsm::Fonts::default_font);
			if (!enabled) { return; }
			if (this->size == 0) { this->size = 1.0f; }
			ImGui::SetNextWindowPos({ (float)this->pos.x - 8, (float)this->pos.y - 12 });
			ImGuiIO& __io = ImGui::GetIO();
			ImVec2 _v = ImGui::CalcTextSize(this->text.c_str());
			std::string _s = _fmt_name();
			ImGui::Begin(_s.c_str(), 0, GUIRenderer::__elementWinFlags);
			if (this->buttonFlags & ButtonElementFlags::NoBackground) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0, 0, 0, 0)));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(0, 0, 0, 0)));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(0, 0, 0, 0)));
			}

			bool __pressed = ImGui::Button(_fmt_name().c_str(), _v);
			ImGui::SameLine();
			ImGui::SetCursorPosX(this->pos.x + _v.x / 2);
			ImGui::SetWindowFontScale(this->size);
			ImGui::TextMulticolored(this->text.c_str());
			if (this->buttonFlags & ButtonElementFlags::NoBackground) {
				ImGui::PopStyleColor(3);
			}
			if (__pressed) { this->onClick(); }
			ImGui::PopFont();
			ImGui::End();
		}
		ButtonElement(std::string _txt, ButtonElementFlags _flags = {}) {
			_element_arr.push_back(new std::any(this));
			this->text = _txt;
			this->buttonFlags = _flags;
		}
		ButtonElement()
		{
			_element_arr.push_back(new std::any(this));
		}
	};

	std::unordered_map<std::string, Sprite*> characterEmotions;
	struct GameTextBox {
		void_0a_f on_complete = nullptr;
		std::string text;
		std::string title;
		std::string title_base;
		std::string name = "";
		float delta = 0;
		bool is_active;
		bool is_first_frame = true;
		bool is_wanting_dtor = false;
		bool isLeftSpeaking = true;
		bool usesScanlines = true;
		static inline std::stack<GameTextBox*> _mBoxes;
		static inline float seconds_for_btn_flash = 0.5f;
		static inline float padding_bottom = 128;
		static inline float padding_sides = 128;
		static inline int TextBoxOffset = 0;
		int currentIDX = 0;
		float betweenScanTimes = 0.05f;
		float timeSinceLastScan = 0.0f;
		static const int SPRITESIDE_LEFT = 0;
		static const int SPRITESIDE_CENTER = 1;
		static const int SPRITESIDE_RIGHT = 2;
		Sprite* sprites[3]{};
		bool manualDrawStack[3]{};
		void clearSprites() {
			sprites[0] = 0;
			sprites[1] = 0;
			sprites[2] = 0;
		}
		void assignSprite(int side, Sprite* sprite) {
			cwError::serroif(side > 3 || side < 0, "Side must be any of:\nLEFT, RIGHT, CENTER\n (0, 1, 2)\nFound: %i. Defaulting to CENTER", side);
			if (side > 3 || side < 0) { side = SPRITESIDE_CENTER; }
			cwError::serroif(sprite == nullptr, "Sprite was null when assigning to a text box.\nDid you mean to clear the sprite?");
			if (sprites[side] != 0) {
				auto sprite = sprites[side];
				sprite->manualDraw = manualDrawStack[side];
			}
			sprites[side] = sprite;
			sprite->center = true;
			manualDrawStack[side] = sprite->manualDraw;
		}
		void clearSprite(int side) {
			cwError::serroif(side > 3 && side < 0, "Side is greater than the allowed types (0 - 3)\nFound:%i", side);
			side = alib_clamp(side, 0, 3);
			if (sprites[side] != 0) {
				auto sprite = sprites[side];
				sprite->manualDraw = manualDrawStack[side];
			}
			sprites[side] = 0;
		}
		GameTextBox(std::string label_id, std::string m_text, void_0a_f on_complete_func) {
			this->text = m_text;
			this->on_complete = on_complete_func;
			this->title_base = alib_strfmt("###%d", TextBoxOffset);
			this->title = label_id + title_base;
			_mBoxes.push(this);
			TextBoxOffset++;
		}
		void Render() {
			if (is_first_frame) {
				is_first_frame = false;
				return;
			}
			bool showsCursor = true;
			ImGui::PushFont(rsm::Fonts::default_font);
			ImGuiIO& io = ImGui::GetIO();
			if (io.DisplaySize.x > io.DisplaySize.y) {
				padding_sides = io.DisplaySize.x * 0.21f;
			}
			else {
				padding_sides = io.DisplaySize.x * 0.04f;
			}
			float ysz = ImGui::CalcTextSize(text.c_str()).y + ImGui::CalcTextSize("#").y;
			
			ImVec2 wSiz = { io.DisplaySize.x - (padding_sides * 2), padding_bottom * 1.25f };
			ImVec2 wPos = { padding_sides, io.DisplaySize.y - padding_bottom };
			ImGui::BeginFullscreen();
			ImGui::RectFilled(wPos, wSiz, { 0, 0, 0, 200}, 8);
			ImGui::RectUnfilled(wPos, wSiz, 0xc0c0c0fc, 8, 2);
			UI::isInteracting = true;
			if (timeSinceLastScan > betweenScanTimes) {
				timeSinceLastScan = 0;
				currentIDX++;
			}
			timeSinceLastScan += io.DeltaTime;
			this->is_active = ImGui::IsWindowFocused();
			const char* line = text.c_str();
			int tSzLen = text.length();
			if (usesScanlines) {
				showsCursor = tSzLen == text.length();
				//tSzLen = alib_min(currentIDX, text.length());
			}
			const char* text_end = text.c_str() + tSzLen;
			auto pos = wPos;
			float accum_height = 0;
			if (line < text_end)
			{
				while (line < text_end)
				{
					const char* line_end = (const char*)memchr(line, '\n', text_end - line);
					if (!line_end)
						line_end = text_end;
					std::string s = std::string(line, (int)(line_end - line));
					const char* str = s.c_str();
					if (alib_streqn(s.c_str(), "[NM:", 4)) {
						int emLen = alib_strspan(s.c_str() + 4, ":/]");
						std::string substr = s.substr(4, emLen);
						s = s.c_str() + 7 + emLen;
						name = substr;
					} if (alib_streqn(s.c_str(), "[LE:", 4)) {
						int emLen = alib_strspan(s.c_str() + 4, ":/]");
						std::string substr = s.substr(4, emLen);
						s = s.c_str() + 7 + emLen;
						if (characterEmotions.find(substr) != characterEmotions.end()) {
							assignSprite(SPRITESIDE_LEFT,characterEmotions.at(substr));
						}
					} if (alib_streqn(s.c_str(), "[CE:", 4)) {
						int emLen = alib_strspan(s.c_str() + 4, ":/]");
						std::string substr = s.substr(4, emLen);
						s = s.c_str() + 7 + emLen;
						if (characterEmotions.find(substr) != characterEmotions.end()) {
							assignSprite(SPRITESIDE_CENTER, characterEmotions.at(substr));
						}
					} if (alib_streqn(s.c_str(), "[RE:", 4)) {
						int emLen = alib_strspan(s.c_str() + 4, ":/]");
						std::string substr = s.substr(4, emLen);
						s = s.c_str() + 7 + emLen;
						if (characterEmotions.find(substr) != characterEmotions.end()) {
							assignSprite(SPRITESIDE_RIGHT, characterEmotions.at(substr));
						}
					}
					ImGui::PushCursorPos(ImVec2(pos.x + 8, pos.y + 8 + accum_height));

					ImGui::PushFontScale(1.5f);
					auto nsz = ImGui::CalcTextSize(name.c_str());
					ImGui::PushCursorPos(ImVec2(pos.x + 4, pos.y - (nsz.y + 4)));
					ImGui::TextMulticoloredUnformatted(name.c_str());
					ImGui::PopCursorPos();
					ImGui::PopFontScale();
					auto tsz = ImGui::CalcTextSize(s.c_str());
					ImGui::TextMulticoloredUnformatted(s.c_str());
					accum_height += tsz.y;
					line = line_end + 1;
					ImGui::PopCursorPos();
				}
			}
			if (delta > seconds_for_btn_flash && showsCursor) {
				ImGui::SameLine();
				ImGui::PushFont(rsm::Fonts::symbols);
				ImGui::Text("_");
				ImGui::PopFont();
				if (delta > seconds_for_btn_flash * 2) {
					delta = 0;
				}
			}
			ImGui::PopFont();
			ImGui::End();
			delta += io.DeltaTime;
			if (
				io.MouseClicked
				&& showsCursor
				) {
				if (on_complete != NULL) {
					on_complete();
				}
				is_wanting_dtor = true;
				isInteracting = false;

			}
		}
	};
	struct Alignment{
		static inline const int LEFT = 1;
		static inline const int RIGHT = 2;
		static inline const int CENTER = 3;
		static inline const int TOP = 4;
		static inline const int BOTTOM = 5;
	};
	struct GameOptionsBox {
		static inline bool key_pressed = false;
		d_typedef_func_ty(void, on_complete_t, ABT);
		on_complete_t on_complete = nullptr;
		bool is_wanting_dtor = false;
		std::string title;
		int alignment;
		std::vector<std::string> keys;
		ABTDataStub options;
		int selected_key = 0;
		static inline std::stack<GameOptionsBox*> _mOptionsBoxes;
		bool is_first_frame = true;
		volatile bool wait_a_frame = false;
		void Render() {
			// wait for destructor
			if (is_wanting_dtor || is_first_frame) {
				is_first_frame = false;
				return;
			}
			if (options.is_array()) {
				this->is_wanting_dtor = true;
				cwError::push(cwError::CW_ERROR);
				cwError::serrof("Error rendering GameOptionsBox: Options was an array! It needs to be a map! Are you initializing this properly?");
				cwError::pop();
				return;
			}
			ImGui::PushFont(rsm::Fonts::default_font);
			UI::isInteracting = true;
			ImGuiIO& io = ImGui::GetIO();
			if (io.DisplaySize.x > io.DisplaySize.y) {
				GameTextBox::padding_sides = io.DisplaySize.x * 0.21f;
			}
			else {
				GameTextBox::padding_sides = io.DisplaySize.x * 0.04f;
			}
			ImVec2 sz = ImVec2{ io.DisplaySize.x - (GameTextBox::padding_sides * 1.25f), GameTextBox::padding_bottom * 0.9f };
			ImGui::SetNextWindowSize(sz);
			ImVec2 pos;
			float padding_bottom = (GameTextBox::padding_bottom * 2);
			switch (alignment) {
			default:
			case Alignment::BOTTOM:
			case Alignment::CENTER: pos = { GameTextBox::padding_sides, io.DisplaySize.y - GameTextBox::padding_bottom }; break;
			case Alignment::LEFT: pos = { GameTextBox::padding_sides * 0.5f, io.DisplaySize.y - padding_bottom }; break;
			case Alignment::RIGHT: pos = { io.DisplaySize.x - GameTextBox::padding_sides * 2.25f, io.DisplaySize.y - padding_bottom * 0.5f }; break;
			case Alignment::TOP:pos = { GameTextBox::padding_sides, GameTextBox::padding_bottom }; break;
			}
			ImGui::SetNextWindowPos(pos);
			ImGui::Begin(title.c_str(), 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
			ImRect upper_half = { pos, ImVec2{sz.x, sz.y * 0.5f} };
			ImRect lower_half = { {pos.x, pos.y + sz.y * 0.5f }, ImVec2{sz.x, sz.y * 0.5f}};
			for (int i = 0; i < keys.size(); i++) {
				if (wait_a_frame || is_wanting_dtor) {
					continue;
				}
				std::string _cur_key = keys.at(i);
				if (i == 0 && is_first_frame) {
					selected_key = i;
				}
				json key = this->options.at(_cur_key);
				if (key.is_boolean()) {
					bool _m_selected = true;
					if (selected_key != i) {
						_m_selected = false;
						ImGui::BeginDisabled();
					}
					ImGui::Text(_cur_key.c_str());
					if (!_m_selected) {
						ImGui::EndDisabled();
					}
					if ( io.MouseDoubleClicked && ImGui::GetCurrentWindow()->ClipRect.Contains(io.MousePos)) {
						is_wanting_dtor = true;
						UI::isInteracting = false;
						options.set_bool(keys.at(selected_key), true);
						if (on_complete != NULL) {
							on_complete(options);
						}
					}
					// we need to wait a frame if some action is done, otherwise it'll think the key is pressed when iterating. This will not affect anything visually.
					if (io.MouseClicked && lower_half.Contains(io.MousePos)) {
						if (!key_pressed) {
							key_pressed = true;
							alib_clampptr(&(selected_key += 1), 0, (int)keys.size() - 1);
							wait_a_frame = true;
							continue;
						}
						else {
							key_pressed = false;
						}
					}
					else if (io.MouseDoubleClicked && upper_half.Contains(io.MousePos)) {
						if (!key_pressed) {
							key_pressed = true;
							alib_clampptr(&(selected_key -= 1), 0, (int)keys.size() - 1);
							wait_a_frame = true;
							continue;
						}
						else {
							key_pressed = false;
						}
					}
				}
			}
			ImGui::PopFont();
			ImGui::End();
			wait_a_frame = false;
		}
		GameOptionsBox(std::string m_title, ABTDataStub m_options, on_complete_t m_on_complete) {
			_mOptionsBoxes.push(this);
			this->options = m_options;
			this->title = m_title;
			this->alignment = Alignment::RIGHT;
			this->keys = m_options.get_keys();
			this->on_complete = m_on_complete;
		}
		GameOptionsBox(std::string m_title, ABTDataStub m_options, on_complete_t m_on_complete, int m_alignment) {
			_mOptionsBoxes.push(this);
			this->options = m_options;
			this->title = m_title;
			this->alignment = m_alignment;
			this->keys = m_options.get_keys();
			this->on_complete = m_on_complete;
		}

	};
	void GUIRenderer::Render() {
		_element_arr.shrink_to_fit();
		for (ui_elem_offset = 0; ui_elem_offset < _element_arr.size(); ui_elem_offset++) {
			std::any* tx_elem = _element_arr.at(ui_elem_offset);
			if (!tx_elem) {
				_element_arr.erase(_element_arr.begin() + ui_elem_offset);
				// Will shrink vector next frame.
				continue;
			}
			if (alib_costr(tx_elem->type().name(), "UI::TextElement *")) {
				TextElement* _e = std::any_cast<TextElement*>(*tx_elem);
				if (!_e) {
					_element_arr.erase(_element_arr.begin() + ui_elem_offset);
					continue;
				}
				if (!_e->enabled || _e->__render_on_update) { continue; }
				_e->Render();
				continue;
			}
			if (alib_costr(tx_elem->type().name(), "UI::ButtonElement *")) {
				ButtonElement* _e = std::any_cast<ButtonElement*>(*tx_elem);
				if (!_e) {
					_element_arr.erase(_element_arr.begin() + ui_elem_offset);
					continue;
				}
				if (!_e->enabled || _e->__render_on_update) { continue; }
				_e->Render();
				continue;
			}
			if (alib_costr(tx_elem->type().name(), "UI::ImageElement *")) {
				ImageElement* _e = std::any_cast<ImageElement*>(*tx_elem);
				if (!_e) {
					_element_arr.erase(_element_arr.begin() + ui_elem_offset);
					continue;
				}
				if (!_e->enabled || _e->manualDraw) { continue; }
				_e->Draw();
				continue;
			}
		}
		if (GameTextBox::_mBoxes.size() >= 1) {
			GameTextBox* box = GameTextBox::_mBoxes.top();
			if (!box->is_wanting_dtor) {
				box->Render();
			}
			else {
				GameTextBox::_mBoxes.pop();
				delete box;
			}
		}
		if (GameOptionsBox::_mOptionsBoxes.size() >= 1) {
			GameOptionsBox* box = GameOptionsBox::_mOptionsBoxes.top();
			if (!box->is_wanting_dtor) {
				box->Render();
			}
			else {
				GameOptionsBox::_mOptionsBoxes.pop();
				delete box;
			}
		}
	}
	void GUIRenderer::RenderAnyUpdate() {
		_element_arr.shrink_to_fit();
		//for (ui_elem_offset = 0; ui_elem_offset < _element_arr.size(); ui_elem_offset++) {
		//	std::any* tx_elem = _element_arr.at(ui_elem_offset);
		//	if (!tx_elem) {
		//		_element_arr.erase(_element_arr.begin() + ui_elem_offset);
		//		// Will shrink vector next frame.
		//		continue;
		//	}
		//	if (alib_costr(tx_elem->type().name(), "UI::TextElement *")) {
		//		TextElement* _e = std::any_cast<TextElement*>(*tx_elem);
		//		if (!_e->enabled || !_e->__render_on_update) { continue; }
		//		_e->Render();
		//		continue;
		//	}
		//	if (alib_costr(tx_elem->type().name(), "UI::ButtonElement *")) {
		//		ButtonElement* _e = std::any_cast<ButtonElement*>(*tx_elem);
		//		if (!_e->enabled || !_e->__render_on_update) { continue; }
		//		_e->Render();
		//		continue;
		//	}
		//	if (alib_costr(tx_elem->type().name(), "UI::ImageElement *")) {
		//		ImageElement* _e = std::any_cast<ImageElement*>(*tx_elem);
		//		if (!_e->enabled || !_e->__render_on_update) { continue; }
		//		_e->Render();
		//		continue;
		//	}
		//}
	}
}

#endif
