package main

import (
	"unsafe"
	"os"
	"io"
    "fmt"
)

const (
	// sync bytes
	SYNC1 = 0xF8
	SYNC2 = 0x8F
	SYNC3 = 0x9F
	SYNC4 = 0xF9
	// msg type
	MSG_ASK_IMG = 0x01
	MSG_IMG = 0x02
	MSG_START = 0x03
	MSG_LBL = 0x04
	// other const
	ERR_MSG_TYPE = 0xFF
	ERR_BUILD_MSG = 0
)

var (
	HEADER_LEN int
)

type ipc_msg struct {
	sync [4]byte
	msg_type byte
	offset uint32
	lastPiece bool
	msg_len uint32
	total_len uint32
}

func init() {
	var m ipc_msg
	HEADER_LEN = int(unsafe.Sizeof(m))
}

func header_sync(shm *[]byte) {
    (*shm)[0] = SYNC1
    (*shm)[1] = SYNC2
    (*shm)[2] = SYNC3
    (*shm)[3] = SYNC4
}

func build_msg(shm *[]byte, msg_type byte) (uint32, bool) {
	var msg_len uint32

    header_sync(shm)

    switch msg_type {
    case MSG_LBL:
        msg_len = msg_lbl(shm, msg_type);
    case MSG_START:
        msg_len = msg_start(shm, msg_type)
    case MSG_IMG:
        msg_len = msg_img(shm, msg_type)
    }

	if msg_len == ERR_BUILD_MSG {
		return ERR_BUILD_MSG, false
	}else {
		return msg_len + uint32(HEADER_LEN), true
	}
}

func parse_msg(shm *[]byte, buff *[]byte) byte {
	from := (*ipc_msg)(unsafe.Pointer(&(*buff)[0]))

    if !validate_sync(from) {
        return ERR_MSG_TYPE
	}

	copy(*shm, *buff)			// TODO

    return (*from).msg_type
}

func validate_sync(msg *ipc_msg) bool{
    var sync []byte = []byte{SYNC1,SYNC2,SYNC3,SYNC4}
    for i:=0;i<4;i++ {
        if (*msg).sync[i] != sync[i] {
            return false
		}
	}
    return true
}

func msg_img(shm *[]byte, msg_type byte) uint32{
	img := "./cryingJingChou.png"

	f, err := os.Open(img)
	if err != nil {
		return ERR_BUILD_MSG
	}
	defer f.Close()

	msg := (*ipc_msg)(unsafe.Pointer(&(*shm)[0]))

    (*msg).msg_type = msg_type
    (*msg).offset = 0
    (*msg).lastPiece = true

	n, err := f.Read((*shm)[HEADER_LEN:])
	if err != nil {
		if err != io.EOF {
			f.Close()
			return ERR_BUILD_MSG
		}
	}
    fmt.Println("xxxxxxxxxxxxxxx", n)
    (*msg).msg_len = uint32(n)
    return (*msg).msg_len
}

func msg_start(shm *[]byte, msg_type byte) uint32 {
	src := "Backend Started Successfully"
	msg := (*ipc_msg)(unsafe.Pointer(&(*shm)[0]))

    (*msg).msg_type = msg_type
    (*msg).offset = 0
    (*msg).lastPiece = true

    (*msg).msg_len = uint32(len(src))
	copy((*shm)[HEADER_LEN:], []byte(src))

    return (*msg).msg_len
}

func msg_lbl(shm *[]byte, msg_type byte) uint32 {
    src := "靖仇，真的爲難你了...\n今後爲師不會再勉強你做任何事\n你就儘管做你想做的事吧------"
	msg := (*ipc_msg)(unsafe.Pointer(&(*shm)[0]))

    (*msg).msg_type = msg_type
    (*msg).offset = 0
    (*msg).lastPiece = true

    (*msg).msg_len = uint32(len(src))
	copy((*shm)[HEADER_LEN:], []byte(src))

    return (*msg).msg_len
}
