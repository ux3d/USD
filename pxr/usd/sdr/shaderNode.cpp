//
// Copyright 2018 Pixar
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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//

#include "pxr/pxr.h"
#include "pxr/base/tf/refPtr.h"
#include "pxr/usd/ndr/debugCodes.h"
#include "pxr/usd/sdr/shaderMetadataHelpers.h"
#include "pxr/usd/sdr/shaderNode.h"
#include "pxr/usd/sdr/shaderProperty.h"

#include <algorithm>
#include <unordered_set>

PXR_NAMESPACE_OPEN_SCOPE

using ShaderMetadataHelpers::StringVal;
using ShaderMetadataHelpers::StringVecVal;
using ShaderMetadataHelpers::TokenVal;
using ShaderMetadataHelpers::TokenVecVal;
using ShaderMetadataHelpers::IntVal;

TF_DEFINE_PUBLIC_TOKENS(SdrNodeMetadata, SDR_NODE_METADATA_TOKENS);
TF_DEFINE_PUBLIC_TOKENS(SdrNodeContext, SDR_NODE_CONTEXT_TOKENS);
TF_DEFINE_PUBLIC_TOKENS(SdrNodeRole, SDR_NODE_ROLE_TOKENS);

SdrShaderNode::SdrShaderNode(
    const NdrIdentifier& identifier,
    const NdrVersion& version,
    const std::string& name,
    const TfToken& family,
    const TfToken& context,
    const TfToken& sourceType,
    const std::string& definitionURI,
    const std::string& implementationURI,
    NdrPropertyUniquePtrVec&& properties,
    const NdrTokenMap& metadata,
    const std::string &sourceCode)
    : NdrNode(identifier, version, name, family,
              context, sourceType, definitionURI, implementationURI,
              std::move(properties), metadata, sourceCode)
{
    // Cast inputs to shader inputs
    for (const auto& input : _inputs) {
        _shaderInputs[input.first] =
            dynamic_cast<SdrShaderPropertyConstPtr>(input.second);
    }

    // ... and the same for outputs
    for (const auto& output : _outputs) {
        _shaderOutputs[output.first] =
            dynamic_cast<SdrShaderPropertyConstPtr>(output.second);
    }

    _InitializePrimvars();
    _PostProcessProperties();

    // Tokenize metadata
    _label = TokenVal(SdrNodeMetadata->Label, _metadata);
    _category = TokenVal(SdrNodeMetadata->Category, _metadata);
    _departments = TokenVecVal(SdrNodeMetadata->Departments, _metadata);
    _pages = _ComputePages();
}

void
SdrShaderNode::_PostProcessProperties()
{
    // See if this shader node has been tagged with an explict USD encoding
    // version, which affects how properties manifest in USD files. We propagate
    // this metadatum to the individual properties, since the encoding is
    // controlled there in the GetTypeAsSdfType method.
    static const int DEFAULT_ENCODING = -1;
    int usdEncodingVersion =
        IntVal(SdrNodeMetadata->SdrUsdEncodingVersion, _metadata,
               DEFAULT_ENCODING);

    const NdrTokenVec vsNames = GetAllVstructNames();

    for (const NdrPropertyUniquePtr& property : _properties) {
        SdrShaderPropertyConstPtr constShaderProperty =
            dynamic_cast<SdrShaderPropertyConstPtr>(property.get());
        // This function, and only this function, has special permission (is a
        // friend function of SdrProperty) to call private methods and so we
        // need a non-const pointer.
        SdrShaderProperty* shaderProperty =
            const_cast<SdrShaderProperty*>(constShaderProperty);

        if (usdEncodingVersion != DEFAULT_ENCODING) {
            shaderProperty->_SetUsdEncodingVersion(usdEncodingVersion);
        }

        bool isVStruct = std::find(vsNames.begin(), vsNames.end(),
                                   shaderProperty->GetName()) != vsNames.end();
        if (isVStruct) {
            shaderProperty->_ConvertToVStruct();
        }

        // There must not be any further modifications of this property after
        // this method has been called.
        shaderProperty->_FinalizeProperty();
    }
}

SdrShaderPropertyConstPtr
SdrShaderNode::GetShaderInput(const TfToken& inputName) const
{
    return dynamic_cast<SdrShaderPropertyConstPtr>(
        NdrNode::GetInput(inputName)
    );
}

SdrShaderPropertyConstPtr
SdrShaderNode::GetShaderOutput(const TfToken& outputName) const
{
    return dynamic_cast<SdrShaderPropertyConstPtr>(
        NdrNode::GetOutput(outputName)
    );
}

