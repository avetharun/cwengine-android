#include "../utils.hpp"
#include "../imgui/imgui.h"
// requires at least C++11
const std::string vformat(const char* const zcFormat, ...) {

    // initialize use of the variable argument array
    va_list vaArgs;
    va_start(vaArgs, zcFormat);

    // reliably acquire the size
    // from a copy of the variable argument array
    // and a functionally reliable call to mock the formatting
    va_list vaArgsCopy;
    va_copy(vaArgsCopy, vaArgs);
    const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaArgsCopy);
    va_end(vaArgsCopy);

    // return a formatted string without risking memory mismanagement
    // and without assuming any compiler or platform specific behavior
    std::vector<char> zc(iLen + 1);
    std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
    va_end(vaArgs);
    return std::string(zc.data(), iLen);
}



alib_string::alib_string(std::string other) {
    this->swap(other);
}
alib_string::alib_string() {}

alib_string::alib_string(const char* buf) {
    this->append(buf);
}

template <typename T>
std::vector<T> alib_cvector(std::initializer_list<T> list)
{
    std::vector<T> retval{};
    for (T elem : list) {
        retval.push_back(elem);
    }
    return retval;
}
template<typename K, typename V>
std::map<V, K> alib_reverse_map(const std::map<K, V>& m) {
    std::map<V, K> r;
    for (const auto& kv : m) {
        r[kv.second] = kv.first;
    }
    return r;
}
template<typename K, typename V>
std::unordered_map<V, K> alib_reverse_hmap(const std::unordered_map<K, V>& m) {
    std::unordered_map<V, K> r;
    for (const auto& kv : m) {
        r[kv.second] = kv.first;
    }
    return r;
}

template <typename k, typename v>
bool alib_VectorWithPairContains(std::vector<std::pair<k, v>> vec, k key) {
    for (int i = 0; i < vec.size(); i++) {
        if (vec.at(i).first == key) {
            return true;
        }
    }
    return false;
}

template <typename v>
size_t alib_FindValueInVector(std::vector<v> vec, v val) {
    for (int i = 0; i < vec.size(); i++) {
        if (vec.at(i) == val) {
            return i;
        }
    }
    return vec.size();
}

template <typename k, typename v>
size_t alib_FindOffsetOfPairInVector(std::vector<std::pair<k, v>> vec, k key) {
    for (int i = 0; i < vec.size(); i++) {
        if (vec.at(i).first == key) {
            return i;
        }
    }
    return vec.size();
}

template<typename T>
inline void alib_clampptr(T* out, T lower, T upper) {
    *out =
        (*out <= lower) ? lower : // out <= lower : return lower
        (*out <= upper) ? *out :  // out <= upper : return out
        upper;                     // out >  upper : return upper
}

template<typename T>
inline T alib_clamp(T n, T lower, T upper) {
    return n <= lower ? lower : n <= upper ? n : upper;
}

template<typename T>
void alib_invalidatev(std::vector<T> __v) {
    for (int i = 0; i < __v.size(); i++) {
        delete& __v.at(i);
    }
    __v.clear();
}

template<typename K, typename V>
void alib_invalidatem(std::map<K, V> m) {
    for (const auto& kv : m) {
        delete kv.second;
    }
    m.clear();
}
template<typename V_T>
void alib_remove_if(std::vector<V_T> _vec, std::function<bool(V_T)> _p) {
    for (int i = 0; i < _vec.size(); i++) {
        if (_p(_vec.at(i))) {
            _vec.erase(_vec.begin() + i);
        }
    }
}

float alib_rad2deg(float rad) {
    return rad * 180 / M_PI;
}

float alib_deg2rad(float deg) {
    return deg * M_PI / 180;
}

long long alib_time() {
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    return now_ms.time_since_epoch().count();
}

int alib_randi_range(int min, int max) { return min + rand() % ((max + 1) - min); }

float alib_randf01() { return static_cast <float> (rand()) / static_cast <float> (RAND_MAX); }

float alib_randf() { return alib_randf01() * FLT_MAX; }

float alib_randf_range(int min, int max) { return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min))); }

bool alib_atob(const char* buffer) {
    size_t l = strlen(buffer);
    if (l >= 1) {
        if (buffer[0] == 't' || buffer[0] == '1') {
            return true;
        }
        if (buffer[0] == 'f' || buffer[0] == '0') {
            return false;
        }
    }
    return false;
}

