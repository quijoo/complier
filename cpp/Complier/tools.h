
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
/// �ַ����ָ�
/// </summary>
/// <param name="in">�����ַ���</param>
/// <param name="pattern">�ָ��־</param>
/// <returns>�ָ����Ӵ�����(�����մ�)</returns>
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
            //��ʼ�ָ�
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
/// ȥ���ַ����е�\r\n
/// </summary>
/// <param name="in">�����ַ���</param>
/// <returns>����ַ���</returns>
std::string do_strip(const std::string& str, int striptype, const std::string& chars)
{
    std::string::size_type strlen = str.size();
    std::string::size_type charslen = chars.size();
    std::string::size_type i, j;

    //Ĭ������£�ȥ���հ׷�
    if (0 == charslen)
    {
        i = 0;
        //ȥ����߿հ��ַ�
        if (striptype != RIGHTSTRIP)
        {
            while (i < strlen && ::isspace(str[i]))
            {
                i++;
            }
        }
        j = strlen;
        //ȥ���ұ߿հ��ַ�
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
        //��ɾ������תΪc�ַ���
        const char* sep = chars.c_str();
        i = 0;
        if (striptype != RIGHTSTRIP)
        {
            //memchr��������sepָ����ڴ������ǰcharslen���ֽڲ���str[i]
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
        //�������Ҫɾ�����ַ�
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