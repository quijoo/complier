
#pragma once

#define LEFTSTRIP 0
#define RIGHTSTRIP 1
#define BOTHSTRIP 2

#define TRACE trace(__FUNCTION__)

#include <string>
#include <vector>

class trace
{
public:
    trace(std::string s)
    {
        name = s;
        std::cout << "[enter function]:<" << name << ">" << std::endl;

    };
    ~trace()
    {
        std::cout << "[leave function]:<" << name << ">" << std::endl;
    };
private:
    std::string name;
};


int* kmp_next(std::string pattern)
{
    int* next = new int[pattern.length()];
    next[0] = -1;
    int j = 0, k = -1;
    while (j < (int)pattern.length() - 1)
    {
        if (k == -1 || pattern[j] == pattern[k])
        {
            j++; k++;
            next[j] = k;
        }
        else
        {
            k = next[k];
        }
    }
    return next;
}

int kmp_match(std::string pattern, std::string s_in)
{
    if (pattern == "" || s_in == "" || s_in.length() < 1 || s_in.length() < pattern.length())
    {
        return -1;
    }
    int j = 0, i = 0;
    int* next = kmp_next(pattern);
    while (i < (int)pattern.length() && j < (int)s_in.length())
    {
        if (i == -1 || pattern[i] == s_in[j])
        {
            i++; j++;
        }
        else
        {
            i = next[i];
        }
    }
    if (i == (int)pattern.length())
    {
        return j - i;
    }
    return -1;
}

/// <summary>
/// 字符串分割
/// </summary>
/// <param name="in">输入字符串</param>
/// <param name="pattern">分割标志</param>
/// <returns>分割后的子串链表(保留空串)</returns>
std::vector<std::string> split(std::string in, const std::string &pattern)
{
    std::string tmp = "";
    int pattern_pos = 0;
    std::vector<std::string> splited;
    for (int i = 0; i < in.size(); i++)
    {
        tmp += in[i];
        if (in[i] == pattern[pattern_pos]) pattern_pos += 1;
        else pattern_pos = 0;

        if (pattern_pos == pattern.size())
        {
            //开始分割
            for (int k = 0; k < pattern.size(); k++)
                tmp.pop_back();
            splited.push_back(tmp.substr());
            tmp = "";
            pattern_pos = 0;
        }
    }
    splited.push_back(tmp);
    return splited;
}

/// <summary>
/// 去除字符串中的\r\n
/// </summary>
/// <param name="in">输入字符串</param>
/// <returns>输出字符串</returns>
std::string do_strip(const std::string& str, int striptype, const std::string& chars)
{
    std::string::size_type strlen = str.size();
    std::string::size_type charslen = chars.size();
    std::string::size_type i, j;

    //默认情况下，去除空白符
    if (0 == charslen)
    {
        i = 0;
        //去掉左边空白字符
        if (striptype != RIGHTSTRIP)
        {
            while (i < strlen && ::isspace(str[i]))
            {
                i++;
            }
        }
        j = strlen;
        //去掉右边空白字符
        if (striptype != LEFTSTRIP)
        {
            j--;
            while (j >= i && ::isspace(str[j]))
            {
                j--;
            }
            j++;
        }
    }
    else
    {
        //把删除序列转为c字符串
        const char* sep = chars.c_str();
        i = 0;
        if (striptype != RIGHTSTRIP)
        {
            //memchr函数：从sep指向的内存区域的前charslen个字节查找str[i]
            while (i < strlen && memchr(sep, str[i], charslen))
            {
                i++;
            }
        }
        j = strlen;
        if (striptype != LEFTSTRIP)
        {
            j--;
            while (j >= i && memchr(sep, str[j], charslen))
            {
                j--;
            }
            j++;
        }
        //如果无需要删除的字符
        if (0 == i && j == strlen)
        {
            return str;
        }
        else
        {
            return str.substr(i, j - i);
        }
    }
}
std::string strip(const std::string& str, const std::string& chars = " ")
{
    return do_strip(str, BOTHSTRIP, chars);
}

std::string lstrip(const std::string& str, const std::string& chars = " ")
{
    return do_strip(str, LEFTSTRIP, chars);
}

std::string rstrip(const std::string& str, const std::string& chars = " ")
{
    return do_strip(str, RIGHTSTRIP, chars);
}