char* alib_itoa(int num, char* buffer, int base) {
    int curr = 0;

    if (num == 0) {
        // Base case
        buffer[curr++] = '0';
        buffer[curr] = '\0';
        return buffer;
    }

    int num_digits = 0;

    if (num < 0) {
        if (base == 10) {
            num_digits++;
            buffer[curr] = '-';
            curr++;
            // Make it positive and finally add the minus sign
            num *= -1;
        }
        else
            // Unsupported base. Return NULL
            return NULL;
    }

    num_digits += (int)floor(log(num) / log(base)) + 1;

    // Go through the digits one by one
    // from left to right
    while (curr < num_digits) {
        // Get the base value. For example, 10^2 = 1000, for the third digit
        int base_val = (int)pow(base, num_digits - 1 - curr);

        // Get the numerical value
        int num_val = num / base_val;

        char value = num_val + '0';
        buffer[curr] = value;

        curr++;
        num -= base_val * num_val;
    }
    buffer[curr] = '\0';
    return buffer;
}

char* alib_ftoa(char* buf, size_t len, float f)
{
    char* cp = buf;
    unsigned long    l, rem;

    if (f < 0) {
        *cp++ = '-';
        f = -f;
    }
    l = (unsigned long)f;
    f -= (float)l;
    rem = (unsigned long)(f * 1e6);
    snprintf(cp, len, "%lu.%6.6lu", l, rem);
    return buf;
}

#ifdef WIN32
bool alib_can_reach_mem(void* ptr) {
    __try {
        char prefix = *(((char*)ptr)); //Get the first byte. If this doesn't cause an error, then we can reach this.
        return true;
    }
    __except (true) { //Catch all unique exceptions (Windows exceptions) 
        return false; //Can't reach this memory
    }
}
#endif



void __alib_internal_reqlen__f_impl(size_t* sz, alib_string arr) {
    if ((*sz) == 0) {
        *sz = arr.size();
    }
}
bool alib_file_exists(const char* name) {
    if (FILE* file = fopen(name, "r")) {
        fclose(file);
        return true;
    }
    else {
        return false;
    }
}
#include <sys/stat.h>
size_t alib_file_length(FILE* file, struct stat* file_stat) {
    fstat(_fileno(file), file_stat);
    size_t sz = file_stat->st_size;
    return sz;
}
size_t alib_file_length(FILE* file) {
    struct stat file_stat;
    fstat(_fileno(file), &file_stat);
    size_t sz = file_stat.st_size;
    return sz;
}
size_t alib_file_length(const char* fname) {
    if (!alib_file_exists(fname)) {
        return 0;
    }
    FILE* file = fopen(fname, "rb");
    size_t sz = alib_file_length(file);
    fclose(file);
    return sz;
}
std::string alib_file_read(std::ifstream& file) {
    std::ostringstream buf;
    buf << file.rdbuf();
    return buf.str();
}
std::string alib_file_read(const char* fname) {
    std::ostringstream buf;
    std::ifstream file(fname, std::ios::binary);
    buf << file.rdbuf();
    file.close();
    return buf.str();
}
void alib_file_read(const char* fname, const char* out_, unsigned long long* size_) {
    if (!alib_file_exists(fname)) {
        return;
    }
    FILE* file = fopen(fname, "rb");
    struct stat file_stat;
    size_t len = alib_file_length(file, &file_stat);
    *size_ = len;
    if (!out_) {
        out_ = alib_malloca(char, len);
    }
    fread((void*)out_, len, 1, file);
    fclose(file);
}
void alib_file_read(const char* fname, const char* out_, unsigned long long size_) {
    if (!alib_file_exists(fname)) {
        return;
    }
    FILE* file = fopen(fname, "rb");
    struct stat file_stat;
    size_t len = alib_file_length(file, &file_stat);
    if (!out_) {
        out_ = alib_malloca(char, len);
    }
    fread((void*)out_, len, 1, file);
    fclose(file);
}
void alib_file_write(const char* filen, alib_string d, size_t l) {
    std::ofstream file(filen, std::ios::out | std::ios::binary);
    __alib_internal_reqlen__f_impl(&l, d);
    file.write(d, l);
    file.close();
}
void alib_file_write(std::ostream& file, const char* d, size_t l) {
    __alib_internal_reqlen__f_impl(&l, d);
    file.write(d, l);
}
bool alib_scwd(const char* dir) { return CHDIR(dir) == 0; }
char* alib_gcwd() {
    return GETCWD(0, 0);
}
#if (defined(ALIB_FORCE_WIN_UTILS) && defined(_WIN32)) && (!defined(ALIB_NO_WIN_UTILS)) 
#include <Windows.h>
#include "../imgui/imgui.h"
#include "../imgui/imgui.h"
void alib_hide_console()
{
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}
void alib_show_console()
{
    ::ShowWindow(::GetConsoleWindow(), SW_SHOW);
}
bool alib_console_visible()
{
    return ::IsWindowVisible(::GetConsoleWindow()) != FALSE;
}
#endif

