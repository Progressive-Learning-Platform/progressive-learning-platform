/* 
 * a go implementation of a plp cpu simulator
 *
 * fritz 2012
 *
 */

package main

import (
	"flag"
	"fmt"
	"os"
	"runtime/pprof"
)

const version = "1.0"

var (
	help    = flag.Bool("h", false, "print this help")
	plpfile = flag.String("plp", "", "plp file to open")
	debug   = flag.Bool("v", false, "print extra information")
	prof    = flag.Bool("p", false, "output pprof info to goplp.prof")
)

func log(arg ...interface{}) {
	if *debug {
		fmt.Println(arg)
	}
}

func usage() {
	fmt.Println("usage: goplp <options>")
	flag.PrintDefaults()
	os.Exit(1)
}

func init() {
	flag.Usage = usage
	flag.Parse()

	if *help {
		usage()
	}

	fmt.Println("goplp", version)
	fmt.Println("goplp  Copyright (C) 2012  David Fritz")
	fmt.Println("This program comes with ABSOLUTELY NO WARRANTY;")
	fmt.Println("This is free software, and you are welcome to redistribute it")
	fmt.Println("under certain conditions;")
}

func main() {
	if *plpfile != "" {
		newPLPFile(*plpfile)
	}
	var c Console
	c.Init(os.Stdin)
	if *prof {
		f, err := os.Create("goplp.prof")
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		pprof.StartCPUProfile(f)
		defer pprof.StopCPUProfile()
	}
	c.Run()
}
