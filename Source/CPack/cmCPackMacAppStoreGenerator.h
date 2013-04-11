/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2000-2009 Kitware, Inc.

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/

#ifndef cmCPackMacAppStoreGenerator_h
#define cmCPackMacAppStoreGenerator_h

#include "cmCPackBundleGenerator.h"

/** \class cmCPackMacAppStoreGenerator
 * \brief A generator for Mac App Store packages
 *
 */
class cmCPackMacAppStoreGenerator : public cmCPackBundleGenerator
{
public:
  cmCPackTypeMacro(cmCPackMacAppStoreGenerator, cmCPackDragNDropGenerator);

  cmCPackMacAppStoreGenerator();
  virtual ~cmCPackMacAppStoreGenerator();

protected:
  virtual int InitializeInternal();
  virtual const char* GetOutputExtension();
  int PackageFiles();

  int CreatePackage(const std::string& src_dir);
  std::string InstallPrefix;
};

#endif
