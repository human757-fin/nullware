#include "src/common/string_utils.h"

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <random>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>

#ifdef NULLWARE_PLATFORM_WINDOWS
#include <windows.h>
#include <wincrypt.h>
#else
#include <openssl/md5.h>
#include <openssl/sha.h>
#endif

namespace nullware {

std::string TrimString(const std::string& str) {
    return TrimStringRight(TrimStringLeft(str));
}

std::string TrimStringLeft(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    return (start == std::string::npos) ? "" : str.substr(start);
}

std::string TrimStringRight(const std::string& str) {
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

std::string ToLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string ToUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

bool StartsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && 
           str.compare(0, prefix.size(), prefix) == 0;
}

bool EndsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && 
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool Contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

std::string ReplaceAll(const std::string& str, const std::string& from, const std::string& to) {
    std::string result = str;
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}

std::vector<std::string> SplitString(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    return result;
}

std::vector<std::string> SplitString(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> result;
    size_t pos = 0;
    size_t prev_pos = 0;
    
    while ((pos = str.find(delimiter, prev_pos)) != std::string::npos) {
        std::string item = str.substr(prev_pos, pos - prev_pos);
        if (!item.empty()) {
            result.push_back(item);
        }
        prev_pos = pos + delimiter.length();
    }
    
    // Add the last part
    std::string last_item = str.substr(prev_pos);
    if (!last_item.empty()) {
        result.push_back(last_item);
    }
    
    return result;
}

std::string JoinString(const std::vector<std::string>& parts, const std::string& delimiter) {
    if (parts.empty()) return "";
    
    std::string result = parts[0];
    for (size_t i = 1; i < parts.size(); ++i) {
        result += delimiter + parts[i];
    }
    return result;
}

bool IsStringEmptyOrWhitespace(const std::string& str) {
    return str.find_first_not_of(" \t\n\r\f\v") == std::string::npos;
}

std::string UrlEncode(const std::string& str) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;
    
    for (char c : str) {
        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }
        
        // Any other characters are percent-encoded
        escaped << '%' << std::setw(2) << int((unsigned char)c);
    }
    
    return escaped.str();
}

std::string UrlDecode(const std::string& str) {
    std::ostringstream decoded;
    
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '%') {
            if (i + 2 >= str.size()) {
                decoded << str[i];
                continue;
            }
            
            std::string hex = str.substr(i + 1, 2);
            char decoded_char = static_cast<char>(std::stoi(hex, nullptr, 16));
            decoded << decoded_char;
            i += 2;
        } else if (str[i] == '+') {
            decoded << ' ';
        } else {
            decoded << str[i];
        }
    }
    
    return decoded.str();
}

// Base64 encoding table
static const std::string base64_chars = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

std::string Base64Encode(const std::string& str) {
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];
    
    for (auto c : str) {
        char_array_3[i++] = c;
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            
            for (i = 0; i < 4; i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }
    
    if (i) {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';
        
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;
        
        for (j = 0; j < i + 1; j++)
            ret += base64_chars[char_array_4[j]];
        
        while (i++ < 3)
            ret += '=';
    }
    
    return ret;
}

std::string Base64Decode(const std::string& str) {
    std::string ret;
    int i = 0;
    int j = 0;
    int in_len = str.size();
    unsigned char char_array_4[4], char_array_3[3];
    
    while (in_len-- && (str[j] != '=')) {
        char_array_4[i++] = str[j];
        j++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);
            
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            
            for (i = 0; i < 3; i++)
                ret += char_array_3[i];
            i = 0;
        }
    }
    
    if (i) {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;
        
        for (j = 0; j < 4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);
        
        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
        
        for (j = 0; j < i - 1; j++)
            ret += char_array_3[j];
    }
    
    return ret;
}

std::string HexEncode(const std::string& str) {
    std::ostringstream hex;
    hex << std::hex << std::setfill('0');
    
    for (unsigned char c : str) {
        hex << std::setw(2) << static_cast<int>(c);
    }
    
    return hex.str();
}

std::string HexDecode(const std::string& str) {
    std::string result;
    
    for (size_t i = 0; i < str.length(); i += 2) {
        std::string byte_string = str.substr(i, 2);
        char byte = static_cast<char>(std::stoi(byte_string, nullptr, 16));
        result.push_back(byte);
    }
    
    return result;
}

std::string StringFormat(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    // Determine the required size
    va_list args_copy;
    va_copy(args_copy, args);
    int size = vsnprintf(nullptr, 0, format, args_copy) + 1;
    va_end(args_copy);
    
    // Allocate buffer and format
    std::vector<char> buffer(size);
    vsnprintf(buffer.data(), size, format, args);
    va_end(args);
    
    return std::string(buffer.data());
}

