package main

/*
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <string.h>
#include <stdlib.h>

#define CONFIG_FILE "config.lua"

char *host_buff;

void getHostInfo()
{
	const char *host;
	int port;

	host_buff = malloc(128);

	lua_State *L = luaL_newstate();
	luaL_dofile(L, CONFIG_FILE);

	lua_getglobal(L, "HOST");
	lua_getglobal(L, "PORT");

	if (!lua_isstring(L, -2))
		luaL_error(L, "'HOST' should be a string\n");
	if (!lua_isnumber(L, -1))
		luaL_error(L, "'PORT' should be a number\n");

	host = lua_tostring(L, -2);
	port = lua_tointeger(L, -1);
	if (0 != strlen(host))
		snprintf(host_buff, 128, "%s:%d", host, port);
	else
		snprintf(host_buff, 128, ":%d", port);

	lua_close(L);
}

int getPktSize()
{
	int size;

	lua_State *L = luaL_newstate();
	luaL_dofile(L, CONFIG_FILE);

	lua_getglobal(L, "MAX_PKG_SIZE");

	if (!lua_isnumber(L, -1))
		luaL_error(L, "'PORT' should be a number\n");

	size = lua_tointeger(L, -1);

	lua_close(L);
	return size;
}

void free_buff()
{
	if (NULL != host_buff) {
		free(host_buff);
		host_buff = NULL;
	}
}

#cgo linux LDFLAGS: -llua -lm -ldl
#cgo windows,386 CFLAGS:  -I"lua-5.2.3/include"
#cgo windows,386 LDFLAGS: -L"lua-5.2.3/lib" -llua52
*/
import "C"

import (
    "io"
	"fmt"
	"net"
	"time"
	"unsafe"
)

var (
	chDone chan bool
	MAX_PKG_SIZE int
)

func getService() (s string) {
	C.getHostInfo()
    s = C.GoString(C.host_buff)
	C.free_buff()
	MAX_PKG_SIZE = int(C.getPktSize())
	fmt.Println(MAX_PKG_SIZE,s)
	return
}

func main() {
	chDone = make(chan bool)
	go msgProcessor()
	<-chDone
}

func msgProcessor() {
	service := getService()
	tmp_buff := make([]byte, 5242880)	// 5 * 1024 * 1024
	read_buff := make([]byte, 5242880)	// 5 * 1024 * 1024

	conn, err := net.Dial("tcp", service)
	if err != nil {
		fmt.Println(err)
	}
	defer conn.Close()

    buildAndSend(&tmp_buff, &conn, MSG_START)

	for {
		n , err := conn.Read(read_buff)
		if err != nil {
            if err != io.EOF {
                fmt.Println("Read error :", err)
                continue
            } else {
				break
			}
		}

		msg_type := parse_msg(&tmp_buff, &read_buff)
		if ERR_MSG_TYPE == msg_type {
			continue			// parse failed
		}
		consumeDataAndFeedBack(&tmp_buff, msg_type, &conn, n);
		//time.Sleep(time.Second * 3600)
	}

	chDone<-true
}

func consumeDataAndFeedBack(write_buff *[]byte, msg_type byte, conn *net.Conn, n int) {
	switch msg_type {
	case MSG_ASK_IMG:
		buildAndSend(write_buff, conn, MSG_LBL)
		buildAndSend(write_buff, conn, MSG_IMG)
	}
}

func buildAndSend(write_buff *[]byte, conn *net.Conn, msg_type byte) {
	msg_len, ok := build_msg(write_buff, msg_type)
	if !ok {
		return
	}
	if (msg_len > 0) {
        //(*conn).Write((*write_buff)[:msg_len])
		splitAndSend(write_buff, conn)
	}
}

func splitAndSend(shm *[]byte, conn *net.Conn) {
	msg := (*ipc_msg)(unsafe.Pointer(&(*shm)[0]))
	data_len := int((*msg).msg_len)
	max_pkg_data := MAX_PKG_SIZE - HEADER_LEN

	(*msg).total_len = (*msg).msg_len
	if data_len <= max_pkg_data {
		(*conn).Write((*shm)[:data_len+HEADER_LEN])
	} else {
		alreadSent := 0
		buff := make([]byte, MAX_PKG_SIZE)
		var left, piece int
		var done bool = false
		for {
			time.Sleep(time.Millisecond * 20)
			buff = make([]byte, MAX_PKG_SIZE)
			left = data_len - alreadSent
			if left > max_pkg_data {
				piece = max_pkg_data
				(*msg).lastPiece = false
			} else {
				piece = left
				done = true
				(*msg).lastPiece = true
			}
			(*msg).offset = uint32(alreadSent)
			(*msg).msg_len = uint32(piece)
			copy(buff, (*shm)[:HEADER_LEN])
			copy(buff[HEADER_LEN:], (*shm)[HEADER_LEN+alreadSent:HEADER_LEN+alreadSent+piece])
			fmt.Println(HEADER_LEN+alreadSent,HEADER_LEN+alreadSent+piece)
			(*conn).Write(buff[:piece+HEADER_LEN])
			alreadSent += piece
			if done {
				break
			}
		}
	}
}
