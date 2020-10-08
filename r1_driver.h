/**
 * @file r1_driver.h 
 * @brief R1 driver library is for driving OMO-R1 robot along with other OMOROBOT's proprietary sensor modules using CAN transceiver such as MCP2515 
 * For more info, please visit www.omorobot.com 
 *  
 * @License
 * Copyright (c) OMOROBOT INC. All rights reserved. Copyright (c) Kyuhyong You. All rights reserved. 
 * This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 * */
#ifndef _R1_DRIVER_H_
#define _R1_DRIVER_H_

#include <inttypes.h>

#define CAN_MOTOR_CMD_VW        0x81
#define CAN_MOTOR_ODO_REQUEST   0x82
#define CAN_MOTOR_ODO_RESET     0x83

//#define DEBUG_DRIVER      //Uncomment this to print can messages on Serial port

class MCP2515;

enum R1_MessageType{
    R1MSG_ODO,
    R1MSG_LINEPOS,
    R1MSG_LINEOUT
};

enum R1_DriveMode{
    R1DRV_DefaultMode,
    R1DRV_LineTracerMode        ///Set to line tracer mode when Line sensor available
};

enum TAG_Type{
    TAG_None    = 0,
    TAG_DEPOT   = 0xA0,     //160
    TAG_POU     = 0xA2,     //162
    TAG_APPROACH= 0xAA,     //170
    TAG_TURN    = 0xB0,     //176
    TAG_CIN     = 0xC1,     //193
    TAG_COUT    = 0xC0,      //192
    TAG_SPEED   = 0xE0,     //224
    TAG_SONAR   = 0xE2,     //226
    TAG_READY   = 0xFE     //254
};

typedef struct {
    uint8_t     bytes[4];
    TAG_Type     type;
} Tag_Struct;

class OMOROBOT_R1
{
    typedef void (*loop_event)(void);
public:
    
    typedef void (*R1_NewDataClientEvent)(R1_MessageType);
    typedef void (*R1_NewTagReadEvent)(Tag_Struct);
    OMOROBOT_R1();
    OMOROBOT_R1(uint16_t cspin);        //Added to support for different cs pin
    OMOROBOT_R1(MCP2515* mcp2515);

    void    begin(void);
    void    onNewData(R1_NewDataClientEvent cbEvent);
    void    onNewTag(R1_NewTagReadEvent cbEvent);
    void    spin(void);
    void    control_motor_VW(int V, int W);
    void    request_odo();
    void    set_driveMode(R1_DriveMode mode);
    void    set_lineoutTime(int ms);
    void    new_can_line(struct can_frame can_rx);
    void    new_can_odo(struct can_frame can_rx);
    void    go(int target_speed);
    void    go(void);
    void    stop();
    void    pause();
    bool    is_going();
    int     get_odo_l();
    int     get_odo_r();
    int8_t  get_linePos();
    int     get_lineoutTimer();
    int     can_TxMsg_init(struct can_frame* frame, int id, int dlc);
private:
    
    MCP2515 *_mcp2515;
    R1_NewDataClientEvent   _cbDataEvent;
    R1_NewTagReadEvent      _cbTagEvent;
    R1_DriveMode            _drive_mode;
    loop_event              _3ms_loop;
    loop_event              _10ms_loop;
    bool                    _odoReset = false;
    int                     _odo_l;
    int                     _odo_r;
    uint64_t                _odoRequest_millis_last;
    uint64_t                _lineDetect_millis_last;            //Last time line detected millis
    bool                    _isLineOut = false;

    uint64_t                _3ms_loop_millis_last;
    uint64_t                _10ms_loop_millis_last;
    uint64_t                _100ms_loop_millis_last;
    bool                    _can_rx_extern = false;             //Can rx read performed externally
    //uint8_t     _tag_data[4];
    uint8_t                 _tag_data_prev[4];
    uint16_t                _same_tag_reset_timer;
    Tag_Struct              _new_tagStr;
};

#endif