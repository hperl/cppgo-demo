package main

import (
	"fmt"
	"os"

	"github.com/hperl/cppgo-demo/ast"
)

func main() {
	if len(os.Args) != 2 {
		fmt.Printf("usage: %s source_file\n", os.Args[0])
		os.Exit(-1)
	}
	sourcePath := os.Args[1]

	src, err := os.Open(sourcePath)
	if err != nil {
		fmt.Printf("Could not open %s: %v\n", sourcePath, err)
	}

	functions := ast.Functions(src)

	if len(functions) == 0 {
		fmt.Println("\n\nNo functions found :(")
	}
	for _, fn := range functions {
		fmt.Printf("Found function %q from %d:%d to %d:%d\n",
			fn.Name,
			fn.SourceRange.Begin.Row, fn.SourceRange.Begin.Column,
			fn.SourceRange.End.Row, fn.SourceRange.End.Column,
		)
	}

	os.Exit(0)
}
