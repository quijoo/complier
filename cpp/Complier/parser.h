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
    // �ʷ�����״̬���Ľ�������
    // ������һ�����غ��򻺴��ύ��������������﷨������Ԫ����
    std::cout << chr << std::endl;
    if (*chr == EOF)
    {
        std::cout << "touch eof." << std::endl;
    }
}