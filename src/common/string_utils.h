#ifndef NULLWARE_COMMON_STRING_UTILS_H_
#define NULLWARE_COMMON_STRING_UTILS_H_

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>

namespace nullware {

// String manipulation utilities

// Trim whitespace from both ends of a string
std::string TrimString(const std::string& str);

// Trim whitespace from the left end of a string
std::string TrimStringLeft(const std::string& str);

// Trim whitespace from the right end of a string
std::string TrimStringRight(const std::string& str);

// Convert string to lowercase
std::string ToLower(const std::string& str);

// Convert string to uppercase
std::string ToUpper(const std::string& str);

// Check if string starts with prefix
bool StartsWith(const std::string& str, const std::string& prefix);

// Check if string ends with suffix
bool EndsWith(const std::string& str, const std::string& suffix);

// Check if string contains substring
bool Contains(const std::string& str, const std::string& substr);

// Replace all occurrences of a substring with another substring
std::string ReplaceAll(const std::string& str, const std::string& from, const std::string& to);

// Split string by delimiter
std::vector<std::string> SplitString(const std::string& str, char delimiter);
std::vector<std::string> SplitString(const std::string& str, const std::string& delimiter);

// Join strings with delimiter
std::string JoinString(const std::vector<std::string>& parts, const std::string& delimiter);

// Check if string is empty or whitespace only
bool IsStringEmptyOrWhitespace(const std::string& str);

// URL encoding/decoding
std::string UrlEncode(const std::string& str);
std::string UrlDecode(const std::string& str);

// Base64 encoding/decoding
std::string Base64Encode(const std::string& str);
std::string Base64Decode(const std::string& str);

// Hex encoding/decoding
std::string HexEncode(const std::string& str);
std::string HexDecode(const std::string& str);

// String formatting (similar to sprintf)
std::string StringFormat(const char* format, ...);

// Check if string is a valid URL
bool IsValidUrl(const std::string& url);

// Extract domain from URL
std::string ExtractDomain(const std::string& url);

// Extract path from URL
std::string ExtractPath(const std::string& url);

// Extract query parameters from URL
std::string ExtractQuery(const std::string& url);

// Parse query parameters into key-value pairs
std::vector<std::pair<std::string, std::string>> ParseQueryParameters(const std::string& query);

// Check if string is a valid hostname
bool IsValidHostname(const std::string& hostname);

// Check if string is a valid IP address
bool IsValidIpAddress(const std::string& ip);

// Check if string is a valid email address
bool IsValidEmail(const std::string& email);

// Generate a random string of given length
std::string GenerateRandomString(size_t length);

// Generate a UUID string
std::string GenerateUUID();

// Hash a string using various algorithms
std::string MD5Hash(const std::string& str);
std::string SHA1Hash(const std::string& str);
std::string SHA256Hash(const std::string& str);

// Compare strings case-insensitively
bool EqualsIgnoreCase(const std::string& a, const std::string& b);

// Wildcard pattern matching
bool WildcardMatch(const std::string& str, const std::string& pattern);

// Check if string matches any of the patterns
bool MatchesAny(const std::string& str, const std::vector<std::string>& patterns);

}  // namespace nullware

#endif  // NULLWARE_COMMON_STRING_UTILS_H_
