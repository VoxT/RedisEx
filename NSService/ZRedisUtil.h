/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ZRedisUtil.h
 * Author: Thieu Vo
 *
 * Created on March 22, 2017, 9:43 AM
 */

#ifndef ZREDISUTIL_H
#define ZREDISUTIL_H

#include "redis/zcluster.h"
#include "ZRedisDefine.h"

class ZRedisUtil {
private:
    ZCluster m_zCluster;

public:
    static ZRedisUtil& GetInstance();
    bool Init(const std::string& host, uint32_t port);
    
    
    std::string GetMsgKey(const uint64_t uMsgId)
    {
        std::string strMsgId = Poco::NumberFormatter::format(uMsgId);
        
        return std::string("ns:msg:" + strMsgId + ":info");
    }
    
    std::string GetUserMsgListKey(const uint64_t uUserId)
    {
        std::string strUserId = Poco::NumberFormatter::format(uUserId);
        
        return std::string("ns:msg_list_of_user:" + strUserId);
    }
    
    std::string GetSenderMsgListKey(const uint64_t uSenderId)
    {
        std::string strSenderId = Poco::NumberFormatter::format(uSenderId);
        
        return std::string("ns:msg_list_of_sender:" + strSenderId);
    }

    
    bool SaveInfo(const uint64_t uSenderId, const uint64_t uUserId,\
                   const std::string& strMsgData, bool bProcessedMsgResult,\
                   const uint64_t uReqTime, const uint64_t uSendTime);
    
    bool GetSenderStatistic(const uint64_t uSenderId);
};

#endif /* ZREDISUTIL_H */

