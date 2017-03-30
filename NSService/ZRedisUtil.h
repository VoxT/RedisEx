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
    
    bool HSetProcessedTime(const uint64_t uPTime);
    bool HSetRequestCounter(const bool bProcessedMsgResult);
    
    /**
     * @param uSenderId
     * @param uUserId
     * @param strMsgData
     * @param bProcessedMsgResult
     * @param uReqTime
     * @param uSendTime
     * @return 0 if failed. otherwise return msg id.
     */
    uint64_t HSetMsg(const uint64_t uSenderId, const uint64_t uUserId,\
                   const std::string& strMsgData,const bool bProcessedMsgResult,\
                   const uint64_t uReqTime, const uint64_t uSendTime);
    
    bool CheckMsgExists(const uint64_t uMsgId);
        
public:
    static ZRedisUtil& GetInstance();
    bool Init(const std::string& host, uint32_t port);
            
    bool SaveInfo(const uint64_t uSenderId, const uint64_t uUserId,\
                   const std::string& strMsgData,const bool bProcessedMsgResult,\
                   const uint64_t uReqTime, const uint64_t uSendTime);
    
    bool GetListUserBySender(const uint64_t uSenderId, std::vector<uint64_t>& vtUserId);
    bool GetListSenderByUser(const uint64_t uUserId, std::vector<uint64_t>& vtSenderid);
    bool GetListMsgBySender(const uint64_t uSenderId, std::vector<uint64_t>& vtMsgId);
    bool GetListMsgByUser(const uint64_t uUserId, std::vector<uint64_t>& vtMsgId);
    /**
     * @return 0 if failed. otherwise return total request.
     */
    uint64_t GetTotalRequest();
    
    /**
     * @return 0 maybe failed. otherwise return total succeed request
     */
    uint64_t GetTotalSucceedRequest();
    
    /**
     * @return 0 maybe failed. otherwise return total failed request
     */
    uint64_t GetTotalFailedRequest();
    uint64_t GetAverageProcessedTime();
    uint64_t GetMaxProcessedTime();
    uint64_t GetMinProcessedTime();
    
};

#endif /* ZREDISUTIL_H */

