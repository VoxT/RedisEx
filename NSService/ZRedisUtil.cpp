/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ZRedisUtil.cpp
 * Author: Thieu Vo
 * 
 * Created on March 22, 2017, 9:43 AM
 */

#include <set>

#include "ZRedisUtil.h"

ZRedisUtil& ZRedisUtil::GetInstance()
{
    static ZRedisUtil zRedis;
    return zRedis;
}

bool ZRedisUtil::Init(const std::string& strHost, uint32_t port)
{
    if (strHost.empty())
        return false;
    
    if (!m_zCluster.Init(strHost, port))
    {
        std::cerr << "Connect to Redis failed";
        return false;
    }
    
    return true;
}

bool ZRedisUtil::SaveInfo(uint64_t uSenderId, uint64_t uUserId,\
                           const std::string& strMsgData, bool bProcessedMsgResult,\
                           uint64_t uReqTime, uint64_t uSendTime)
{
    if (strMsgData.empty() || (uReqTime > uSendTime))
        return false;
   
    uint64_t uMsgId = HSetMsg(uSenderId, uUserId, strMsgData, bProcessedMsgResult, uReqTime, uSendTime);
    if (uMsgId == 0)
    { 
        std::cout << "set msg failed" << std::endl;
        return false;
    }
    
    std::string strMsgId = Poco::NumberFormatter::format(uMsgId);
    
    // set sender
    if (m_zCluster.SAdd(RDS_NS_SENDERS, Poco::NumberFormatter::format(uSenderId)) == -1)
    { 
        std::cout << "set senders failed" << std::endl;
        return false;
    }
    // set user
    if (m_zCluster.SAdd(RDS_NS_USERS, Poco::NumberFormatter::format(uUserId)) == -1)
    { 
        std::cout << "set users failed" << std::endl;
        return false;
    }
    
    // zset msg list of sender
    if (m_zCluster.ZAdd(GetSenderMsgListKey(uSenderId), uReqTime, strMsgId) == -1)
    { 
        std::cout << "set sender msg list failed" << std::endl;
        return false;
    }
    // zset msg list of user
    if (m_zCluster.ZAdd(GetUserMsgListKey(uUserId), uSendTime, strMsgId) == -1)
    { 
        std::cout << "set user msg list failed" << std::endl;
        return false;
    }
    
    // request counter
    if (!HSetRequestCounter(bProcessedMsgResult))
    { 
        std::cout << "HSetRequestCounter failed" << std::endl;
        return false;
    }
    
    // min, max, average processed time
    if (!HSetProcessedTime(uSendTime - uReqTime))
    { 
        std::cout << "HSetProcessedTime failed" << std::endl;
        return false;
    }
    
    return true;
}

uint64_t ZRedisUtil::HSetMsg(uint64_t uSenderId, uint64_t uUserId,\
                            const std::string& strMsgData, bool bProcessedMsgResult,\
                            uint64_t uReqTime, uint64_t uSendTime)
{
    uint64_t uRet = 0;
    if (strMsgData.empty())
        return uRet;
    
    // get msgId
    uint64_t uMsgIdIncr = m_zCluster.Incr(RDS_NS_MSG_ID_INCR);
    if (uMsgIdIncr == 0)
        return uRet;
    
    // hash msg
    std::string strHash = GetMsgKey(uMsgIdIncr);
    if (strHash.empty())
        return uRet;
    
    if (m_zCluster.HSet(strHash, RDS_NS_MSG_INFO_FIELD_SENDER_ID, Poco::NumberFormatter::format(uSenderId)) == -1)
        return uRet;
    if (m_zCluster.HSet(strHash, RDS_NS_MSG_INFO_FIELD_USER_ID, Poco::NumberFormatter::format(uUserId)) == -1)
        return uRet;
    if (m_zCluster.HSet(strHash, RDS_NS_MSG_INFO_FIELD_DATA, strMsgData) == -1)
        return uRet;
    if (m_zCluster.HSet(strHash, RDS_NS_MSG_INFO_FILED_PROCESSED_RESULT, (bProcessedMsgResult?"1":"0")) == -1)
        return uRet;
    if (m_zCluster.HSet(strHash, RDS_NS_MSG_INFO_FIELD_REQUEST_TIME, Poco::NumberFormatter::format(uReqTime)) == -1)
        return uRet;
    if (m_zCluster.HSet(strHash, RDS_NS_MSG_INFO_FIELD_SEND_TIME, Poco::NumberFormatter::format(uSendTime)) == -1)
        return uRet;
    
    uRet = uMsgIdIncr;
    
    return uRet;
}

