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
    if (strHost.empty() || !port)
    {
        std::cerr << "Info data invalid: " + strHost + ":" + Poco::NumberFormatter::format(port) << std::endl;
        return false;
    }
    
    if (!m_zCluster.Init(strHost, port))
    {
        std::cerr << "Connect to Redis failed";
        return false;
    }
    
    return true;
}

std::string ZRedisUtil::GetMsgKey(uint64_t uMsgId)
{
    std::string strMsgId = Poco::NumberFormatter::format(uMsgId);
    return std::string("ns:msg:" + strMsgId + ":info");
}

std::string ZRedisUtil::GetKeyListMsgByUserID(uint64_t uUserId)
{
    std::string strUserId = Poco::NumberFormatter::format(uUserId);
    return std::string("ns:msg_list_of_user:" + strUserId);
}

std::string ZRedisUtil::GetKeyListMsgBySenderID(uint64_t uSenderId)
{
    std::string strSenderId = Poco::NumberFormatter::format(uSenderId);
    return std::string("ns:msg_list_of_sender:" + strSenderId);
}

bool ZRedisUtil::SaveInfo(uint64_t uSenderId, uint64_t uUserId,\
                           const std::string& strMsgData, bool bProcessedMsgResult,\
                           uint64_t uReqTime, uint64_t uSendTime)
{
    if (strMsgData.empty() || (uReqTime > uSendTime))
        return false;
   
    uint64_t uMsgId = SaveMsgInfo(uSenderId, uUserId, strMsgData, bProcessedMsgResult, uReqTime, uSendTime);
    if (uMsgId == 0)
    { 
        std::cout << "save msg failed" << std::endl;
        return false;
    }
    
    // set sender
    if (m_zCluster.SAdd(RDS_NS_SENDERS, Poco::NumberFormatter::format(uSenderId)) == -1)
    { 
        std::cout << "set senders failed senderid=" << uSenderId << std::endl;
        return false;
    }
    // set user
    if (m_zCluster.SAdd(RDS_NS_USERS, Poco::NumberFormatter::format(uUserId)) == -1)
    { 
        std::cout << "set users failed userid=" << uUserId << std::endl;
        return false;
    }
    
    std::string strMsgId = Poco::NumberFormatter::format(uMsgId);
    // zset msg list of sender
    if (m_zCluster.ZAdd(GetKeyListMsgBySenderID(uSenderId), uReqTime, strMsgId) == -1)
    { 
        std::cout << "Set sender msg list failed senderid=" << uSenderId << std::endl;
        return false;
    }
    // zset msg list of user
    if (m_zCluster.ZAdd(GetKeyListMsgByUserID(uUserId), uSendTime, strMsgId) == -1)
    { 
        std::cout << "Set user msg list failed userid=" << uUserId << std::endl;
        return false;
    }
    
    // request counter
    if (!UpdateRequestCounter(bProcessedMsgResult))
    { 
        std::cout << "Update request counter failed msgid=" << uMsgId << std::endl;
        return false;
    }
    
    // min, max, average processed time
    if (!UpdateProcessedTime(uSendTime - uReqTime))
    { 
        std::cout << "Update processed time failed msgid=" << uMsgId << std::endl;
        return false;
    }
    
    return true;
}

uint64_t ZRedisUtil::SaveMsgInfo(uint64_t uSenderId, uint64_t uUserId,\
                            const std::string& strMsgData, bool bProcessedMsgResult,\
                            uint64_t uReqTime, uint64_t uSendTime)
{
    uint64_t uRet = 0;
    if (strMsgData.empty() || (uReqTime > uSendTime))
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
    if (m_zCluster.HSet(strHash, RDS_NS_MSG_INFO_FILED_PROCESSED_RESULT, (bProcessedMsgResult ? "1":"0")) == -1)
        return uRet;
    if (m_zCluster.HSet(strHash, RDS_NS_MSG_INFO_FIELD_REQUEST_TIME, Poco::NumberFormatter::format(uReqTime)) == -1)
        return uRet;
    if (m_zCluster.HSet(strHash, RDS_NS_MSG_INFO_FIELD_SEND_TIME, Poco::NumberFormatter::format(uSendTime)) == -1)
        return uRet;
    
    return uMsgIdIncr;
}

bool ZRedisUtil::SetProcessedTime(uint64_t uPTime)
{
    std::string strPTime = Poco::NumberFormatter::format(uPTime);
    if (!m_zCluster.Set(RDS_NS_PROCESSED_TIME_MAX, strPTime))
        return false;
    if (!m_zCluster.Set(RDS_NS_PROCESSED_TIME_MIN, strPTime))
        return false;
    if (!m_zCluster.Set(RDS_NS_PROCESSED_TIME_AVERAGE, strPTime))
        return false;

    return true;
}


