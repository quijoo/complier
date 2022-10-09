#pragma once
#include <iostream>
class Parser
{
private:
    /* data */
public:
    Parser(/* args */);
    ~Parser();
    void process(char*);
};

Parser::Parser(/* args */){}

Parser::~Parser(){}

void Parser::process(char* chr)
{
    // 词法分析状态机的解析方法
    // 解析出一个词素后向缓存提交，缓存加锁，待语法分析单元调用
    std::cout << chr << std::endl;
    if (*chr == EOF)
    {
        std::cout << "touch eof." << std::endl;
    }
}