bool ZRedisUtil::HSetProcessedTime(uint64_t uPTime)
{
    // processed time
    uint64_t uIsExists = m_zCluster.Exists(RDS_NS_PROCESSED_TIME);
    if (uIsExists == -1)
        return false;
    
    std::string strPTime = Poco::NumberFormatter::format(uPTime);
    
    // Set hash key if key isn't exists
    if (uIsExists == 0)
    {
        if (m_zCluster.HSet(RDS_NS_PROCESSED_TIME, RDS_NS_PROCESSED_TIME_FIELD_MAX, strPTime) == -1)
            return false;
        if (m_zCluster.HSet(RDS_NS_PROCESSED_TIME, RDS_NS_PROCESSED_TIME_FIELD_MIN, strPTime) == -1)
            return false;
        if (m_zCluster.HSet(RDS_NS_PROCESSED_TIME, RDS_NS_PROCESSED_TIME_FIELD_AVERAGE, strPTime) == -1)
            return false;
    }
    
    // Set max processed time
    uint64_t uMaxTime = GetMaxProcessedTime();
    if (uMaxTime == 0)
        return false;
    if (uPTime > uMaxTime)
    {
        if (m_zCluster.HSet(RDS_NS_PROCESSED_TIME, RDS_NS_PROCESSED_TIME_FIELD_MAX, strPTime) == -1)
            return false;
    }
    
    // Set min processed time
    uint64_t uMinTime = GetMinProcessedTime();
    if (uMinTime == 0)
        return false;
    if (uPTime < uMinTime)
    {
        if (m_zCluster.HSet(RDS_NS_PROCESSED_TIME, RDS_NS_PROCESSED_TIME_FIELD_MIN, strPTime) == -1)
            return false;
    }
    
    // Set average processed time
    uint64_t uReqTotal = GetTotalRequest();
    if (uReqTotal == 0)
        return false;
    if (uReqTotal == 1)
        return true;
    
    // cal average processed time
    uint64_t uAvgTime = GetAverageProcessedTime();
    if (uAvgTime == 0)
        return false;

    uAvgTime = (uint64_t) ((((uAvgTime*(uReqTotal - 1)) + uPTime) / uReqTotal) + 0.5);
    if (m_zCluster.HSet(RDS_NS_PROCESSED_TIME, RDS_NS_PROCESSED_TIME_FIELD_AVERAGE, Poco::NumberFormatter::format(uAvgTime)) == -1)
        return false;
    
    return true;
}

bool ZRedisUtil::HSetRequestCounter(bool bProcessedMsgResult)
{
    // request counter
    uint8_t uIsExists = m_zCluster.Exists(RDS_NS_REQ_COUNTER);
    if (uIsExists == -1)
        return false;
    
    // Set hash key if key isn't exists
    if (uIsExists == 0)
    {
        if (m_zCluster.HSet(RDS_NS_REQ_COUNTER, RDS_NS_REQ_COUNTER_FIELD_TOTAL, "0") == -1)
            return false;
        if (m_zCluster.HSet(RDS_NS_REQ_COUNTER, RDS_NS_REQ_COUNTER_FIELD_SUCCEED, "0") == -1)
            return false;
    }
    
    // Set total request
    if (m_zCluster.HIncrby(RDS_NS_REQ_COUNTER, RDS_NS_REQ_COUNTER_FIELD_TOTAL, 1) == 0)
        return false;
    
    // Set total success request
    if (bProcessedMsgResult)
    {
        if (m_zCluster.HIncrby(RDS_NS_REQ_COUNTER, RDS_NS_REQ_COUNTER_FIELD_SUCCEED, 1) == 0)
            return false;
    }
    
    return true;
}

bool ZRedisUtil::CheckMsgExists(uint64_t uMsgId)
{
    std::string strMsgKey = GetMsgKey(uMsgId);
    if (m_zCluster.Exists(strMsgKey) != 1)
        return false;
    
    return true;
}

bool ZRedisUtil::GetListMsgBySender(uint64_t uSenderId, std::vector<uint64_t>& vtMsgId)
{
    vtMsgId.clear();
    std::string strKey = GetSenderMsgListKey(uSenderId);
    
    return m_zCluster.ZRangeInteger(strKey, 0, -1, vtMsgId);
}