unsigned long alib_fnull(const char* arr) {
    int n_t_offset = 0;
    while (*arr++ != '\0') { n_t_offset++; }
    return n_t_offset;
}

unsigned long alib_n_fnull(const char* arr, int limit) {
    for (int i = 0; i < limit; i++) {
        if (arr[i] == '\0') {
            return i;
        }
    }
    return 0;
}

size_t alib_nulpos(const char* arr) {
    return alib_fnull(arr);
}

size_t alib_nulposn(const char* arr, int limit) {
    return alib_n_fnull(arr, limit);
}

int alib_log(int base, int n) {
#ifndef _CMATH_
    // 75% as fast as cmath, if using recursion. If we have cmath avalible, use that instead.
    return (n > base - 1)
        ? 1 + alib_log(n / base, base)
        : 0;
#else
    return (int)(log(n) / log(base));
#endif
}

bool alib_wrange(int min, int max, int val) {
    return ((val - max) * (val - min) <= 0);
}

bool alib_fisclose(float first, float second, float rel_tol, float abs_tol) {
    return abs(first - second) <= alib_max(rel_tol * alib_max(abs(first), abs(second)), abs_tol);
}

bool alib_disclose(double first, double second, double rel_tol, double abs_tol) {
    return abs(first - second) <= alib_max(rel_tol * alib_max(abs(first), abs(second)), abs_tol);
}

bool alib_iswithin(float first, float second, float amount) {
    float a = abs(first - second);
    return (a < amount);
}

int alib_digitsInNum(long n, int base)
{
    if (n == 0) { return 1; }
    if (n < 0) {
        n *= -1;
    }
    // log using base, then
    // taking it to the lowest int then add 1
    return (int)floor(alib_log(base, n)) + 1;
}

char* alib_getDigitsOfNumber(int num, bool ascii) {
    int amt_digits = alib_digitsInNum(num, 10);
    if (amt_digits <= 0) {
        amt_digits = 1;
    }
    char* digits = (char*)malloc(amt_digits + 1);
    int i = 1;
    while (num != 0)
    {
        // What even is math?
        const int least_significant_digit = num % 10;
        digits[i] = (least_significant_digit + ((ascii) ? 48 : 0));
        num /= 10;
        i++;
    }

    return digits;
}

void alib_reqlen(size_t* sz, const char* arr) {
    if ((*sz) == 0) {
        (*sz) = strlen(arr);
    }
}

const char* alib_bit_rep[16] = {
    "0000","0001","0010","0011",
    "0100","0101","0110","0111",
    "1000","1001","1010","1011",
    "1100","1101","1110","1111",
};
void alib_print_byte(uint8_t byte)
{
    printf("%s%s", alib_bit_rep[byte >> 4], alib_bit_rep[byte & 0x0F]);
}

