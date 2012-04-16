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
	return getImage(PLPFile)
}

func getOrg(f *tar.Reader) bool {
	var data = new(bytes.Buffer)
	_, err := io.Copy(data,f)
	if err != nil {
		fmt.Println("could not read metafile:", err)
		return false
	}
	lines := strings.Split(data.String(),"\n")
	org, _ = strconv.Atoi((strings.Split(lines[1],"="))[1])
	log("got org of:", org)
	return true
}

func getImage(f *tar.Reader) bool {
	var data = new(bytes.Buffer)
	_, err := io.Copy(data,f)
	if err != nil {
		fmt.Println("could not read image data:", err)
		return false
	}
	image = data.Bytes()
	log("read", len(image), "bytes of image")
	return true
}
