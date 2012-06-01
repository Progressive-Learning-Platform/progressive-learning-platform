package main

import (
	"fmt"
	"net"
	"sync"
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
	go uart_listen()
}

func mod_uart_read(address uint32) uint32 {
	a := address - mod_uart_start
	switch a {
	case 0:
		log("uart read command")
		return 0
	case 4:
		log("uart read status:", uart_status)
		return uart_status
	case 8:
		log("uart read receive:", uart_receive)
		return uart_receive
	case 12:
		log("uart read send:", uart_send)
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
		log("uart write command:", data)
		mod_uart_command(data)
	case 12:
		log("uart write send:", data)
		uart_send = data
	}
}

func mod_uart_command(d uint32) {
	switch d {
	case 1: // send
		uart_send_queue <- byte(uart_send)
	case 2: // clear ready bit
		uart_lock.Lock()
		uart_status = 0x01
		uart_lock.Unlock()
	}
}

func uart_listen() {
	ln, err := net.Listen("tcp", fmt.Sprintf(":%v", *uart_port))
	if err != nil {
		fmt.Println("could not bind to port:", *uart_port, err)
		return
	}
	for {
		conn, err := ln.Accept()
		if err != nil {
			fmt.Println("uart error accepting:", err)
		}
		done := make(chan bool)
		ack := make(chan bool)
		go func() {
			for {
				var d = make([]byte, 1)
				_, err := conn.Read(d)
				if err != nil {
					fmt.Println(err)
					ack <- true
					return
				}
				select {
				case <-done:
					ack <- true
					return
				default:
				}
				if uart_status == 0x03 {
					continue // dropped byte
				}
				uart_lock.Lock()
				uart_receive = uint32(d[0])
				uart_status = 0x03
				uart_lock.Unlock()
			}
		}()
		for {
			a := make([]byte, 1)
			a[0] = <-uart_send_queue
			_, err := conn.Write(a)
			if err != nil {
				break
			}
		}
		done <- true
		<-ack
	}
}
