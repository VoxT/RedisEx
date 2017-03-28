/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ConfigReader.cpp
 * Author: haint3
 * 
 * Created on February 23, 2016, 2:36 PM
 */

#include "ZConfigReader.h"
#include "inireader/INIReader.h"
#include <iostream>
#include <regex>

ZConfigReader::ZConfigReader()
{
    readConfig(CONFIG_PATH);
}

ZConfigReader::~ZConfigReader()
{
}

bool ZConfigReader::readConfig(const std::string& path)
{
    INIReader reader(path);
    if (reader.ParseError() < 0) {
        std::cout << "read config error: " << reader.ParseError() << std::endl;
        return false;
    }

    m_port = reader.GetInteger("server_config", "port", 80);

    m_timeWaitSendMsg = reader.GetInteger("config", "wait_time_send_msg", 500);
    m_timeWaitSendMsg *= 1000; // to microsecond
    
    m_timeWaitResendMsg = reader.GetInteger("config", "wait_time_resend_msg", 3000);
    m_timeWaitResendMsg *= 1000; // to microsecond
    
    m_maxSendTimes = reader.GetInteger("config", "max_send_times", 3);

    m_nsMtupUri = reader.Get("config", "urlnsmtup", "");
    
    m_maxUIDsSize = reader.GetInteger("config", "maxuidssize", 100);

    int nNumRedisCluster = reader.GetInteger("redis", "num_redis", 1);
    if (nNumRedisCluster < 1)
        nNumRedisCluster = 1;
    std::string redisSection = "";
    for (int i = 0; i < nNumRedisCluster; i++) {
        redisSection = std::string("redis_") + std::to_string(i + 1);
        REDISINFO redisInfo;
        redisInfo.host = reader.Get(redisSection, "host", "");
        redisInfo.port = reader.GetInteger(redisSection, "port", 0);
        if (!CheckRedisInfo(&redisInfo))
            continue;
        m_vtListRedisCluster.push_back(redisInfo);
    }

    return true;
}

void ZConfigReader::extractInteger(const std::string& str, std::vector<int>& vtInt)
{
    stringstream ss(str);
    int iter;
    char ch;

    while (ss >> iter) {
        ss >> ch; //step one character
        vtInt.push_back(iter);
    }
}

bool ZConfigReader::CheckRedisInfo(LPREDISINFO lpRdsInfo)
{
    if (!lpRdsInfo)
        return false;

    if ((lpRdsInfo->port <= 0) || lpRdsInfo->host.empty())
        return false;

    if (m_vtListRedisCluster.size() == 0)
        return true;

    for (int i = 0; i < m_vtListRedisCluster.size(); i++) {
        if ((lpRdsInfo->host.compare(m_vtListRedisCluster[i].host) == 0)
            && (lpRdsInfo->port == m_vtListRedisCluster[i].port))
            return false;
    }

    return true;
}