int alib_endswith(const char* str, const char* suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

int alib_beginswith(const char* str, const char* prefix)
{
    if (!str || !prefix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lenprefix = strlen(prefix);
    if (lenprefix > lenstr)
        return 0;
    return strncmp(str, prefix, lenprefix) == 0;
}

int alib_substr2(const char* input, size_t input_len) {

}

int alib_beginswith_anddoesntfollow(const char* str, const char* prefix)
{
    if (!str || !prefix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lenprefix = strlen(prefix);
    if (lenprefix > lenstr)
        return 0;
    // No character after
    if (lenstr < lenprefix + 1) { return strncmp(str, prefix, lenprefix) == 0; }
    return strncmp(str, prefix, lenprefix) == 0 && str[lenprefix] != prefix[lenprefix - 1];
}

int alib_getchrpos(const char* src, char c, size_t len)
{
    alib_reqlen(&len, src);
    for (size_t i = 0; i < len; i++)
    {
        if (src[i] == c)
            return (int)i;
    }
    return -1;
}

bool alib_costr(std::string src, const char* match) {
    if (src.find(match) != std::string::npos) {
        return true;
    }
    return false;
}

int alib_chreq(const char* src, const char* match) {
    size_t sl = strlen(src);
    size_t ml = strlen(match);
    if (ml > sl) { return false; }
    return strncmp(src, match, ml);
}

int alib_streq(std::string src, const char* match) {
    size_t sl = src.size();
    size_t ml = strlen(match);
    if (ml > sl) { return false; }
    return (src.compare(match) == 0);
}

int alib_streqn(std::string src, const char* match, const size_t len) {
    return strncmp(src.c_str(), match, len) == 0;
}

int alib_strspan(const char* str, const char* match, size_t len, size_t match_len) {
    alib_reqlen(&len, str);
    alib_reqlen(&match_len, match);
    for (size_t i = 0; i < len; i++) {
        if (alib_streqn(str + i, match, match_len)) {
            return i;
        }
    }
    return -1;
}

size_t alib_chrocc(const char* src, char c, size_t len) {
    alib_reqlen(&len, src);
    size_t occ = 0;
    for (size_t i = 0; i < len; i++) {
        if (src[i] == c) { occ++; }
    }
    return occ;
}

const char* alib_rmocc(const char* src, char c, size_t len) {
    alib_reqlen(&len, src);
    std::vector<char> src_copy;
    for (int i = 0; i < len; i++) {
        if (src[i] == c) { continue; }
        src_copy.push_back(src[i]);
    }
    src_copy.push_back('\0');
    return src_copy.data();
}

char alib_get_byte(void* data, int offset) {
    return ((char*)data)[offset];
}

char alib_get_byte(void* data) {
    return ((char*)data)[0];
}

void alib_set_byte(void* data, char byte, int offset) {
    reinterpret_cast<char*>(data)[offset] = byte;
}

void alib_set_byte(void* data, char byte) {
    reinterpret_cast<char*>(data)[0] = byte;
}

#ifdef __GNUC__
int vscprintf(const char* format, va_list ap)
{
    va_list ap_copy;
    va_copy(ap_copy, ap);
    int retval = vsnprintf(NULL, 0, format, ap_copy);
    va_end(ap_copy);
    return retval;
}
#endif
#ifdef _MSC_VER
int vasprintf(char** strp, const char* format, va_list ap)
{
    int len = vscprintf(format, ap);
    if (len == -1)
        return -1;
    char* str = (char*)malloc((size_t)len + 1);
    if (!str)
        return -1;
    int retval = vsnprintf(str, len + 1, format, ap);
    if (retval == -1) {
        free(str);
        return -1;
    }
    *strp = str;
    return retval;
}
int asprintf(char** strp, const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    int retval = vasprintf(strp, format, ap);
    va_end(ap);
    return retval;
}
#endif


size_t alib_2d_ar_pos(size_t pitch, size_t x, size_t y, size_t bytes_per_step) {
    return y * pitch + x * bytes_per_step;
}

alib_xy alib_idx_to_xy(int width, int idx) {
    alib_xy xy{};
    xy.x = (idx % width);
    xy.y = (idx / width);
    return xy;
}

int alib_xy_to_idx(int width, alib_xy xy) {
    return xy.x + width * xy.y;
}

size_t alib_va_arg_length(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    size_t bufsz = snprintf(NULL, 0, fmt, args);
    va_end(args);
    return bufsz;
}

size_t alib_va_arg_length(const char* fmt, va_list args) {
    size_t bufsz = vsnprintf(NULL, 0, fmt, args);
    return bufsz;
}

void alib_urlhttpopen_ext(alib_string url) {
#ifdef ALIB_WIN
    ShellExecuteA(0, 0, url, 0, 0, SW_SHOW);
#endif
}

alib_string alib_urlhttpget(alib_string url) {
#ifdef ALIB_WIN


#endif
}

alib_string alib_strfmtv(const char* zcFormat, va_list args) {
    // reliably acquire the size
    // from a copy of the variable argument array
    // and a functionally reliable call to mock the formatting
    va_list vaArgsCopy;
    va_copy(vaArgsCopy, args);
    const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaArgsCopy);
    va_end(vaArgsCopy);

    // return a formatted string without risking memory mismanagement
    // and without assuming any compiler or platform specific behavior
    std::vector<char> zc(iLen + 1);
    std::vsnprintf(zc.data(), zc.size(), zcFormat, args);
    return std::string(zc.data(), iLen);
}
// requires at least C++11
alib_string alib_strfmt(const char* const zcFormat, ...) {

    // initialize use of the variable argument array
    va_list vaArgs;
    va_start(vaArgs, zcFormat);

    // reliably acquire the size
    // from a copy of the variable argument array
    // and a functionally reliable call to mock the formatting
    va_list vaArgsCopy;
    va_copy(vaArgsCopy, vaArgs);
    const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaArgsCopy);
    va_end(vaArgsCopy);

    // return a formatted string without risking memory mismanagement
    // and without assuming any compiler or platform specific behavior
    std::vector<char> zc(iLen + 1);
    std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
    va_end(vaArgs);
    return std::string(zc.data(), iLen);
}


float alib_lerp_low(float a, float b, float f)
{
    return a + f * (b - a);
}

float alib_lerp(float a, float b, float f) {
    return (a * (1.0f - f)) + (b * f);
}

float alib_clerp(float a, float b, float f) {
    float o = (a * (1.0f - f)) + (b * f);
    if (o < a) { return a; }
    if (o > b) { return b; }
    return o;
}

const char* alib_chrrepl(const char* in, char match, char repl_value) {
    while (*(in++)) {
        if (*in == match) {
            (*(char*)in) = repl_value;
        }
    }
    return in;
}

void alib_copy_signed(signed int a, signed int* b) {
    *b = (a < 0) ? -*b : (*b < 0) ? -*b : *b;
}

std::wstring convert_wstr(std::string str) {
    std::wstring str2(str.length(), L' '); // Make room for characters
    std::copy(str.begin(), str.end(), str2.begin());
    return str2;
}

void alib_split(std::string arr, std::string del, std::vector<std::string>* out)
{
    size_t start = 0;
    size_t end = arr.find(del);
    while (end != -1) {
        out->push_back(arr.substr(start, end - start));
        start = end + del.size();
        end = arr.find(del, start);
    }
    out->push_back(arr.substr(start, end - start));
}

void alib_split_quoted(std::string arr, std::vector<std::string>* out)
{
    size_t start = 0;
    size_t end = arr.find("\"");
    while (end != -1) {
        out->push_back(arr.substr(start, end - start));
        start = end + 1;
        end = arr.find("\"", start);
    }
    out->push_back(arr.substr(start, end - start));
}

std::vector<std::string> alib_split(const std::string& s, char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;
    while (std::getline(ss, item, delim)) {
        //elems.push_back(item);
        elems.push_back(std::move(item)); // if C++11 (based on comment from @mchiasson)
    }
    return elems;
}

void alib_strsplit(std::string& str, char delim, std::vector<std::string>& out)
{
    size_t start;
    size_t end = 0;
    // Doesn't contain 'delim', so return the base string itself.
    if ((str.find_first_not_of(delim, end)) == std::string::npos) {
        out.push_back(str);
        return;
    }
    while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
    {
        end = str.find(delim, start);
        out.push_back(str.substr(start, end - start));
    }
}

std::string alib_str_hex(std::string s) {

    std::stringstream s1;
    for (int i = 0; i < s.length(); i++) {
        s1 << std::hex << (int)s.at(i);
    }

    return s1.str();
}

std::string alib_int_hex(long l) {
    std::stringstream stream;
    stream << std::hex << l;
    return std::string(stream.str());
}

std::string alib_lower(const char* s)
{
    std::string s2 = s;
    std::transform(s2.begin(), s2.end(), s2.begin(), tolower);
    return s2;
}

std::string alib_lowers(std::string s2)
{
    std::transform(s2.begin(), s2.end(), s2.begin(), tolower);
    return s2;
}

std::string alib_upper(const char* s)
{
    std::string s2 = s;
    std::transform(s2.begin(), s2.end(), s2.begin(), toupper);
    return s2;
}

std::string alib_uppers(std::string s2)
{
    std::transform(s2.begin(), s2.end(), s2.begin(), toupper);
    return s2;
}

double alib_percent(long double num, double percent) {
    long double _n_d100 = (num / 100);
    return lroundl(_n_d100 * percent);
}

float alib_percentf(float num, float percent) {
    float _n_d100 = (num * 0.01);
    return lroundf(_n_d100 * percent);
}

int alib_percents(int base, std::string percent_str) {
    if ((alib_endswith(percent_str.c_str(), "%"))) percent_str.erase(percent_str.end());
    int percent = atoi(percent_str.c_str());
    return alib_percent(base, percent);
}
template <class T1, class T2>
std::size_t pair_hash::operator () (const std::pair<T1, T2>& p) const {
    auto h1 = std::hash<T1>{}(p.first);
    auto h2 = std::hash<T2>{}(p.second);

    // Combining the hash values to get a unique hash for the pair
    return h1 ^ h2;
}
alib_MARKDOWN alib_parseRRGGBB_AAOPT(alib_string str) {
    alib_MARKDOWN struc;
    struc.hasColor = false;
    struc.hasMarkdown = false;
    uint32_t hexColor = 0;
    bool validHex = true;
    int mdIDX = 0;
    int validHexChars = 0;
    mdIDX = str.find_first_of('$');
    for (int i = 0; i < alib_min((int)str.size(), 8); i++) {
        char c = str.at(i);
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
            validHex = false;
            break;
        }
        validHexChars++;
    }
    bool isRRGGBBAA = validHexChars == 8;
    if (validHexChars == 6 || validHexChars == 8)
    {
        char hex[9];
        strncpy(hex, str.c_str(), validHexChars);
        hex[validHexChars] = 0;
        unsigned int hexColor = 0;
        if (sscanf(hex, "%x", &hexColor) > 0)
        {
            if (validHexChars == 8)
            {
                struc.r = static_cast<float>((hexColor & 0xFF000000) >> 24) / 255.0f;
                struc.g = static_cast<float>((hexColor & 0x00FF0000) >> 16) / 255.0f;
                struc.b = static_cast<float>((hexColor & 0x0000FF00) >> 8) / 255.0f;
                struc.a = static_cast<float>((hexColor & 0x000000FF)) / 255.0f;
            }
            else if (validHexChars == 6) {
                struc.r = static_cast<float>((hexColor & 0x00FF0000) >> 16) / 255.0f;
                struc.g = static_cast<float>((hexColor & 0x0000FF00) >> 8) / 255.0f;
                struc.b = static_cast<float>((hexColor & 0x000000FF)) / 255.0f;
                struc.a = 1.0f;
            }
            struc.hasColor = true;
        }
    }
    else {
        struc.r = 1; struc.g = 1; struc.b = 1; struc.a = 1;
    }
    if (mdIDX > 0) {
        std::string sub = str.substr(mdIDX);
        for (int i = 0; i < sub.size(); i++) {
            switch (sub.at(i)) {
            case 'i':
                struc.italic = true;
                break;
            case 'b':
                struc.bold = true;
                break;
            case 's':
                struc.strikethrough = true;
                break;
            case 'u':
                struc.underscore = true;
                break;
            }
        }
        struc.hasMarkdown = true;
    }
    return struc;
}
#if !defined(ALIB_PROJECT_NAME)
#define ALIB_PROJECT_NAME "UnnamedAndroidProject (DEFINE USING #define ALIB_PROJECT_NAME '')"
#endif
int android_asset_fread(void* cookie, char* buf, int size) {
    return AAsset_read((AAsset*)cookie, buf, size);
}
int android_asset_fwrite(void* cookie, const char* buf, int size) {
    return EACCES; // can't provide write access to the apk
}
fpos_t android_asset_fseek(void* cookie, fpos_t offset, int whence) {
    return AAsset_seek((AAsset*)cookie, offset, whence);
}
int android_asset_fclose(void* cookie) {
    AAsset_close((AAsset*)cookie);
    return 0;
}
long android_asset_ftell(void* cookie) {
    return AAsset_getLength64((AAsset*)cookie);
}
FILE* android_asset_fopen(const char* fname, AAssetManager* _mgr) {
    AAsset* asset = AAssetManager_open(_mgr, fname, 0);
    if (!asset) return NULL;

    return funopen(asset, android_asset_fread, android_asset_fwrite, android_asset_fseek, android_asset_fclose);
}

