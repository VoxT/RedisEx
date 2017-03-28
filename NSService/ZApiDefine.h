/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ZApiDefine.h
 * Author: danhcc
 *
 * Created on October 13, 2016, 3:46 PM
 */

#ifndef ZAPIDEFINE_H
#define ZAPIDEFINE_H

#define API_RES_SUCCESS                     0
#define API_RES_WRONG_VERSION               400
#define API_RES_WRONG_METHOD                401
#define API_RES_SAVE_MSG_FAIL               402
#define API_RES_WRONG_API                   404
#define API_INVALID_SIGNATURE               403
#define API_RES_PARSE_FAIL                  405
#define API_RES_MISS_DATA                   406
#define API_RES_REQ_SIZE_TOO_BIG            407
#define API_RES_READ_REQ_FAILED             408
#define API_EXCEED_MAX_UIDS_SIZE            409

#define API_RES_MTU_SENT                    51
#define API_RES_MTU_SAVED                   52
#define API_RES_NS_CODE                     "code"

#define API_PATH_NSSERVICE                  "nsservice"
#define API_PATH                            "api"
#define API_PATH_MSG                        "msg"

#define MAX_REQ_BUFFER_SIZE                 10240

#endif /* ZAPIDEFINE_H */

