/*************************************************************************
*
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* Copyright 2000, 2010 Oracle and/or its affiliates.
*
* OpenOffice.org - a multi-platform office productivity suite
*
* This file is part of OpenOffice.org.
*
* OpenOffice.org is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License version 3
* only, as published by the Free Software Foundation.
*
* OpenOffice.org is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License version 3 for more details
* (a copy is included in the LICENSE file that accompanied this code).
*
* You should have received a copy of the GNU Lesser General Public License
* version 3 along with OpenOffice.org.  If not, see
* <http://www.openoffice.org/license.html>
* for a copy of the LGPLv3 License.
*
************************************************************************/

#include "precompiled_configmgr.hxx"
#include "sal/config.h"

#include "rtl/ref.hxx"
#include "rtl/ustring.hxx"

#include "data.hxx"
#include "groupnode.hxx"
#include "node.hxx"
#include "nodemap.hxx"

namespace configmgr {

GroupNode::GroupNode(
    int layer, bool extensible, rtl::OUString const & templateName):
    Node(layer), extensible_(extensible), templateName_(templateName),
    mandatory_(Data::NO_LAYER)
{}

rtl::Reference< Node > GroupNode::clone() const {
    return new GroupNode(*this);
}

NodeMap & GroupNode::getMembers() {
    return members_;
}

rtl::OUString GroupNode::getTemplateName() const {
    return templateName_;
}

void GroupNode::setMandatory(int layer) {
    mandatory_ = layer;
}

int GroupNode::getMandatory() const {
    return mandatory_;
}

bool GroupNode::isExtensible() const {
    return extensible_;
}

GroupNode::GroupNode(GroupNode const & other):
    Node(other), extensible_(other.extensible_),
    templateName_(other.templateName_), mandatory_(other.mandatory_)
{
    cloneNodeMap(other.members_, &members_);
}

GroupNode::~GroupNode() {}

Node::Kind GroupNode::kind() const {
    return KIND_GROUP;
}

void GroupNode::clear() {
    members_.clear();
}

}