// Constructors, destructor
template<typename T>
ALStack<T>::ALStack() { Size = Capacity = 0; Data = NULL; }

template<typename T>
ALStack<T>::ALStack(const ALStack<T>& src) { Size = Capacity = 0; Data = NULL; operator=(src); }

template<typename T>
ALStack<T>& ALStack<T>::operator=(const ALStack<T>& src) { clear(); resize(src.Size); memcpy(Data, src.Data, (size_t)Size * sizeof(T)); return *this; }

template<typename T>
ALStack<T>::~ALStack() { if (Data) free(Data); }

template<typename T>
void ALStack<T>::clear() { if (Data) { Size = Capacity = 0; free(Data); Data = NULL; } }

// Important: does not destruct anything
template<typename T>
void ALStack<T>::clear_delete() { for (int n = 0; n < Size; n++) Data[n].~T(); clear(); }

// Important: never called automatically! always explicit.
template<typename T>
void ALStack<T>::clear_destruct() { for (int n = 0; n < Size; n++) Data[n].~T(); clear(); }

template<typename T>
bool ALStack<T>::empty() const { return Size == 0; }

template<typename T>
int ALStack<T>::size() const { return Size; }

template<typename T>
int ALStack<T>::size_in_bytes() const { return Size * (int)sizeof(T); }

