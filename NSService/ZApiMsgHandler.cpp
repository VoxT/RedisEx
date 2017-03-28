/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ZApiMsgHandler.cpp
 * Author: Thieu Vo
 * 
 * Created on March 21, 2017, 2:43 PM
 */

#include "ZApiMsgHandler.h"
#include "ZApiUtil.h"
#include "ZApiExceptionHandler.h"
#include "ZRedisUtil.h"
#include "ZApiDefine.h"

bool ZApiMsgHandler::ProceessData(const std::string& strMsgData)
{
    if (strMsgData.empty())
        return false;
    
    return (rand() % 2);
}

void ZApiMsgHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
    uint64_t uReqTime = ZApiUtil::GetTimestampMillis();
    std::ostream& respStream = response.send();
    
    std::string strJsonData, strError;
    if (!ZApiUtil::GetJsonDataFromRequest(request, strJsonData, strError))
    {
        respStream << strError;
        return;
    }

    std::string strMsgData, strSenderId, strUserId;
    if (!ZApiUtil::GetValueFromJsonString<std::string>(strJsonData, API_KEY_SENDER_ID, strSenderId))
    {
        respStream << strError;
        return;
    }
    if (!ZApiUtil::GetValueFromJsonString<std::string>(strJsonData, API_KEY_USER_ID, strUserId))
    {
            
        respStream << strError;
        return;
    }
    if (!ZApiUtil::GetValueFromJsonString<std::string>(strJsonData, API_KEY_DATA, strMsgData))
    {
        respStream << strError;
        return;
    }
    
    bool bPMsgResult = ProceessData(strMsgData);
    uint64_t uSendTime = ZApiUtil::GetTimestampMillis() + 500;
    
    if (!ZRedisUtil::GetInstance().SavedInfo(strSenderId, strUserId, strMsgData, bPMsgResult, uReqTime, uSendTime))
    {
        respStream << ZApiUtil::HandleResult(API_RES_SAVE_MSG_FAIL, "save message failed.", true);
        return;
    }
    
    respStream << "save info success";
}

