package main

import (
	"strconv"
	"fmt"
)

var watches_r []uint32
var watches_m []uint32

func watch_register(r string) {
	found := false
	for i,reg := range(registers) {
		n,ok := strconv.Atoi(r)
		if r == reg || (n == i) && ok == nil {
			// are we already watching?
			for j:=0; j<len(watches_r); j++ {
				if watches_r[j] == uint32(i) {
					fmt.Printf("register $%v already watched\n", r)
					return
				}
			}
			watches_r = append(watches_r, uint32(i))
			found = true
		}
	}
	if !found {
		fmt.Println("invalid register:", r)
	}
}

func watch_memory(m string) {
	a, ok := strToAddr(m)
	if !ok {
		return
	}
	// are we already watching?
	for i:=0; i<len(watches_m); i++ {
		if watches_m[i] == a {
			fmt.Printf("memory %v already watched\n", m)
			return
		}
	}
	watches_m = append(watches_m, a)
}

func print_watches() {
	for i:=0; i<len(watches_r); i++ {
		fmt.Printf("$%v/$%v : %#08x\n", watches_r[i], registers[int(watches_r[i])], rf[watches_r[i]])
	}
	for i:=0; i<len(watches_m); i++ {
		d, ok := cpu_read(watches_m[i])
		if ok {
			fmt.Printf("%#08x : %#08x\n", watches_m[i], d)
		}
	}
}

func watched_m(a uint32) {
	for i:=0; i<len(watches_m); i++ {
		if watches_m[i] == a {
			d, ok := cpu_read(a)
			if ok {
				fmt.Printf("watch: %#08x : %#08x\n", a, d)
			}
		}
	}
}

func watched_r(r int) {
	for i:=0; i<len(watches_r); i++ {
		if watches_r[i] == uint32(r) {
			fmt.Printf("watch: $%02d/$%v : %#08x\n", i, registers[int(r)], rf[r])
		}
	}
}