template<typename T>
int ALStack<T>::max_size() const { return 0xff / (int)sizeof(T); }

template<typename T>
int ALStack<T>::capacity() const { return Capacity; }

template<typename T>
T* ALStack<T>::begin() { return Data; }

template<typename T>
const T* ALStack<T>::begin() const { return Data; }

template<typename T>
T* ALStack<T>::end() { return Data + Size; }

template<typename T>
const T* ALStack<T>::end() const { return Data + Size; }

template<typename T>
T& ALStack<T>::front() { assert(Size > 0); return Data[0]; }

template<typename T>
const T& ALStack<T>::front() const { assert(Size > 0); return Data[0]; }

template<typename T>
T& ALStack<T>::back() { assert(Size > 0); return Data[Size - 1]; }

template<typename T>
T& ALStack<T>::top() { return back(); }

template<typename T>
const T& ALStack<T>::back() const { assert(Size > 0); return Data[Size - 1]; }

template<typename T>
const T& ALStack<T>::top() const { return back(); }

template<typename T>
T& ALStack<T>::at(int amt) { assert(amt < Size); return *(Data + amt); }

template<typename T>
void ALStack<T>::swap(ALStack<T>& rhs) { int rhs_size = rhs.Size; rhs.Size = Size; Size = rhs_size; int rhs_cap = rhs.Capacity; rhs.Capacity = Capacity; Capacity = rhs_cap; T* rhs_data = rhs.Data; rhs.Data = Data; Data = rhs_data; }

