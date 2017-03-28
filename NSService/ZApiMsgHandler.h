/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ZApiMsgHandler.h
 * Author: Thieu Vo
 *
 * Created on March 21, 2017, 2:43 PM
 */

#ifndef ZAPIMSGHANDLER_H
#define ZAPIMSGHANDLER_H


#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

class ZApiMsgHandler : public Poco::Net::HTTPRequestHandler 
{
public:
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
    bool ProceessData(const std::string& strMsgData);
private:

};

#endif /* ZAPIMSGHANDLER_H */