bool ZRedisUtil::UpdateProcessedTime(uint64_t uPTime)
{
    // processed time
    uint64_t uIsExists = m_zCluster.Exists(RDS_NS_PROCESSED_TIME_AVERAGE);
    if (uIsExists == -1)
        return false;
    
    // Set key if key isn't exists
    if (uIsExists == 0)
        return SetProcessedTime(uPTime);
    
    std::string strPTime = Poco::NumberFormatter::format(uPTime);
    // Set max processed time
    uint64_t uMaxTime = 0;
    if (!GetMaxProcessedTime(uMaxTime))
        return false;
    if (uPTime > uMaxTime)
    {
        if (!m_zCluster.Set(RDS_NS_PROCESSED_TIME_MAX, strPTime))
            return false;
    }
    
    // Set min processed time
    uint64_t uMinTime = 0;
    if (!GetMinProcessedTime(uMinTime))
        return false;
    if (uPTime < uMinTime)
    {
        if (!m_zCluster.Set(RDS_NS_PROCESSED_TIME_MIN, strPTime))
            return false;
    }
    
    // Set average processed time
    uint64_t uReqTotal = 0;
    if (!GetTotalRequest(uReqTotal))
        return false;
    if (uReqTotal == 0)
        return false;
    if (uReqTotal == 1)
        return true;
    
    // cal average processed time
    uint64_t uAvgTime = 0;
    if (!GetAverageProcessedTime(uAvgTime))
        return false;

    uAvgTime = (uint64_t) ((((uAvgTime*(uReqTotal - 1)) + uPTime) / uReqTotal) + 0.5);
    if (!m_zCluster.Set(RDS_NS_PROCESSED_TIME_AVERAGE, Poco::NumberFormatter::format(uAvgTime)))
        return false;
    
    return true;
}

bool ZRedisUtil::UpdateRequestCounter(bool bProcessedMsgResult)
{
    if (m_zCluster.Incr(RDS_NS_REQ_COUNTER_TOTAL) == 0)
        return false;
    
    if (!bProcessedMsgResult)
    {
        if (m_zCluster.Incr(RDS_NS_REQ_COUNTER_FAILED) == 0)
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
    std::string strKey = GetKeyListMsgBySenderID(uSenderId);
    return m_zCluster.ZRangeInteger(strKey, 0, -1, vtMsgId);
}

bool ZRedisUtil::GetListMsgByUser(uint64_t uUserId, std::vector<uint64_t>& vtMsgId)
{
    vtMsgId.clear();
    std::string strKey = GetKeyListMsgByUserID(uUserId);
    return m_zCluster.ZRangeInteger(strKey, 0, -1, vtMsgId);  
}

bool ZRedisUtil::GetListSenderByUser(uint64_t uUserId, std::vector<uint64_t>& vtSenders)
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

bool ZRedisUtil::GetTotalRequest(uint64_t& uResult)
{
    uResult = 0;
    return m_zCluster.GetInteger(RDS_NS_REQ_COUNTER_TOTAL, uResult);
}

bool ZRedisUtil::GetTotalFailedRequest(uint64_t& uResult)
{
    uResult = 0;
    return m_zCluster.GetInteger(RDS_NS_REQ_COUNTER_FAILED, uResult);
}

bool ZRedisUtil::GetTotalSucceedRequest(uint64_t& uResult)
{
    uResult = 0;
    uint64_t uTotal = 0;
    if (!GetTotalRequest(uTotal))
        return false;
    
    uint64_t uTotalFailed = 0;
    if (!GetTotalFailedRequest(uTotalFailed))
        return false;
    
    uResult = uTotal - uTotalFailed;
    return true;
}

bool ZRedisUtil::GetAverageProcessedTime(uint64_t& uResult)
{
    uResult = 0;
    return m_zCluster.GetInteger(RDS_NS_PROCESSED_TIME_AVERAGE, uResult);
}

bool ZRedisUtil::GetMaxProcessedTime(uint64_t& uResult)
{
    uResult = 0;
    return m_zCluster.GetInteger(RDS_NS_PROCESSED_TIME_MAX, uResult);
}

bool ZRedisUtil::GetMinProcessedTime(uint64_t& uResult)
{
    uResult = 0;
    return m_zCluster.GetInteger(RDS_NS_PROCESSED_TIME_MIN, uResult);
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