template<typename T>
T* ALStack<T>::insert(const T* it, const T& v) { assert(it >= Data && it <= Data + Size); const ptrdiff_t off = it - Data; if (Size == Capacity) reserve(_grow_capacity(Size + 1)); if (off < (int)Size) memmove(Data + off + 1, Data + off, ((size_t)Size - (size_t)off) * sizeof(T)); memcpy(&Data[off], &v, sizeof(v)); Size++; return Data + off; }

template<typename T>
bool ALStack<T>::contains(const T& v) const { const T* data = Data;  const T* data_end = Data + Size; while (data < data_end) if (*data++ == v) return true; return false; }

template<typename T>
int ALStack<T>::_grow_capacity(int sz) const { int new_capacity = Capacity ? (Capacity + Capacity / 2) : 8; return new_capacity > sz ? new_capacity : sz; }

template<typename T>
void ALStack<T>::resize(int new_size) { if (new_size > Capacity) reserve(_grow_capacity(new_size)); Size = new_size; }

template<typename T>
void ALStack<T>::resize(int new_size, const T& v) { if (new_size > Capacity) reserve(_grow_capacity(new_size)); if (new_size > Size) for (int n = Size; n < new_size; n++) memcpy(&Data[n], &v, sizeof(v)); Size = new_size; }

template<typename T>
void ALStack<T>::shrink(int new_size) { assert(new_size <= Size); Size = new_size; }

