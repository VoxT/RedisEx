/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ZRedisDefine.h
 * Author: danhcc
 *
 * Created on September 29, 2016, 8:41 AM
 */

#ifndef ZREDISDEFINE_H
#define ZREDISDEFINE_H

#define RDS_NS_MSG_ID_INCR                          "ns:msg_id_incr"
#define RDS_NS_MSG_INFO_FIELD_DATA                  "data"
#define RDS_NS_MSG_INFO_FIELD_SENDER_ID             "senderid"
#define RDS_NS_MSG_INFO_FIELD_USER_ID               "userid"
#define RDS_NS_MSG_INFO_FILED_PROCESSED_RESULT      "result"
#define RDS_NS_MSG_INFO_FIELD_REQUEST_TIME          "reqtime"
#define RDS_NS_MSG_INFO_FIELD_SEND_TIME             "sendtime"
#define RDS_NS_USERS                                "ns:users"
#define RDS_NS_SENDERS                              "ns:senders"

#define MAX_TIME_EXPIRE_KEY                 172800 // seconds (48h)

#endif /* ZREDISDEFINE_H */

