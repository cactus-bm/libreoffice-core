/*
 * This file is part of the LibreOffice project.
 *
 * Based on LLVM/Clang.
 *
 * This file is distributed under the University of Illinois Open Source
 * License. See LICENSE.TXT for details.
 *
 */

#ifndef PLUGIN_H
#define PLUGIN_H

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Rewrite/Rewriter.h>

using namespace clang;
using namespace llvm;
using namespace std;

namespace loplugin
{

class Plugin
    {
    public:
        explicit Plugin( ASTContext& context );
    protected:
        DiagnosticBuilder report( DiagnosticsEngine::Level level, StringRef message, SourceLocation loc = SourceLocation());
        bool ignoreLocation( SourceLocation loc );
        bool ignoreLocation( const Decl* decl );
        bool ignoreLocation( const Stmt* stmt );
        ASTContext& context;
    };

class RewritePlugin
    : public Plugin
    {
    public:
        explicit RewritePlugin( ASTContext& context, Rewriter& rewriter );
    protected:
        Rewriter& rewriter;
    };

inline
bool Plugin::ignoreLocation( const Decl* decl )
    {
    return ignoreLocation( decl->getLocation());
    }

inline
bool Plugin::ignoreLocation( const Stmt* stmt )
    {
    return ignoreLocation( stmt->getLocStart());
    }

} // namespace

#endif // COMPILEPLUGIN_H