NdrTokenVec
SdrShaderNode::GetAssetIdentifierInputNames() const
{
    NdrTokenVec result;
    for (const auto &inputName : GetInputNames()) {
        if (auto input = GetShaderInput(inputName)) {
            if (input->IsAssetIdentifier()) {
                result.push_back(input->GetName());
            }
        }
    }
    return result;
}

SdrShaderPropertyConstPtr 
SdrShaderNode::GetDefaultInput() const
{
    std::vector<SdrShaderPropertyConstPtr> result;
    for (const auto &inputName : GetInputNames()) {
        if (auto input = GetShaderInput(inputName)) {
            if (input->IsDefaultInput()) {
                return input;
            }
        }
    }
    return nullptr;
}

std::string
SdrShaderNode::GetHelp() const
{
    return StringVal(SdrNodeMetadata->Help, _metadata);
}

std::string
SdrShaderNode::GetImplementationName() const
{
    return StringVal(SdrNodeMetadata->ImplementationName, _metadata, GetName());
}

std::string
SdrShaderNode::GetRole() const
{
    return StringVal(SdrNodeMetadata->Role, _metadata, GetName());
}

NdrTokenVec
SdrShaderNode::GetPropertyNamesForPage(const std::string& pageName) const
{
    NdrTokenVec propertyNames;

    for (const NdrPropertyUniquePtr& property : _properties) {
        const SdrShaderPropertyConstPtr shaderProperty =
            dynamic_cast<const SdrShaderPropertyConstPtr>(property.get());

        if (shaderProperty->GetPage() == pageName) {
            propertyNames.push_back(shaderProperty->GetName());
        }
    }

    return propertyNames;
}

NdrTokenVec
SdrShaderNode::GetAllVstructNames() const
{
    std::unordered_set<std::string> vstructs;

    auto hasVstructMetadata = [] (const SdrShaderPropertyConstPtr& property) {
        const NdrTokenMap& metadata = property->GetMetadata();
        const auto t = metadata.find(SdrPropertyMetadata->Tag);
        return (t != metadata.end() && t->second == "vstruct");
    };

    for (const auto& input : _shaderInputs) {

        if (hasVstructMetadata(input.second)) {
            vstructs.insert(input.first);
            continue;
        }

        if (!input.second->IsVStructMember()) {
            continue;
        }

        const TfToken& head = input.second->GetVStructMemberOf();

        if (_shaderInputs.count(head)) {
            vstructs.insert(head);
        }
    }

    for (const auto& output : _shaderOutputs) {

        if (hasVstructMetadata(output.second)) {
            vstructs.insert(output.first);
            continue;
        }

        if (!output.second->IsVStructMember()) {
            continue;
        }

        const TfToken& head = output.second->GetVStructMemberOf();

        if (_shaderOutputs.count(head)) {
            vstructs.insert(head);
        }
    }

    // Transform the set into a vector
    return NdrTokenVec(vstructs.begin(), vstructs.end());
}

void
SdrShaderNode::_InitializePrimvars()
{
    NdrTokenVec primvars;
    NdrTokenVec primvarNamingProperties;

    // The "raw" list of primvars contains both ordinary primvars, and the names
    // of properties whose values contain additional primvar names
    const NdrStringVec rawPrimvars =
        StringVecVal(SdrNodeMetadata->Primvars, _metadata);

    for (const std::string& primvar : rawPrimvars) {
        if (TfStringStartsWith(primvar, "$")) {
            const std::string propertyName = TfStringTrimLeft(primvar, "$");
            const SdrShaderPropertyConstPtr input =
                GetShaderInput(TfToken(propertyName));

            if (input && (input->GetType() == SdrPropertyTypes->String)) {
                primvarNamingProperties.emplace_back(
                    TfToken(std::move(propertyName))
                );
            } else {
                TF_DEBUG(NDR_PARSING).Msg(
                    "Found a node [%s] whose metadata "
                    "indicates a primvar naming property [%s] "
                    "but the property's type is not string; ignoring.",  
                    GetName().c_str(), primvar.c_str());
            }
        } else {
            primvars.emplace_back(TfToken(primvar));
        }
    }

    _primvars = primvars;
    _primvarNamingProperties = primvarNamingProperties;
}

NdrTokenVec
SdrShaderNode::_ComputePages() const
{
    NdrTokenVec pages;

    for (const NdrPropertyUniquePtr& property : _properties) {
        auto sdrProperty = static_cast<SdrShaderPropertyPtr>(property.get());
        const TfToken& page = sdrProperty->GetPage();

        // Exclude duplicate pages
        if (std::find(pages.begin(), pages.end(), page) != pages.end()) {
            continue;
        }

        pages.emplace_back(std::move(page));
    }

    return pages;
}

PXR_NAMESPACE_CLOSE_SCOPE
