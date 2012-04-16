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
	org int
	image []byte
)

func newPLPFile(file string) {
	log("attempting to open:", file)

	f, err := os.Open(file)
	if err != nil {
		fmt.Println("could not open:", err)
		return
	}

	PLPFile = tar.NewReader(f)
	hdr, err := PLPFile.Next()
	if err != nil {
		fmt.Println("error with plp file:", err)
		return
	}
	if hdr.Name != "plp.metafile" { // metafile should be first
		fmt.Println("error with plpfile:", err)
		return
	}
	getOrg(PLPFile)

	for {
		hdr, err = PLPFile.Next()
		if err != nil {
			fmt.Println("could not find image, perhaps file is not assembled?")
			return
		} else if hdr.Name == "plp.image" {
			break
		}
	}
	getImage(PLPFile)
}

func getOrg(f *tar.Reader) {
	var data = new(bytes.Buffer)
	_, err := io.Copy(data,f)
	if err != nil {
		fmt.Println("could not read metafile:", err)
		return
	}
	lines := strings.Split(data.String(),"\n")
	org, _ = strconv.Atoi((strings.Split(lines[1],"="))[1])
	log("got org of:", org)
}

func getImage(f *tar.Reader) {
	var data = new(bytes.Buffer)
	_, err := io.Copy(data,f)
	if err != nil {
		fmt.Println("could not read image data:", err)
		return
	}
	image = data.Bytes()
	log("read", len(image), "bytes of image")
}
