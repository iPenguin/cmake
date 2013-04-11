/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2000-2009 Kitware, Inc., Insight Software Consortium

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/

#include "cmCPackMacAppStoreGenerator.h"
#include "cmCPackLog.h"
#include "cmSystemTools.h"

#include <cmsys/RegularExpression.hxx>

//----------------------------------------------------------------------
cmCPackMacAppStoreGenerator::cmCPackMacAppStoreGenerator()
{
}

//----------------------------------------------------------------------
cmCPackMacAppStoreGenerator::~cmCPackMacAppStoreGenerator()
{
}

//----------------------------------------------------------------------
int cmCPackMacAppStoreGenerator::InitializeInternal()
{

  const std::string productbuild_path = cmSystemTools::FindProgram(
    "productbuild", std::vector<std::string>(), false);
  if(productbuild_path.empty())
    {
    cmCPackLogger(cmCPackLog::LOG_ERROR,
      "Cannot locate productbuild command"
      << std::endl);
    return 0;
    }
  this->SetOptionIfNotSet("CPACK_COMMAND_PRODUCTBUILD",
    productbuild_path.c_str());

  return this->Superclass::InitializeInternal();
}

const char* cmCPackMacAppStoreGenerator::GetOutputExtension()
{
    return ".pkg";
}

//----------------------------------------------------------------------
int cmCPackMacAppStoreGenerator::PackageFiles()
{
  if(!this->ConstructAppBundle())
    {
    return 0;
    }

  if(!this->SignPackage(toplevel))
    {
    return 0;
    }

  return this->CreatePackage(toplevel);
}

//----------------------------------------------------------------------
int cmCPackMacAppStoreGenerator::CreatePackage(const std::string &src_dir)
{

  const std::string cpack_apple_cert_installer =
    this->GetOption("CPACK_APPLE_CERT_INSTALLER")
    ? this->GetOption("CPACK_APPLE_CERT_INSTALLER") : "";

  //Optionally codesign the application.
  if(!cpack_apple_cert_installer.empty())
    {
    std::string bundle_path;
    bundle_path = src_dir + "/";
    bundle_path += this->GetOption("CPACK_BUNDLE_NAME");
    bundle_path += ".app";

    cmOStringStream temp_build_pkg;
    temp_build_pkg << this->GetOption("CPACK_COMMAND_PRODUCTBUILD");
    temp_build_pkg << " --component \"" << bundle_path;
    temp_build_pkg << "\" /Applications --sign \"";
    temp_build_pkg << this->GetOption("CPACK_APPLE_CERT_INSTALLER");
    temp_build_pkg << "\" --product \"";
    temp_build_pkg << bundle_path << "/Contents/Info.plist\" \"";
    temp_build_pkg << this->GetOption("CPACK_TEMPORARY_PACKAGE_FILE_NAME");
    temp_build_pkg << "\"";

    if(!this->RunCommand(temp_build_pkg))
      {
      cmCPackLogger(cmCPackLog::LOG_ERROR,
        "Error building package:"
        << this->GetOption("CPACK_TOPLEVEL_DIRECTORY")
        << "/" << this->GetOption("CPACK_BUNDLE_NAME")
        << this->GetOutputExtension()
        << std::endl);

      return 0;
      }
    }

    return 1;
}