bool ZRedisUtil::GetListMsgByUser(uint64_t uUserId, std::vector<uint64_t>& vtMsgId)
{
    vtMsgId.clear();
    std::string strKey = GetUserMsgListKey(uUserId);
    
    return m_zCluster.ZRangeInteger(strKey, 0, -1, vtMsgId);  
}

bool ZRedisUtil::GetListSenderByUser(const uint64_t uUserId, std::vector<uint64_t>& vtSenders)
{
    vtSenders.clear();
    
    std::vector<uint64_t> vtMsgId;
    if (!GetListMsgByUser(uUserId, vtMsgId))
        return false;
    
    std::set<uint64_t> setSender;
    for (uint32_t i = 0; i < vtMsgId.size(); i++)
    {
        if (!CheckMsgExists(vtMsgId[i]))
            continue;
            
        uint64_t uSenderId = m_zCluster.HGetInteger(GetMsgKey(vtMsgId[i]), RDS_NS_MSG_INFO_FIELD_SENDER_ID);
        if (uSenderId == 0)
            continue;
        
        setSender.insert(uSenderId);               
    }
    std::copy(setSender.begin(), setSender.end(), std::back_inserter(vtSenders));
    
    return true;
}

bool ZRedisUtil::GetListUserBySender(uint64_t uSenderId, std::vector<uint64_t>& vtUsers)
{
    vtUsers.clear();
    
    std::vector<uint64_t> vtMsgId;
    if (!GetListMsgBySender(uSenderId, vtMsgId))
        return false;
    
    std::set<uint64_t> setUser;
    for (uint32_t i = 0; i < vtMsgId.size(); i++)
    {
        if (!CheckMsgExists(vtMsgId[i]))
            continue;
        
        uint64_t uUserId = m_zCluster.HGetInteger(GetMsgKey(vtMsgId[i]), RDS_NS_MSG_INFO_FIELD_USER_ID);
        if (uUserId == 0)
            continue;
        
        setUser.insert(uUserId);               
    }
    std::copy(setUser.begin(), setUser.end(), std::back_inserter(vtUsers));
    
    return true;
}

uint64_t ZRedisUtil::GetTotalRequest()
{
    return m_zCluster.HGetInteger(RDS_NS_REQ_COUNTER, RDS_NS_REQ_COUNTER_FIELD_TOTAL);
}

uint64_t ZRedisUtil::GetTotalSucceedRequest()
{
    return m_zCluster.HGetInteger(RDS_NS_REQ_COUNTER, RDS_NS_REQ_COUNTER_FIELD_SUCCEED);
}

uint64_t ZRedisUtil::GetTotalFailedRequest()
{
    uint64_t uRet = 0;
    uint64_t uTotalReq = GetTotalRequest();
    uint64_t uTotalSucceed = GetTotalSucceedRequest();

    if (uTotalReq < uTotalSucceed)
        return uRet;
    
    uRet = uTotalReq - uTotalSucceed;
    
    return uRet;
}

uint64_t ZRedisUtil::GetAverageProcessedTime()
{
    return m_zCluster.HGetInteger(RDS_NS_PROCESSED_TIME, RDS_NS_PROCESSED_TIME_FIELD_AVERAGE);
}

uint64_t ZRedisUtil::GetMaxProcessedTime()
{
    return m_zCluster.HGetInteger(RDS_NS_PROCESSED_TIME, RDS_NS_PROCESSED_TIME_FIELD_MAX);
}

uint64_t ZRedisUtil::GetMinProcessedTime()
{
    return m_zCluster.HGetInteger(RDS_NS_PROCESSED_TIME, RDS_NS_PROCESSED_TIME_FIELD_MIN);
}

bool ZRedisUtil::GetAllSenders(std::vector<uint64_t>& vtSenders)
{
    vtSenders.clear();
    
    if (!m_zCluster.SMembersInteger(RDS_NS_SENDERS, vtSenders))
        return false;
    
    return true;
}

bool ZRedisUtil::GetAllUsers(std::vector<uint64_t>& vtUsers)
{
    vtUsers.clear();
    
    if (!m_zCluster.SMembersInteger(RDS_NS_USERS, vtUsers))
        return false;
    
    return true;
}
