#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <condition_variable>
#include <fstream>
#include "parser.h"
#include "tools.h"

const int BUFFSIZE = 8192;
const int LEN = 10;

class Semaphore
{
public:
    Semaphore(long count = 0) : count(count) {}
    //V操作，唤醒
    void signal()
    {
        std::unique_lock<std::mutex> unique(mt);
        ++count;
        if (count <= 0)
            cond.notify_one();
    }
    //P操作，阻塞
    void wait()
    {
        std::unique_lock<std::mutex> unique(mt);
        --count;
        if (count < 0)
            cond.wait(unique);
    }
private:
    std::mutex mt;
    std::condition_variable cond;
    long count;
};

class CBuffer
{
private:
    struct
    {
        char* data;
        int readbytes;
    } buffer[LEN];

    void load();
    void read();
    std::string filename;
    Parser* parser;
    void (Parser::* func)(char*) = &Parser::process;
    Semaphore* empty;
    Semaphore* store;

public:
    CBuffer(std::string, Parser);
    ~CBuffer();
    void run();
};

CBuffer::CBuffer(std::string file, Parser p) :filename(file), parser(&p)
{
    for (int i = 0; i < LEN; i++)
    {
        buffer[i].data = new char[BUFFSIZE];
        buffer[i].readbytes = 0;
    }
    this->empty = new Semaphore(LEN);
    this->store = new Semaphore(0);
}
CBuffer::~CBuffer() {}

void CBuffer::load()
{
    int i = 0;
    errno_t err;
    size_t loaded = 0, ret = 0;
    FILE* fp;
    if ((err = fopen_s(&fp, filename.c_str(), "r")) != 0)
    {
        std::cout << "[Cbuffer.load] Cant open file." << std::endl;
        store->signal();
        return;
    }
    while (true)
    {
        empty->wait();
        loaded = fread(buffer[i].data, sizeof(char), BUFFSIZE, fp);
        std::cout << "[Cbuffer.load]" << i << "th round load " << loaded << " bytes." << std::endl;
        buffer[i].readbytes = (int)loaded;
        // 出错情况
        if (loaded == 0)
        {
            store->signal();
            break;
        }
        i = (i + 1) % LEN;
        store->signal();
    }
}

void CBuffer::read()
{
    int wordcount = 0;
    int i = 0;
    while (true)
    {
        store->wait();
        if (buffer[i].readbytes == 0) { break; }
        // TODO:Process buffer data by char.
        std::cout << "[Cbuffer.read]" << i << "th round process " << buffer[i].readbytes << " bytes." << std::endl;

        for (int j = 0; j < buffer[i].readbytes; j++)
        {
            (parser->*func)(&(buffer[i].data[j]));
        }
        i = (i + 1) % LEN;
        empty->signal();
    }
}

void CBuffer::run()
{
    int param = 0;
    std::thread load_thread(&CBuffer::load, this);
    std::thread process_thread(&CBuffer::read, this);
    load_thread.join();
    process_thread.join();
}