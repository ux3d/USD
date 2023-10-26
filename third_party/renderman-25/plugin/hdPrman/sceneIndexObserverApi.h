//
// Copyright 2023 Pixar
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
#ifndef EXT_RMANPKG_25_0_PLUGIN_RENDERMAN_PLUGIN_HD_PRMAN_SCENE_INDEX_OBSERVER_API_H
#define EXT_RMANPKG_25_0_PLUGIN_RENDERMAN_PLUGIN_HD_PRMAN_SCENE_INDEX_OBSERVER_API_H

#include "pxr/imaging/hd/version.h"

// There was no hdsi/version.h before this HD_API_VERSION.
#if HD_API_VERSION >= 58 

#include "pxr/imaging/hdsi/version.h"

// HDPRMAN_USE_SCENE_INDEX_OBSERVER controls whether hdPrman uses the new
// HdsiPrimManagingSceneIndexObserver and other new API for the implementation
// as a scene index observer.
//
// We can only use for late enough versions of USD.
//
#if HDSI_API_VERSION >= 11
#define HDPRMAN_USE_SCENE_INDEX_OBSERVER
#endif

#endif // #if HD_API_VERSION >= 58

#endif // EXT_RMANPKG_25_0_PLUGIN_RENDERMAN_PLUGIN_HD_PRMAN_SCENE_INDEX_OBSERVER_API_H
