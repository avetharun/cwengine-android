#ifndef APK_HELPERS_HPP_INCL_PCH
#define APK_HELPERS_HPP_INCL_PCH
#include "../include/apk_helpers.h"
#include "../include/abt/cwerror.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json/json.hpp>
#include "../ani.h"
void apkh_alib_reqlen(size_t* sz, const char* arr) {
    if ((*sz) == 0) {
        (*sz) = strlen(arr);
    }
}
void apkh_alib_file_write(const char* filen, std::string d, size_t l = 0) {
    std::ofstream file(filen, std::ios::out | std::ios::binary);
    apkh_alib_reqlen(&l, d.c_str());
    file.write(d.c_str(), l);
    file.close();
}
std::string apkh_alib_file_read(const char* fname) {
    std::ostringstream buf;
    std::ifstream file(fname, std::ios::binary);
    buf << file.rdbuf();
    file.close();
    return buf.str();
}
const char* root_asset_folder = "/storage/emulated/0/avetharun/htg/";
const char* ReadRootFileBytes(const char* filename) {
    std::string _f_tmp = root_asset_folder;
    _f_tmp += filename;
    return apkh_alib_file_read(filename).c_str();
}
void WriteRootFileBytes(const char* filename, const char* bytes, size_t len) {
    apkh_alib_reqlen(&len, bytes);
    std::string _f_tmp = root_asset_folder;
    _f_tmp += filename;
    apkh_alib_file_write(filename, bytes, len);
}
bool AssetExists(const char* filename) {
    AAsset* asset_descriptor = AAssetManager_open(ANIEnv::app()->activity->assetManager, filename, AASSET_MODE_UNKNOWN);
    if (asset_descriptor) {
        LOGV("[FTH] LoadFromAPK asset %s found", filename);
        AAsset_close(asset_descriptor);
        return true;
    }
    LOGV("[FTH] LoadFromAPK asset %s not found", filename);
    return false;
}
// Helper to retrieve data placed into the /raw/ directory (res/raw)
const char* ReadAssetBytes(const char* filename, size_t* num_bytes_out)
{
    char* out_data = nullptr;
    size_t num_bytes = 0;
    AAsset* asset_descriptor = AAssetManager_open(ANIEnv::app()->activity->assetManager, filename, AASSET_MODE_BUFFER);
    if (asset_descriptor)
    {
    retry_read:
        num_bytes = AAsset_getLength(asset_descriptor);
        out_data = (char*)malloc(num_bytes);
        out_data[num_bytes] = '\0';
        size_t num_bytes_read = AAsset_read(asset_descriptor, out_data, num_bytes);
        AAsset_close(asset_descriptor);
        if (num_bytes != num_bytes_read) {
            cwError::push(cwError::CW_WARN);
            cwError::serrof("Asset %s was not read correctly. Bytes expected: %zx, Bytes read: %zx", filename, num_bytes, num_bytes_read);
            cwError::pop();
            goto retry_read;
        }
    }
    else {
        cwError::push(cwError::CW_ERROR);
        cwError::serrof("Asset %s not found", filename);
        cwError::pop();
        return 0;
    }
    if (num_bytes_out) { *num_bytes_out = num_bytes; }
    return out_data;
}
nlohmann::json* _assets_strings_json__ = nullptr;

#define apkh_alib_malloct(TYPE) (TYPE*)malloc(sizeof(TYPE))
std::string ReadStringAsset(const char* string_id) {
    if (!_assets_strings_json__) {
        if (!AssetExists("strings.json")) {
            return "No string asset.";
        }
        _assets_strings_json__ = apkh_alib_malloct(nlohmann::json);
        *_assets_strings_json__ = nlohmann::json::parse(ReadAssetBytes("strings.json"));
    }
    if (_assets_strings_json__->contains(string_id)) {
        return _assets_strings_json__->at(string_id).get<std::string>();
    }
    return "not found";
}
#include <sys/stat.h>

int apkh_android_asset_fread(void* cookie, char* buf, int size) {
    return AAsset_read((AAsset*)cookie, buf, size);
}

int apkh_android_asset_fwrite(void* cookie, const char* buf, int size) {
    return EACCES; // can't provide write access to the apk
}

fpos_t apkh_android_asset_fseek(void* cookie, fpos_t offset, int whence) {
    return AAsset_seek((AAsset*)cookie, offset, whence);
}

int apkh_android_asset_fclose(void* cookie) {
    AAsset_close((AAsset*)cookie);
    return 0;
}
long apkh_android_asset_ftell(void* cookie) {
    return AAsset_getLength64((AAsset*)cookie);
}
FILE* apkh_android_asset_fopen(const char* fname, AAssetManager* _mgr) {
    AAsset* asset = AAssetManager_open(_mgr, fname, 0);
    if (!asset) return NULL;

    return funopen(asset, apkh_android_asset_fread, apkh_android_asset_fwrite, apkh_android_asset_fseek, apkh_android_asset_fclose);
}
// From res/raw/
std::string ReadAPKFileBytes(const char* file, size_t* len) {
    if (AssetExists(file)) {
        FILE* apk_f = apkh_android_asset_fopen(file, ANIEnv::app()->activity->assetManager);
        // Determine file size
        fseek(apk_f, 0, SEEK_END);
        size_t size = ftell(apk_f);
        char* out = new char[size + 4];
        rewind(apk_f);
        fread(out, sizeof(char), size, apk_f);
        out[size + 1] = '\0';
        std::string __out(out, size);
        LOGI(__out.c_str());
        delete[] out;
        if (len) { *len = size; }
        return __out;
    }
    LOGV("[FTH] Unable to load file %s, as it doesn't exist!", file);
    return "";

}

#endif