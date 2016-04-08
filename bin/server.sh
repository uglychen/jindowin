#!/usr/bin/env bash

#=============================================================================
#    Copyright (c) 2015
#    ShanghaiKunyan.  All rights reserved
#
#    Filename     : /home/wukun/work/enum_to_switch.py
#    Author       : Sunsolo(卓一航)
#    Email        : wukun@kunyan-inc.com
#    Date         : 2015-12-29 11:27
#    Description  : 
#=============================================================================

FG_GREEN="\033[32;1m"
FG_RED="\033[31;1m"
FG_YELLOW="\033[33;1m"
END="\033[0m"

SCRIPT_NAME=`basename "$0"`
DIR_NAME=`dirname "$0"`
SERVER_NAME="httpprismloader.out"
THREAD_INFO=`ps -elf | grep -w "${SERVER_NAME}" | grep -v  "grep" | grep -v "gdb" |  awk '{print $4}'`

function status(){
    if [ -z "${THREAD_INFO}" ];then
        echo -e ${FG_YELLOW}"The server[${SERVER_NAME}] is dead"${END}
    else 
        echo -e ${FG_GREEN}"The server[${SERVER_NAME}]'s pid is ${THREAD_INFO}"${END}
    fi
}

function stop(){
    if [ -z "${THREAD_INFO}" ];then
        echo -e ${FG_YELLOW}"The server[${SERVER_NAME}] has dead"${END}
    else 
        `kill -9 ${THREAD_INFO}`
        
        if [ $? -eq "0" ];then
            echo -e ${FG_GREEN}"The server[${SERVER_NAME}] has stoped normal"${END}
        else 
            echo -e ${RED}"Stopping server[${SERVER_NAME}] is error"${END}
        fi
    fi
}

function start(){
    if [ -z "${THREAD_INFO}" ];then
        cd ${DIR_NAME}; nohup ./${SERVER_NAME} &
        if [ $? -eq "0" ]; then
            echo -e ${FG_GREEN}"The server[${SERVER_NAME}] has started normal"${END}
        else 
            echo -e ${RED}"Starting server[${SERVER_NAME}] is failed"${END}
        fi
    else 
        echo -e ${FG_YELLOW}"The server[${SERVER_NAME}] is running"${END}
    fi
}

case "$1" in 
    start) 
        start  ;;
    stop) 
        stop   ;;
    restart)
        stop
        start  ;;
    status) 
        status ;;
        * )
        echo -e ${FG_GREEN}"Usage:\n     $0 [start|stop|status|restart]" ;;
esac
