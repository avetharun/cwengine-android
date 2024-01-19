
#ifndef cwlib_cwabt_interpreter_hpp
#define cwlib_cwabt_interpreter_hpp

#include <iostream>
#include <variant>

#include <string>
#include <iomanip>
#include <utility>
#include <iostream>
#include <stdexcept>

#define ALIB_JSON_NLOHHMAN
#include <nlohmann/json/json.hpp>
#include "../../utils.hpp"

using json = nlohmann::json;
#define JSONREF json&
struct ABTDataStub : public nlohmann::json {
	void deserialize_json(std::string d) {
		this->clear();
		this->accept(d);
	}
	std::string serialize_json() {
		return this->dump();
	}

	std::string get_string(std::string key) {
		return this->contains(key) ? this->at(key) : "nil";
	}
	int get_int(std::string key) {
		return this->contains(key) ? this->at(key).get<int>(): 0;
	}
	float get_float(std::string key) {
		return this->contains(key) ? this->at(key).get < float > () : 0;
	}
	bool get_bool(std::string key) {
		return this->contains(key) ? this->at(key).get < bool >() : false;
	}
	std::string get_or_default_string(std::string key, std::string m_default) {
		return this->contains(key) ? this->at(key).get<std::string>() : m_default;
	}
	int get_or_default_int(std::string key, int m_default) {
		return this->contains(key) ? this->at(key).get<int>() : m_default;
	}
	float get_or_default_float(std::string key, float m_default) {
		return this->contains(key) ? this->at(key).get < float >() : m_default;
	}
	bool get_or_default_bool(std::string key, bool m_default) {
		return this->contains(key) ? this->at(key).get < bool >() : m_default;
	}
	// Not very efficient! Call once per init please!
	std::vector<std::string> get_keys() {
		std::vector<std::string> out(size());
		int i = 0;
		for (auto& [k,v] : items()) {
			out.assign(i, k);
			i++;
		}
		return out;
	}
	bool m_contains(std::string key) {
		return this->contains(key);
	}
	void set_float(std::string k, float v) {
		(*this)[k] = v;
	}
	void set_int(std::string k, int v) {
		(*this)[k] = v;
	}
	void set_string(std::string k, std::string v) {
		(*this)[k] = v;
	}
	void set_bool(std::string k, bool v) {
		(*this)[k] = v;
	}
	void add_float(std::string k) {
		(*this)[k] = 0.0f;
	}
	void add_int(std::string k) {
		(*this)[k] = 0;
	}
	void add_string(std::string k) {
		(*this)[k] = "nil";
	}
	void add_bool(std::string k) {
		(*this)[k] = false;
	}

	friend void to_json(nlohmann::json& j, const ABTDataStub& self) {
		j = self;
	};
	friend void from_json(const nlohmann::json& j, ABTDataStub& self) {
		self = j;
	}; // except (3)
};
struct ABT : ABTDataStub{
	static const ABT EMPTY;
	// Note: this will return an EMPTY OBJECT
	ABT* get_object(std::string key) {
		return (ABT*)(this->contains(key) ? ((ABT*)this->at(key).get_ptr <binary_t*>()) : &EMPTY);
	}
	void set_object(std::string k, ABT* other) {
		(*this)[k] = other->get_binary();
	}
	void add_object(std::string k) {
		(*this)[k] = ABT{}.get_binary();
	}
	bool isBSON = false;
	ABT() {}
	ABT(bool isBinary) {
		isBSON = isBinary;
	}
	ABT(bool isBinary, std::string filename) {
		isBSON = isBinary;
		this->decompile(filename.c_str());
	}
	ABT(std::string filename) {
		this->decompile(filename.c_str());
	}
	static void dtor(ABT* ref) {
		ref->invalidate();
	}
	void invalidate() {
		if (type() == json::value_t::null) { return; }
		clear();
		delete this;
	}
	void decompile(const char* filen) {
		size_t flen = alib_file_length(filen);
		alib_string fd = alib_file_read(filen);
		if (!isBSON) {
			deserialize_json(fd.c_str());
			return;
		}
		auto i = std::vector<std::uint8_t>(fd.c_str(), fd.c_str() + fd.size());
		this->binary(i);
	}
	void compile(const char* filen) {
		if (!isBSON) {
			alib_file_write(filen, serialize_json().c_str());
			return;
		}
		std::vector<std::uint8_t> bson = json::to_ubjson(*this, true, true);
		std::string s;
		s.reserve(bson.size());
		int i = 0;
		for (auto& byte : bson) {
			s.assign(i, byte);
			i++;
		}
		alib_file_write(filen, s);
	}
};
const ABT ABT::EMPTY = {};
int abt_geti(JSONREF j) {
	return j.get<int>();
}
std::string abt_getstr(JSONREF j) {
	return j.get<std::string>();
}
#undef JSONREF


#endif	