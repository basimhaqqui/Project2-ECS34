#include "StringUtils.h"
#include <algorithm>
#include <cctype>

namespace StringUtils {

std::string Slice(const std::string &str, ssize_t start, ssize_t end) noexcept {
    if(str.empty()) {
        return str;
    }
    
    // Convert negative indices to positive
    ssize_t length = static_cast<ssize_t>(str.length());
    if(start < 0) {
        start = length + start;
    }
    if(end <= 0) {
        end = length + end;
    }
    
    // Bounds checking
    if(start >= length || end > length || start >= end) {
        return "";
    }
    start = std::max(static_cast<ssize_t>(0), start);
    
    return str.substr(start, end - start);
}

std::string Capitalize(const std::string &str) noexcept {

    //return the string if its empty

    if (str.empty()){

    return str;

    } 

    //storing input string here

    std::string result = str;

    //capatilize the first character

    result[0] = std::toupper(result[0]);

    //i created a for loop here based on size of result to make sure everything

    //else is lower case

    for (size_t i = 1; i < result.size(); ++i) {

        result[i] = std::tolower(result[i]);

    }

    //return all characters

    return result;

}

std::string Upper(const std::string &str) noexcept {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string Lower(const std::string &str) noexcept {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string LStrip(const std::string &str) noexcept {
    if(str.empty()) {
        return str;
    }
    auto start = str.begin();
    while(start != str.end() && std::isspace(*start)) {
        ++start;
    }
    return std::string(start, str.end());
}

std::string RStrip(const std::string &str) noexcept {
    if(str.empty()) {
        return str;
    }
    auto end = str.rbegin();
    while(end != str.rend() && std::isspace(*end)) {
        ++end;
    }
    return std::string(str.begin(), end.base());
}

std::string Strip(const std::string &str) noexcept {
    return LStrip(RStrip(str));
}

std::string Center(const std::string &str, int width, char fill) noexcept {
    if(width <= static_cast<int>(str.length())) {
        return str;
    }
    
    int padding = width - static_cast<int>(str.length());
    int left_padding = padding / 2;
    int right_padding = padding - left_padding;
    
    return std::string(left_padding, fill) + str + std::string(right_padding, fill);
}

std::string LJust(const std::string &str, int width, char fill) noexcept {
    if(width <= static_cast<int>(str.length())) {
        return str;
    }
    return str + std::string(width - str.length(), fill);
}

std::string RJust(const std::string &str, int width, char fill) noexcept {
    if(width <= static_cast<int>(str.length())) {
        return str;
    }
    return std::string(width - str.length(), fill) + str;
}

std::string Replace(const std::string &str, const std::string &old, const std::string &rep) noexcept {
    if(str.empty() || old.empty()) {
        return str;
    }
    
    std::string result = str;
    size_t pos = 0;
    while((pos = result.find(old, pos)) != std::string::npos) {
        result.replace(pos, old.length(), rep);
        pos += rep.length();
    }
    return result;
}

std::vector<std::string> Split(const std::string &str, const std::string &splt) noexcept {
    std::vector<std::string> result;
    if(str.empty()) {
        result.push_back(str);
        return result;
    }
    
    if(splt.empty()) {
        // Split on whitespace
        std::string curr;
        for(char ch : str) {
            if(std::isspace(ch)) {
                if(!curr.empty()) {
                    result.push_back(curr);
                    curr.clear();
                }
            }
            else {
                curr += ch;
            }
        }
        if(!curr.empty()) {
            result.push_back(curr);
        }
    }
    else {
        size_t start = 0;
        size_t end = str.find(splt);
        while(end != std::string::npos) {
            result.push_back(str.substr(start, end - start));
            start = end + splt.length();
            end = str.find(splt, start);
        }
        result.push_back(str.substr(start));
    }
    return result;
}

std::string Join(const std::string &str, const std::vector<std::string> &vect) noexcept {
    if(vect.empty()) {
        return "";
    }
    
    std::string result = vect[0];
    for(size_t i = 1; i < vect.size(); ++i) {
        result += str + vect[i];
    }
    return result;
}

std::string ExpandTabs(const std::string &str, int tabsize) noexcept {
    if (tabsize <= 0) {
        // Treat tabsize 0 or negative as removing all tabs
        std::string result;
        result.reserve(str.size());
        for (char ch : str) {
            if (ch != '\t') {
                result += ch;
            }
        }
        return result;
    }
    
    if(str.empty()) {
        return str;
    }
    
    std::string result;
    size_t col = 0;

    for(char ch : str) {
        if(ch == '\t') {
            // Calculate how many spaces needed to get to next tab stop
            size_t spaces = tabsize - (col % tabsize);
            result.append(spaces, ' ');
            col += spaces;
        }
        else if(ch == '\n' || ch == '\r') {
            result += ch;
            col = 0;  // Reset column count on newline
        }
        else {
            result += ch;
            col++;
        }
    }
    return result;
}

int EditDistance(const std::string &left, const std::string &right, bool ignorecase) noexcept {
    std::string str1 = ignorecase ? Lower(left) : left;
    std::string str2 = ignorecase ? Lower(right) : right;
    
    int m = str1.length();
    int n = str2.length();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));
    
    for(int i = 0; i <= m; ++i) {
        dp[i][0] = i;
    }
    
    for(int j = 0; j <= n; ++j) {
        dp[0][j] = j;
    }
    
    for(int i = 1; i <= m; ++i) {
        for(int j = 1; j <= n; ++j) {
            if(str1[i-1] == str2[j-1]) {
                dp[i][j] = dp[i-1][j-1];
            }
            else {
                dp[i][j] = 1 + std::min({dp[i-1][j],    // deletion
                                       dp[i][j-1],    // insertion
                                       dp[i-1][j-1]}); // substitution
            }
        }
    }
    
    return dp[m][n];
}

} // namespace StringUtils