/*
 * This file is part of the LibreOffice project.
 *
 * Based on LLVM/Clang.
 *
 * This file is distributed under the University of Illinois Open Source
 * License. See LICENSE.TXT for details.
 *
 */

#ifndef TUTORIAL1_H
#define TUTORIAL1_H

#include "plugin.hxx"

namespace loplugin
{

// The class implementing the plugin action.
class Tutorial1
    // Inherits from the Clang class that will allow examing the Clang AST tree (i.e. syntax tree).
    : public RecursiveASTVisitor< Tutorial1 >
    // And the base class for LO Clang plugins.
    , public Plugin
    {
    public:
        // Ctor, nothing special.
        Tutorial1( ASTContext& context );
        // The function that will be called to perform the actual action.
        virtual void run();
        // Function from Clang, it will be called for every return statement in the source.
        bool VisitReturnStmt( ReturnStmt* returnstmt );
    };

} // namespace

#endif // POSTFIXINCREMENTFIX_H

