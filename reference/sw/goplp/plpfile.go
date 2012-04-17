package main

import (
	"archive/tar"
	"os"
	"fmt"
	"bytes"
	"io"
	"strings"
	"strconv"
)

var (
	PLPFile *tar.Reader = nil
	org uint32
	image []uint32
)

func newPLPFile(file string) bool {
	log("attempting to open:", file)

	f, err := os.Open(file)
	if err != nil {
		fmt.Println("could not open:", err)
		return false
	}

	PLPFile = tar.NewReader(f)
	hdr, err := PLPFile.Next()
	if err != nil {
		fmt.Println("error with plp file:", err)
		return false
	}
	if hdr.Name != "plp.metafile" { // metafile should be first
		fmt.Println("error with plpfile:", err)
		return false
	}
	if !getOrg(PLPFile) {
		return false
	}

	for {
		hdr, err = PLPFile.Next()
		if err != nil {
			fmt.Println("could not find image, perhaps file is not assembled?")
			return false
		} else if hdr.Name == "plp.image" {
			break
		}
	}
	if !getImage(PLPFile) {
		return false
	}
	copyImage()
	return true
}

func getOrg(f *tar.Reader) bool {
	var data = new(bytes.Buffer)
	_, err := io.Copy(data,f)
	if err != nil {
		fmt.Println("could not read metafile:", err)
		return false
	}
	lines := strings.Split(data.String(),"\n")
	o, _ := strconv.Atoi((strings.Split(lines[1],"="))[1])
	org = uint32(o)
	log(fmt.Sprintf("got org of: %#08x", org))
	return true
}

func getImage(f *tar.Reader) bool {
	var data = new(bytes.Buffer)
	_, err := io.Copy(data,f)
	if err != nil {
		fmt.Println("could not read image data:", err)
		return false
	}
	b := data.Bytes()
	for i := 0; i < len(b)/4; i+=4 {
		image = append(image, (uint32(b[i])<<24) | (uint32(b[i+1])<<16) | (uint32(b[i+2])<<8) | uint32(b[i+3]))
		log(fmt.Sprintf("added to image: %#08x", image[len(image)-1]))
	}
	log("read", len(image), "words of image")
	return true
}

func copyImage() {
	// starting from the org, write each part of image to the cpu
	var i uint32
	for i=0; i<uint32(len(image)); i++ {
		cpu_write(org + i*4, image[i])
	}
	// update the pc with the org
	pc = org
}
