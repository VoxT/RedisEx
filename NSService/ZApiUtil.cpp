/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "ZApiUtil.h"
#include "ZLogUtil.h"
#include "ZApiDefine.h"
#include <sys/time.h>
#include <iostream>

std::string ZApiUtil::GetResponseString(uint32_t u32Code, const std::string& strMessage)
{
    return ("{\"code\":" + Poco::NumberFormatter::format(u32Code) + ",\"message\":\"" + strMessage + "\"}");
}

std::string ZApiUtil::HandleResult(uint32_t u32Code, const std::string& strMessage, bool bIsError)
{
    if (bIsError) {
        ZLogUtil::instance().Error("Response code=" + Poco::NumberFormatter::format(u32Code) + " mes=" + strMessage);
    }
    else {
        ZLogUtil::instance().Debug("Response code=" + Poco::NumberFormatter::format(u32Code) + " mes=" + strMessage);
    }
    
    std::string strRet = ZApiUtil::GetResponseString(u32Code, strMessage);
    return strRet;
}

bool ZApiUtil::GetJsonDataFromRequest(Poco::Net::HTTPServerRequest& request, std::string& strJsonData, std::string& strErrMsg)
{
    strErrMsg.clear();
    strJsonData.clear();
    
    std::istream& reqStream = request.stream();
    uint8_t buffer[MAX_REQ_BUFFER_SIZE + 1] = {0};
    const uint32_t uContentLen = request.getContentLength();

    if (uContentLen > MAX_REQ_BUFFER_SIZE)
    {
        strErrMsg = ZApiUtil::HandleResult(API_RES_REQ_SIZE_TOO_BIG, "Request content size too big", true);
        return false;
    }
    
    uint32_t uCountBytes = 0;
    try {
        while (!reqStream.eof() && reqStream.good() && (uContentLen > uCountBytes)) 
        {
            uint32_t uReadBytes = MAX_REQ_BUFFER_SIZE - uCountBytes;
            reqStream.read((char*) buffer + uCountBytes, uReadBytes);
            uCountBytes += reqStream.gcount();
        }
    }
    catch (std::exception& e) {
        ZLogUtil::instance().Debug("Read req data has exception: " + std::string(e.what()));
        strErrMsg = ZApiUtil::HandleResult(API_RES_READ_REQ_FAILED, "Read request data failed", true);
        return false;
    }
    
    if (uContentLen != uCountBytes)
    {
        strErrMsg = ZApiUtil::HandleResult(API_RES_READ_REQ_FAILED, "Read request content failed", true);
        return false;
    }
    
    strJsonData = std::string ((char*) buffer, uContentLen);    
    
    return true;
}

uint64_t ZApiUtil::GetTimestampMillis()
{
   struct timeval tp;
   gettimeofday(&tp, NULL);
   //get current timestamp in milliseconds
   return ((uint64_t) (tp.tv_sec * 1000L + tp.tv_usec / 1000)); 
}

