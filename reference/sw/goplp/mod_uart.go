package main

import (
	"sync"
//	"fmt"
//	"net"
)

const (
	mod_uart_size  = 16
	mod_uart_start = 0xf0000000
)

var uart_status uint32 = 0x01
var uart_receive uint32 = 0
var uart_send uint32 = 0
var uart_lock sync.Mutex
var uart_send_queue = make(chan byte, 1024)

// modules register themselves in the memory map :)
func init() {
	map_register(mod_uart_start, mod_uart_start+mod_uart_size-4, mod_uart_read, mod_uart_write)
}

func mod_uart_read(address uint32) uint32 {
	a := address - mod_uart_start
	switch a {
	case 0:
		return 0
	case 4:
		return uart_status
	case 8:
		return uart_receive
	case 12:
		return uart_send
	default:
		return 0
	}
	return 0
}

func mod_uart_write(address, data uint32) {
	a := address - mod_uart_start
	switch a {
	case 0:
		mod_uart_command(data)
	case 12:
		uart_send = data
	}
}

func mod_uart_command(d uint32) {
	switch d {
	case 1: // send
	case 2: // clear ready bit
		uart_lock.Lock()
		uart_status = 0x01
		uart_lock.Unlock()
	}
}

