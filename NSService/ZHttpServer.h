/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ZHttpServer.h
 * Author: danhcc
 *
 * Created on October 13, 2016, 3:04 PM
 */

#ifndef ZHTTPSERVER_H
#define ZHTTPSERVER_H

#include <Poco/Util/ServerApplication.h>

class ZHttpServer : public Poco::Util::ServerApplication
{
protected:
    int main(const std::vector<std::string> &args);
};

#endif /* ZHTTPSERVER_H */

