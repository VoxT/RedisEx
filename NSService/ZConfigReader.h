/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ConfigReader.h
 * Author: haint3
 *
 * Created on February 23, 2016, 2:36 PM
 */

#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include <string>
#include <vector>

#define CONFIG_PATH         "config.ini"

using namespace std;

typedef struct tagRedisInfo
{
    std::string host;
    int port;

    tagRedisInfo()
    {
        reset();
    }

    void reset()
    {
        host = "";
        port = 0;
    }
} REDISINFO, *LPREDISINFO;

class ZConfigReader
{
public:

    static ZConfigReader& instance()
    {
        static ZConfigReader myInstance;
        return myInstance;
    }

    virtual ~ZConfigReader();
    bool readConfig(const std::string& path);

private:
    ZConfigReader();
    void extractInteger(const std::string& str, std::vector<int>& vtInt);
    bool CheckRedisInfo(LPREDISINFO lpRdsInfo);
public:
    std::vector<REDISINFO> m_vtListRedisCluster;
    uint16_t m_port;

    uint64_t m_timeWaitSendMsg; // microsecond
    uint64_t m_timeWaitResendMsg; // microsecond
    uint64_t m_maxSendTimes;
    uint32_t m_maxUIDsSize;
    std::string m_nsMtupUri;
};

#endif /* CONFIGREADER_H */

