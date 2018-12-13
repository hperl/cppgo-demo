#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Tooling.h"
#include "ast/ast.pb.h"

extern "C" {
#include "functions.h"
}

using namespace clang;

class FindFunctionsVisitor : public RecursiveASTVisitor<FindFunctionsVisitor> {
 public:
  explicit FindFunctionsVisitor(ASTContext *Context,
                                ast::FunctionDecls *functionDecls)
      : Context(Context), functionDecls(functionDecls) {}

  bool TraverseFunctionDecl(clang::FunctionDecl *Declaration) {
    FullSourceLoc BeginLocation =
        Context->getFullLoc(Declaration->getBeginLoc());
    FullSourceLoc EndLocation = Context->getFullLoc(Declaration->getEndLoc());
    if (BeginLocation.isValid() && EndLocation.isValid()) {
      auto functionDecl = functionDecls->add_function_decl();
      functionDecl->set_name(Declaration->getNameAsString());

      auto sourceRange = functionDecl->mutable_source_range();

      auto beginLocation = sourceRange->mutable_begin();
      beginLocation->set_row(BeginLocation.getSpellingLineNumber());
      beginLocation->set_column(BeginLocation.getSpellingColumnNumber());

      auto endLocation = sourceRange->mutable_end();
      endLocation->set_row(EndLocation.getSpellingLineNumber());
      endLocation->set_column(EndLocation.getSpellingColumnNumber());
    } else {
      llvm::errs() << "Location was invalid\n";
    }
    return true;
  }

 private:
  ASTContext *Context;
  ast::FunctionDecls *functionDecls;
};

class FindFunctionsConsumer : public clang::ASTConsumer {
 public:
  explicit FindFunctionsConsumer(ASTContext *Context,
                                 ast::FunctionDecls *functionDecls)
      : Visitor(Context, functionDecls) {}

  void HandleTranslationUnit(clang::ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

 private:
  FindFunctionsVisitor Visitor;
};

class FindFunctionsAction : public clang::ASTFrontendAction {
 public:
  FindFunctionsAction(ast::FunctionDecls *functionDecls)
      : functionDecls(functionDecls) {}

  std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
      clang::CompilerInstance &Compiler, llvm::StringRef InFile) override {
    return std::unique_ptr<clang::ASTConsumer>(
        new FindFunctionsConsumer(&Compiler.getASTContext(), functionDecls));
  }

 private:
  ast::FunctionDecls *functionDecls;
};

extern "C" Buffer *findFunctions(char *code) {
  ast::FunctionDecls functionDecls;

  clang::tooling::runToolOnCode(new FindFunctionsAction(&functionDecls), code);

  Buffer *buf = (Buffer *)malloc(sizeof(Buffer));
  buf->size = functionDecls.ByteSizeLong();
  buf->data = malloc(buf->size);
  functionDecls.SerializeToArray(buf->data, buf->size);
  return buf;
}

extern "C" void Buffer_free(Buffer *buf) {
  free(buf->data);
  free(buf);
}
