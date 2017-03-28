/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ZApiUtil.h
 * Author: danhcc
 *
 * Created on October 13, 2016, 4:10 PM
 */

#ifndef ZAPIUTIL_H
#define ZAPIUTIL_H


#include <iostream>
#include <Poco/Net/HTTPServerRequest.h>

#include <Poco/NumberFormatter.h>
#include <Poco/Timestamp.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Array.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/Dynamic/VarHolder.h>


class ZApiUtil
{
public:
    static std::string GetResponseString(uint32_t u32Code, const std::string& strMessage);
    static bool GetJsonDataFromRequest(Poco::Net::HTTPServerRequest& request, std::string& strJsonData, std::string& strErrMsg);
    static std::string HandleResult(uint32_t u32Code, const std::string& strMessage, bool bIsError);
    static uint64_t GetTimestampMillis();
    
    template<typename T> static bool GetValueFromJsonString(const std::string& strJsonData, const std::string& strKey, T& tResult)
    {
        if (strJsonData.empty() || strKey.empty())
            return false;

        try {
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result = parser.parse(strJsonData);

            Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

            if (!object->has(strKey)) {
                return false;
            }

            tResult = object->getValue<T>(strKey);

            return true;
        }
        catch (Poco::Exception &ex) {

        }
        catch (std::exception &e) {

        }

        return false;
    }
};

#endif /* ZAPIUTIL_H */

