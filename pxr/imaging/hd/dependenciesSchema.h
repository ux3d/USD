//
// Copyright 2021 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
////////////////////////////////////////////////////////////////////////
// This file is generated by a script.  Do not edit directly.  Edit the
// schema.template.h file to make changes.

#ifndef PXR_IMAGING_HD_DEPENDENCIES_SCHEMA_H
#define PXR_IMAGING_HD_DEPENDENCIES_SCHEMA_H

#include "pxr/imaging/hd/api.h"

#include "pxr/imaging/hd/dependencySchema.h"


PXR_NAMESPACE_OPEN_SCOPE

//-----------------------------------------------------------------------------

#define HDDEPENDENCIES_SCHEMA_TOKENS \
    (__dependencies) \

TF_DECLARE_PUBLIC_TOKENS(HdDependenciesSchemaTokens, HD_API,
    HDDEPENDENCIES_SCHEMA_TOKENS);

//-----------------------------------------------------------------------------

class HdDependenciesSchema : public HdSchema
{
public:
    HdDependenciesSchema(HdContainerDataSourceHandle container)
    : HdSchema(container) {}

    using EntryPair = std::pair<TfToken, HdDependencySchema>;
    using EntryVector = TfSmallVector<EntryPair, 8>;

    EntryVector GetEntries();


    /// Retrieves a container data source with the schema's default name token
    /// "__dependencies" from the parent container and constructs a
    /// HdDependenciesSchema instance.
    /// Because the requested container data source may not exist, the result
    /// should be checked with IsDefined() or a bool comparison before use.
    HD_API
    static HdDependenciesSchema GetFromParent(
        const HdContainerDataSourceHandle &fromParentContainer);

    /// Returns an HdDataSourceLocator (relative to the prim-level data source)
    /// where the container representing this schema is found by default.
    HD_API
    static const HdDataSourceLocator &GetDefaultLocator();

};

PXR_NAMESPACE_CLOSE_SCOPE

#endif