#include "input.h"

alib_inline_run r{ (void_0a_f)[]() -> void{
	Input::KP_UP = alib_cvector<int>({ Input::K_up, Input::K_w });
	Input::KP_RIGHT = alib_cvector<int>({ Input::K_right, Input::K_d });
	Input::KP_LEFT = alib_cvector<int>({ Input::K_left, Input::K_a });
	Input::KP_DOWN = alib_cvector<int>({ Input::K_down, Input::K_s });
	Input::m_keys__reverse = alib_reverse_hmap(Input::m_keys);
} };


int Input::ParseKey(alib_string m_key) {
	alib_string k = alib_lowers(m_key);
	if (m_keys.find(k) != m_keys.end()) {
		return m_keys.at(k);
	}
	return K_UNKNOWN;
}

std::string Input::GetKeyName(int key) {
	if (m_keys__reverse.find(key) != m_keys__reverse.end()) {
		return m_keys__reverse.at(key);
	}
	return "k_unknown";
}

std::string Input::GetKeyNameFriendly(int key) {
	if (m_keys__reverse.find(key) != m_keys__reverse.end()) {
		return m_keys__reverse.at(key).substr(2);
	}
	return "unknown";
}

void Input::Keyboard::EmulateKeyboardDown(unsigned char key) {
	keys[key] = true;
	onKeyPress(key);
}

void Input::Keyboard::EmulateKeyboardUp(unsigned char key) {
	keys[key] = false;
	onKeyRelease(key);
}

bool Input::Keyboard::GetKey(int k) { // Is Key pressed?
	return keys[k];
}

bool Input::Keyboard::GetKeyAny(std::vector<int> ks) { // Is any Key pressed?
	bool b = false;
	for (int i = 0; i < ks.size() && !b; i++) {
		b |= keys[ks[i]];
	}
	return b;
}
bool Input::Keyboard::GetKey(std::vector<int> ks) { // Is any Key pressed?
	return GetKeyAny(ks);
}

bool Input::Keyboard::GetKeyPressed(int k) { // Key pressed this frame?
	return keys_frame[k] && !keys_frame_last[k];
}

bool Input::Keyboard::GetKeyPressedAny(std::vector<int> ks) { // any Key pressed this frame?
	bool b = false;
	for (int i = 0; i < ks.size() && !b; i++) {
		b |= keys_frame[ks[i]] && !keys_frame_last[ks[i]];
	}
	return b;
}

void Input::Keyboard::flush() {
	memset(keys, 0, 1024);
	memset(keys_frame, 0, 1024);
}

void Input::Keyboard::update() {
	memcpy(keys_frame_last, keys_frame, 1024);
	memcpy(keys_frame, keys, 1024);
}

void Input::Mouse::update() {
	memset(&m1df, false, sizeof(bool) * 6);
}

void Input::Keyboard::setKeyState(int k, bool state) {
	switch (state) { 
		case 1: EmulateKeyboardDown(k); break;
		case 0: EmulateKeyboardUp(k); break;
	}
}
