#include "cwerror.h"
#include <unordered_map>
#include "../../utils.hpp"
// Return the current or topmost error state


// Return the current error state, or change if argument 0 is not CW_NONE


// warning ID to human readable

const char* cwError::wtoh(uint32_t w) {
	switch (w) {
	default: return "UNKNOWN";
	case CW_NONE: return "";
	case CW_ERROR: return "ERROR";
	case CW_WARN: return "WARN";
	case CW_MESSAGE: return "Log";
	case CW_VERBOSE: return "V";
	case CW_SILENT: return "SILENT";
	case CW_DEBUG: return "DBG";
	}
}

const char* cwError::geterror() {
	return errorStr.c_str();
}

uint32_t cwError::sstate(cwWarnings state) {
	if (state == CW_RETURN__) { return warningState; }
	warningState = state;
	return state;
}

uint32_t cwError::gstate() {
	return WarningTypeStack.size() == 0 ? warningState : WarningTypeStack.top();
}

void cwError::pstate(cwWarnings state) {
	WarningTypeStack.push(state);
}

void cwError::push(cwWarnings state) {
	pstate(state);
}

void cwError::postate() {
	WarningTypeStack.pop();
}

void cwError::pop() {
	postate();
}

void cwError::serror(const char* err) {
	// return if debugging isn't enabled, and are sending a debug message.
	if (!debug_enabled && (warningState == CW_DEBUG)) {
		return;
	}
	errorStr = (char*)err;
	onError(err, sstate());
}

void cwError::serrof(const char* fmt, va_list args) {
	// return if debugging isn't enabled, and are sending a debug message.
	if (!debug_enabled && (warningState == CW_DEBUG)) {
		return;
	}
	errorStr = alib_strfmtv(fmt, args);
	onError(errorStr.c_str(), gstate());
}

void cwError::serroif(bool v, const char* fmt, ...) {
	if (!v) { return; }
	if (!debug_enabled && (warningState == CW_DEBUG)) {
		return;
	}
	errorStr.clear();
	va_list args;
	va_start(args, fmt);
	errorStr.append(alib_strfmtv(fmt, args));
	va_end(args);
	onError(errorStr.c_str(), gstate());
}
std::stack<int> uniqueStack;
void cwError::pushAllowedUniqueStrings(int amt) {
	uniqueStack.push(amt);
}
void cwError::popAllowedUniqueStrings() {
	uniqueStack.pop();
}
void cwError::serrof(const char* fmt, ...) {
	// return if debugging isn't enabled, and are sending a debug message.
	if (!debug_enabled && (warningState == CW_DEBUG)) {
		return;
	}
	errorStr.clear();
	va_list args;
	va_start(args, fmt);
	errorStr.append(alib_strfmtv(fmt, args));
	va_end(args);
	onError(errorStr.c_str(), gstate());
}

std::unordered_map<long long, int> uniqueKeys = {};
void cwError::serrofUnique(const char* fmt, va_list args) {
	// return if debugging isn't enabled, and are sending a debug message.
	if (!debug_enabled && (warningState == CW_DEBUG)) {
		return;
	}
	std::string tmp = alib_strfmtv(fmt, args);
	long long hash = std::hash<std::string>{}(tmp);
	int amtUsed = 0;
	if (uniqueKeys.find(hash) != uniqueKeys.end()) {
		if (uniqueKeys.at(hash) > uniqueStack.top()) { return; }
		amtUsed = uniqueKeys.at(hash);
	}
	uniqueKeys.insert_or_assign(hash, amtUsed + 1);
	errorStr = tmp;
	onError(errorStr.c_str(), gstate());
}
void cwError::serroifUnique(bool v, const char* fmt, ...) {
	if (!v) { return; }
	if (!debug_enabled && (warningState == CW_DEBUG)) {
		return;
	}
	va_list args;
	va_start(args, fmt);
	serrofUnique(fmt, args);
	va_end(args);
}
void cwError::serrofUnique(const char* fmt, ...) {
	// return if debugging isn't enabled, and are sending a debug message.
	if (!debug_enabled && (warningState == CW_DEBUG)) {
		return;
	}
	va_list args;
	va_start(args, fmt);
	serrofUnique(fmt, args);
	va_end(args);
}