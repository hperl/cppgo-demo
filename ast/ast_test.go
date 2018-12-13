package ast

import (
	"os"
	"reflect"
	"testing"

	"github.com/bazelbuild/rules_go/go/tools/bazel"
)

func TestFunctions(t *testing.T) {
	pth, err := bazel.Runfile("testdata/one_function.c")
	if err != nil {
		t.Fatal(err)
	}
	f, err := os.Open(pth)
	if err != nil {
		t.Fatal(err)
	}
	defer f.Close()
	expected := []*FunctionDecl{{
		Name: "fun",
		SourceRange: &SourceRange{
			Begin: &SourceLocation{
				Row:    3,
				Column: 1,
			},
			End: &SourceLocation{
				Row:    6,
				Column: 1,
			},
		},
	}}
	fns := Functions(f)
	if !reflect.DeepEqual(fns, expected) {
		t.Errorf("\ngot  %+v\nwant %+v", fns, expected)
	}
}
