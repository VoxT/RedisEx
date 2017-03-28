/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ZHandlerFactory.h
 * Author: danhcc
 *
 * Created on October 13, 2016, 3:05 PM
 */

#ifndef ZHANDLERFACTORY_H
#define ZHANDLERFACTORY_H

#include <Poco/Net/HTTPRequestHandlerFactory.h>

class ZHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
    ZHandlerFactory();

    virtual Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);

};

#endif /* ZHANDLERFACTORY_H */