bool IsValidUrl(const std::string& url) {
    if (url.empty()) return false;
    
    // Check for valid scheme
    size_t scheme_end = url.find("://");
    if (scheme_end == std::string::npos) {
        // Allow relative URLs and about: schemes
        if (url.find("about:") == 0 || url.find("data:") == 0 || 
            url.find("javascript:") == 0 || url.find("file:") == 0) {
            return true;
        }
        return false;
    }
    
    std::string scheme = url.substr(0, scheme_end);
    if (scheme != "http" && scheme != "https" && scheme != "ftp" && 
        scheme != "file" && scheme != "data" && scheme != "about") {
        return false;
    }
    
    // Check for valid hostname
    size_t host_start = scheme_end + 3;
    size_t host_end = url.find_first_of(":/?#", host_start);
    
    if (host_end == std::string::npos) {
        host_end = url.length();
    }
    
    std::string host = url.substr(host_start, host_end - host_start);
    if (host.empty()) return false;
    
    return true;
}

std::string ExtractDomain(const std::string& url) {
    if (url.empty()) return "";
    
    // Remove scheme
    size_t scheme_end = url.find("://");
    if (scheme_end != std::string::npos) {
        scheme_end += 3;
    } else {
        scheme_end = 0;
    }
    
    // Find end of domain
    size_t domain_end = url.find_first_of(":/?#", scheme_end);
    if (domain_end == std::string::npos) {
        domain_end = url.length();
    }
    
    std::string domain = url.substr(scheme_end, domain_end - scheme_end);
    
    // Remove port if present
    size_t port_pos = domain.find(':');
    if (port_pos != std::string::npos) {
        domain = domain.substr(0, port_pos);
    }
    
    return domain;
}

std::string ExtractPath(const std::string& url) {
    if (url.empty()) return "/";
    
    size_t scheme_end = url.find("://");
    if (scheme_end == std::string::npos) {
        scheme_end = 0;
    } else {
        scheme_end += 3;
    }
    
    size_t path_start = url.find('/', scheme_end);
    if (path_start == std::string::npos) {
        return "/";
    }
    
    size_t path_end = url.find_first_of("?#", path_start);
    if (path_end == std::string::npos) {
        path_end = url.length();
    }
    
    std::string path = url.substr(path_start, path_end - path_start);
    return path.empty() ? "/" : path;
}

std::string ExtractQuery(const std::string& url) {
    size_t query_start = url.find('?');
    if (query_start == std::string::npos) {
        return "";
    }
    
    size_t query_end = url.find('#', query_start);
    if (query_end == std::string::npos) {
        query_end = url.length();
    }
    
    return url.substr(query_start + 1, query_end - query_start - 1);
}

std::vector<std::pair<std::string, std::string>> ParseQueryParameters(const std::string& query) {
    std::vector<std::pair<std::string, std::string>> params;
    
    if (query.empty()) return params;
    
    size_t pos = 0;
    while (pos < query.length()) {
        size_t amp_pos = query.find('&', pos);
        if (amp_pos == std::string::npos) {
            amp_pos = query.length();
        }
        
        std::string pair = query.substr(pos, amp_pos - pos);
        size_t eq_pos = pair.find('=');
        
        if (eq_pos != std::string::npos) {
            std::string key = UrlDecode(pair.substr(0, eq_pos));
            std::string value = UrlDecode(pair.substr(eq_pos + 1));
            params.emplace_back(key, value);
        } else {
            params.emplace_back(UrlDecode(pair), "");
        }
        
        pos = amp_pos + 1;
    }
    
    return params;
}

bool IsValidHostname(const std::string& hostname) {
    if (hostname.empty() || hostname.length() > 253) return false;
    
    // Check each label
    std::vector<std::string> labels = SplitString(hostname, '.');
    if (labels.empty()) return false;
    
    for (const auto& label : labels) {
        if (label.empty() || label.length() > 63) return false;
        
        // Check for valid characters
        for (char c : label) {
            if (!isalnum(c) && c != '-') return false;
        }
        
        // Check for leading/trailing hyphens
        if (label.front() == '-' || label.back() == '-') return false;
    }
    
    return true;
}

bool IsValidIpAddress(const std::string& ip) {
    // Simple IPv4 check
    std::vector<std::string> parts = SplitString(ip, '.');
    if (parts.size() != 4) return false;
    
    for (const auto& part : parts) {
        if (part.empty() || part.length() > 3) return false;
        
        for (char c : part) {
            if (!isdigit(c)) return false;
        }
        
        int num = std::stoi(part);
        if (num < 0 || num > 255) return false;
    }
    
    return true;
}

bool IsValidEmail(const std::string& email) {
    if (email.empty()) return false;
    
    size_t at_pos = email.find('@');
    if (at_pos == std::string::npos || at_pos == 0 || at_pos == email.length() - 1) {
        return false;
    }
    
    std::string local = email.substr(0, at_pos);
    std::string domain = email.substr(at_pos + 1);
    
    if (local.empty() || domain.empty()) return false;
    
    // Check for valid domain
    if (domain.find('.') == std::string::npos) return false;
    
    return true;
}

std::string GenerateRandomString(size_t length) {
    static const char alphanum[] = 
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);
    
    std::string result;
    result.reserve(length);
    
    for (size_t i = 0; i < length; ++i) {
        result += alphanum[dis(gen)];
    }
    
    return result;
}

