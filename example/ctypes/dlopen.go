package main

// #cgo LDFLAGS: -ldl
// #include <dlfcn.h>
// #include <stdlib.h>
//
// typedef int32_t (*oftr_call)(uint32_t opcode, const char *input,
//                                size_t input_len, char *output,
//                                size_t output_len);
//
// int32_t oftr_version(void *fn, char *out, size_t outlen) {
//   return ((oftr_call)fn)(0, NULL, 0, out, outlen);
// }
import "C"

import "unsafe"
import "fmt"
import "os"
import "regexp"

// Get oftr version by using `oftr_call` API.
func get_version(oftr_call unsafe.Pointer) string {
	var buf [1024]uint8
	out := (*C.char)(unsafe.Pointer(&buf[0]))
	outlen := C.size_t(cap(buf))
	r := int32(C.oftr_version(oftr_call, out, outlen))
	return string(buf[:r])
}

func main() {
	prog := "/usr/local/bin/oftr"
	if len(os.Args) == 2 {
		prog = os.Args[1]
	}

	name := C.CString(prog)
	handle := C.dlopen(name, C.RTLD_LAZY)
	if handle == nil {
		panic(fmt.Sprintf("Unable to dlopen: %s", prog))
	}
	C.free(unsafe.Pointer(name))
	defer C.dlclose(handle)

	sym := C.CString("oftr_call")
	oftr_call := C.dlsym(handle, sym)
	if oftr_call == nil {
		panic("Unable to find oftr_call symbol")
	}
	C.free(unsafe.Pointer(sym))

	version := get_version(oftr_call)

	// Check version result against regular expression.
	re := regexp.MustCompile(`^[0-9]+(\.[0-9]+){2} \([0-9a-f]+\) *<https://github.com/byllyfish/oftr>$`)
	if !re.MatchString(version) {
		fmt.Printf("'%s' does not match '%s'\n", version, re)
		os.Exit(1)
	}

	fmt.Println(version)
}
