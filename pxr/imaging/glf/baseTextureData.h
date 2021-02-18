//
// Copyright 2016 Pixar
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
#ifndef PXR_IMAGING_GLF_BASE_TEXTURE_DATA_H
#define PXR_IMAGING_GLF_BASE_TEXTURE_DATA_H

#include "pxr/pxr.h"
#include "pxr/imaging/glf/api.h"
#include "pxr/imaging/hio/image.h"
#include "pxr/base/tf/declarePtrs.h"
#include "pxr/base/tf/refBase.h"
#include "pxr/base/tf/weakBase.h"

PXR_NAMESPACE_OPEN_SCOPE

TF_DECLARE_WEAK_AND_REF_PTRS(GlfBaseTextureData);

class GlfBaseTextureData : public TfRefBase,
                           public TfWeakBase
{
public:
    GlfBaseTextureData() = default;

    GLF_API
    ~GlfBaseTextureData() override;

    // Disallow copies
    GlfBaseTextureData(const GlfBaseTextureData&) = delete;
    GlfBaseTextureData& operator=(const GlfBaseTextureData&) = delete;

    struct WrapInfo {
        WrapInfo()
          : wrapModeS{false, HioAddressModeRepeat}
          , wrapModeT{false, HioAddressModeRepeat}
          , wrapModeR{false, HioAddressModeRepeat}
        {}

        std::pair<bool, HioAddressMode> wrapModeS;
        std::pair<bool, HioAddressMode> wrapModeT;
        std::pair<bool, HioAddressMode> wrapModeR;
    };

    /// Is this a 1-, 2- or 3-dimensional texture.
    virtual int NumDimensions() const = 0;

    virtual int ResizedWidth(int mipLevel = 0) const = 0;

    virtual int ResizedHeight(int mipLevel = 0) const = 0;

    virtual int ResizedDepth(int mipLevel = 0) const = 0;

    virtual HioFormat GetFormat() const = 0;

    virtual size_t TargetMemory() const = 0;

    virtual WrapInfo GetWrapInfo() const = 0;

    virtual size_t ComputeBytesUsed() const = 0;

    virtual size_t ComputeBytesUsedByMip(int mipLevel = 0) const = 0;

    virtual bool Read(int degradeLevel, 
                      bool generateMipmap, 
                      HioImage::ImageOriginLocation originLocation = 
                                                 HioImage::OriginUpperLeft) = 0;
    
    virtual bool HasRawBuffer(int mipLevel = 0) const = 0;

    virtual unsigned char * GetRawBuffer(int mipLevel = 0) const = 0;   

    virtual int GetNumMipLevels() const = 0;

    virtual bool IsCompressed() const {
        return HioIsCompressed(GetFormat());
    }

};


PXR_NAMESPACE_CLOSE_SCOPE

#endif // GLF_BASETEXTURE_DATA
