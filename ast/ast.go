package ast

// #cgo LDFLAGS: -lclangBasic -lclangFrontend -lclangTooling -lclangAST -lLLVM-7
// #cgo CPPFLAGS: -std=c++11 -fno-rtti -DGOOGLE_PROTOBUF_NO_RTTI
// #include <stdlib.h>
// #include "functions.h"
import "C"
import (
	"io"
	"io/ioutil"
	"log"
	"unsafe"

	"github.com/golang/protobuf/proto"
)

// Functions returns the function declarations in the code.
func Functions(code io.Reader) []*FunctionDecl {
	codeBytes, err := ioutil.ReadAll(code)
	if err != nil {
		panic(err)
	}

	cCodeBytes := C.CString(string(codeBytes))
	defer C.free(unsafe.Pointer(cCodeBytes))

	buffer := C.findFunctions(cCodeBytes)
	defer C.Buffer_free(buffer)
	log.Println("size: ", buffer.size)
	msg := C.GoBytes(unsafe.Pointer(buffer.data), C.int(buffer.size))

	funs := &FunctionDecls{}
	if err := proto.Unmarshal(msg, funs); err != nil {
		log.Fatal(err)
	}
	return funs.FunctionDecl
}
