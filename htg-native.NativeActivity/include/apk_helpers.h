#ifndef APK_HELPERS_HPP_INCL
#define APK_HELPERS_HPP_INCL
#include <cassert>
#include <string>
#include "../ndk-helper/JNIHelper.h"
	void WriteRootFileBytes(const char* filename, const char* bytes, size_t len = 0);
	// Asset exists in the compiled APK Binary?
	bool AssetExists(const char* filename);
	// Read asset from the APK Binary in the /raw/ folder.
	// This will not check if the file exists beforehand! Ensure it exists when calling.
	const char* ReadAssetBytes(const char* filename, size_t* num_bytes_out = 0);
	// Reads a given string at [string_id] from the strings.json file.
	// This assumes strings.json is at the root of the assets/ folder.
	std::string ReadStringAsset(const char* string_id);
	// Reads file from the assets/ folder
	std::string ReadAPKFileBytes(const char* file, size_t* len = 0);

#endif