// Resize a vector to a smaller size, guaranteed not to cause a reallocation
template<typename T>
void ALStack<T>::reserve(int new_capacity) { if (new_capacity <= Capacity) return; T* new_data = (T*)malloc((size_t)new_capacity * sizeof(T)); if (Data) { memcpy(new_data, Data, (size_t)Size * sizeof(T)); free(Data); } Data = new_data; Capacity = new_capacity; }

// NB: It is illegal to call push_back/push_front/insert with a reference pointing inside the ImVector data itself! e.g. v.push_back(v[10]) is forbidden.
template<typename T>
void ALStack<T>::push_back(const T& v) { if (Size == Capacity) { reserve(_grow_capacity(Size + 1)); } memcpy(&Data[Size], &v, sizeof(v)); Size++; }

template<typename T>
void ALStack<T>::pop_back() { assert(Size > 0); Size--; }

template<typename T>
void ALStack<T>::pop() { 
	assert(Size > 0);
	Size--;
}

template<typename T>
void ALStack<T>::push(const T& v) { if (Size == 0) push_back(v); else insert(Data, v); }

template<typename T>
void ALStack<T>::push_front(const T& v) { if (Size == 0) push_back(v); else insert(Data, v); }

alib_inline_run::alib_inline_run(std::function<void()> initFunc) {
    initFunc();
}

alib_inline_run::alib_inline_run(void_0a_f initFunc) {
    initFunc();
}

template<typename T, int max_elements>
void alib_average<T, max_elements>::add(T element) {
    if (m_stack.size() >= length) {
        m_stack.pop_back();
    }
    m_stack.push_front(element);
}

template<typename T, int max_elements>
T alib_average<T, max_elements>::get() {
    T sum = 0;
    int amt = 0;
    for (int i = 0; i < length && i < m_stack.size(); i++) {
        sum += *(m_stack.Data + i);
        amt++;
    }
    if (amt)
        return sum / amt;
    return 0;
}


int alib_j_geti(nlohmann::json j) {
    return j.get<int>();
}
float alib_j_getf(nlohmann::json j) {
    return j.get<float>();
}
double alib_j_getd(nlohmann::json j) {
    return j.get<double>();
}
std::string alib_j_getstr(nlohmann::json j) {
    return j.get<std::string>();
}
const char* alib_j_getchara(nlohmann::json j) {
    return alib_j_getstr(j).c_str();
}
bool alib_j_streq(nlohmann::json j, std::string match) {
    std::string src = j.get<std::string>();
    if (src.size() < match.size()) {}
    return (src.compare(match) == 0);
}
bool alib_j_costr(nlohmann::json j, std::string match) {
    if (j.is_array()) {
        for (int i = 0; i < j.size(); i++) {
            if (j[i].get<std::string>().compare(match.c_str()) == 0) { return true; };
        }
    }
    // If j_ty = array : the following will always return false.
    return j.contains(match);
}
bool alib_j_ieq(nlohmann::json j, int match) {
    return (j.get<int>() == match);
}
bool alib_j_feq(nlohmann::json j, float match) {
    return (j.get<float>() == match);
}

// If [j] contains keys formatted as "a<newline>b<newline>c"
// Note: replace <newline> with \\n (unescaped due to comment)
// (eg if the json element is {"a":{"b":{"c":"d"}}} it would return true)
bool alib_j_cokeys(nlohmann::json j, std::string _s) {
    ___alib__json j_t = j;
    std::vector<std::string> s_keys;
    alib_strsplit(_s, '\n', s_keys);


    for (int i = 0; i < s_keys.size(); i++) {
        std::string s_cur = s_keys[i];
        if (!j_t.contains(s_cur)) {
            return false;
        }
        j_t = j_t.at(s_cur);
    }
    return true;

}
template void alib_clampptr<float>(float*, float, float);
template void alib_clampptr<int>(int*, int, int);
template float alib_clamp<float>(float, float, float);
template int alib_clamp<int>(int, int, int);
template struct ALStack<double>;
template struct alib_average <double, 12>;
template std::vector<int> alib_cvector<int>(std::initializer_list<int>);
template std::unordered_map<int, std::string> alib_reverse_hmap<std::string, int>(const std::unordered_map<std::string, int>&);


bool operator == (const ImVec2& _1, const ImVec2& _2) {
	return memcmp(&_1, &_2, sizeof(ImVec2)) == 0;
};
template struct ALStack<ImVec2>;