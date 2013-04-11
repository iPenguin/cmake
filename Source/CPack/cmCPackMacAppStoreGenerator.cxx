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

//----------------------------------------------------------------------
const char* cmCPackMacAppStoreGenerator::GetPackagingInstallPrefix()
{
  this->InstallPrefix = "/";
  this->InstallPrefix += this->GetOption("CPACK_BUNDLE_NAME");
  this->InstallPrefix += ".app/Contents/Resources";

  return this->InstallPrefix.c_str();
}

const char* cmCPackMacAppStoreGenerator::GetOutputExtension()
{
    return ".pkg";
}

//----------------------------------------------------------------------
int cmCPackMacAppStoreGenerator::PackageFiles()
{

  // Get required arguments ...
  const std::string cpack_bundle_name = this->GetOption("CPACK_BUNDLE_NAME")
    ? this->GetOption("CPACK_BUNDLE_NAME") : "";
  if(cpack_bundle_name.empty())
    {
    cmCPackLogger(cmCPackLog::LOG_ERROR,
      "CPACK_BUNDLE_NAME must be set."
      << std::endl);

    return 0;
    }

  const std::string cpack_bundle_plist = this->GetOption("CPACK_BUNDLE_PLIST")
    ? this->GetOption("CPACK_BUNDLE_PLIST") : "";
  if(cpack_bundle_plist.empty())
    {
    cmCPackLogger(cmCPackLog::LOG_ERROR,
      "CPACK_BUNDLE_PLIST must be set."
      << std::endl);

    return 0;
    }

  const std::string cpack_bundle_icon = this->GetOption("CPACK_BUNDLE_ICON")
    ? this->GetOption("CPACK_BUNDLE_ICON") : "";
  if(cpack_bundle_icon.empty())
    {
    cmCPackLogger(cmCPackLog::LOG_ERROR,
      "CPACK_BUNDLE_ICON must be set."
      << std::endl);

    return 0;
    }

  // Get optional arguments ...
  const std::string cpack_bundle_startup_command =
    this->GetOption("CPACK_BUNDLE_STARTUP_COMMAND")
    ? this->GetOption("CPACK_BUNDLE_STARTUP_COMMAND") : "";

  // The staging directory contains everything that will end-up inside the
  // final disk image ...
  cmOStringStream staging;
  staging << toplevel;

  cmOStringStream contents;
  contents << staging.str() << "/" << cpack_bundle_name
    << ".app/" << "Contents";

  cmOStringStream application;
  application << contents.str() << "/" << "MacOS";

  cmOStringStream resources;
  resources << contents.str() << "/" << "Resources";

  // Install a required, user-provided bundle metadata file ...
  cmOStringStream plist_source;
  plist_source << cpack_bundle_plist;

  cmOStringStream plist_target;
  plist_target << contents.str() << "/" << "Info.plist";

  if(!this->CopyFile(plist_source, plist_target))
    {
    cmCPackLogger(cmCPackLog::LOG_ERROR,
      "Error copying plist.  Check the value of CPACK_BUNDLE_PLIST."
      << std::endl);

    return 0;
    }

  // Install a user-provided bundle icon ...
  cmOStringStream icon_source;
  icon_source << cpack_bundle_icon;

  cmOStringStream icon_target;
  icon_target << resources.str() << "/" << cpack_bundle_name << ".icns";

  if(!this->CopyFile(icon_source, icon_target))
    {
    cmCPackLogger(cmCPackLog::LOG_ERROR,
      "Error copying bundle icon.  Check the value of CPACK_BUNDLE_ICON."
      << std::endl);

    return 0;
    }

  // Optionally a user-provided startup command (could be an
  // executable or a script) ...
  if(!cpack_bundle_startup_command.empty())
    {
    cmOStringStream command_source;
    command_source << cpack_bundle_startup_command;

    cmOStringStream command_target;
    command_target << application.str() << "/" << cpack_bundle_name;

    if(!this->CopyFile(command_source, command_target))
      {
      cmCPackLogger(cmCPackLog::LOG_ERROR,
                    "Error copying startup command. "
                    " Check the value of CPACK_BUNDLE_STARTUP_COMMAND."
        << std::endl);

      return 0;
      }

    cmSystemTools::SetPermissions(command_target.str().c_str(), 0777);
    }

  if(!this->SignPackage(toplevel))
      return 0;

  return this->CreatePackage(toplevel);
}

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

bool cmCPackMacAppStoreGenerator::SupportsComponentInstallation() const
{
  return false;
}