std::string GenerateUUID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    // Generate 8-4-4-4-12 format
    for (int i = 0; i < 8; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 4; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 4; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 4; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 12; i++) ss << dis(gen);
    
    return ss.str();
}

std::string MD5Hash(const std::string& str) {
#ifdef NULLWARE_PLATFORM_WINDOWS
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    BYTE rgbHash[16];
    DWORD cbHash = 0;
    
    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        return "";
    }
    
    if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) {
        CryptReleaseContext(hProv, 0);
        return "";
    }
    
    if (!CryptHashData(hHash, (BYTE*)str.c_str(), str.length(), 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return "";
    }
    
    cbHash = 16;
    if (!CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return "";
    }
    
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    
    std::ostringstream result;
    result << std::hex << std::setfill('0');
    for (DWORD i = 0; i < cbHash; i++) {
        result << std::setw(2) << (int)rgbHash[i];
    }
    
    return result.str();
#else
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5((const unsigned char*)str.c_str(), str.length(), digest);
    
    std::ostringstream result;
    result << std::hex << std::setfill('0');
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        result << std::setw(2) << (int)digest[i];
    }
    
    return result.str();
#endif
}

std::string SHA1Hash(const std::string& str) {
#ifdef NULLWARE_PLATFORM_WINDOWS
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    BYTE rgbHash[20];
    DWORD cbHash = 0;
    
    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        return "";
    }
    
    if (!CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash)) {
        CryptReleaseContext(hProv, 0);
        return "";
    }
    
    if (!CryptHashData(hHash, (BYTE*)str.c_str(), str.length(), 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return "";
    }
    
    cbHash = 20;
    if (!CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return "";
    }
    
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    
    std::ostringstream result;
    result << std::hex << std::setfill('0');
    for (DWORD i = 0; i < cbHash; i++) {
        result << std::setw(2) << (int)rgbHash[i];
    }
    
    return result.str();
#else
    unsigned char digest[SHA_DIGEST_LENGTH];
    SHA1((const unsigned char*)str.c_str(), str.length(), digest);
    
    std::ostringstream result;
    result << std::hex << std::setfill('0');
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        result << std::setw(2) << (int)digest[i];
    }
    
    return result.str();
#endif
}

std::string SHA256Hash(const std::string& str) {
#ifdef NULLWARE_PLATFORM_WINDOWS
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    BYTE rgbHash[32];
    DWORD cbHash = 0;
    
    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        return "";
    }
    
    // Use SHA-256 algorithm
    ALG_ID alg_id = CALG_SHA_256;
    
    if (!CryptCreateHash(hProv, alg_id, 0, 0, &hHash)) {
        CryptReleaseContext(hProv, 0);
        return "";
    }
    
    if (!CryptHashData(hHash, (BYTE*)str.c_str(), str.length(), 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return "";
    }
    
    cbHash = 32;
    if (!CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return "";
    }
    
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    
    std::ostringstream result;
    result << std::hex << std::setfill('0');
    for (DWORD i = 0; i < cbHash; i++) {
        result << std::setw(2) << (int)rgbHash[i];
    }
    
    return result.str();
#else
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)str.c_str(), str.length(), digest);
    
    std::ostringstream result;
    result << std::hex << std::setfill('0');
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        result << std::setw(2) << (int)digest[i];
    }
    
    return result.str();
#endif
}

bool EqualsIgnoreCase(const std::string& a, const std::string& b) {
    if (a.length() != b.length()) return false;
    
    for (size_t i = 0; i < a.length(); ++i) {
        if (tolower(a[i]) != tolower(b[i])) return false;
    }
    
    return true;
}

bool WildcardMatch(const std::string& str, const std::string& pattern) {
    size_t str_pos = 0;
    size_t pattern_pos = 0;
    size_t str_len = str.length();
    size_t pattern_len = pattern.length();
    
    while (str_pos < str_len && pattern_pos < pattern_len) {
        if (pattern[pattern_pos] == '*') {
            // Skip consecutive '*' characters
            while (pattern_pos < pattern_len && pattern[pattern_pos] == '*') {
                pattern_pos++;
            }
            
            if (pattern_pos == pattern_len) {
                return true;  // Pattern ends with '*', match remaining string
            }
            
            // Try to match the rest of the pattern
            while (str_pos < str_len) {
                if (WildcardMatch(str.substr(str_pos), pattern.substr(pattern_pos))) {
                    return true;
                }
                str_pos++;
            }
            
            return false;
        } else if (pattern[pattern_pos] == '?') {
            // '?' matches any single character
            str_pos++;
            pattern_pos++;
        } else if (pattern[pattern_pos] == str[str_pos]) {
            str_pos++;
            pattern_pos++;
        } else {
            return false;
        }
    }
    
    return str_pos == str_len && pattern_pos == pattern_len;
}

bool MatchesAny(const std::string& str, const std::vector<std::string>& patterns) {
    for (const auto& pattern : patterns) {
        if (WildcardMatch(str, pattern)) {
            return true;
        }
    }
    return false;
}

}  // namespace nullware
