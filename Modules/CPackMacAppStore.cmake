##section Variables specific to CPack MacAppStore generator
##end
##module
# - CPack Bundle generator (Mac OS X) specific options
#
# Installers built on Mac OS X using the MacAppStore generator use the
# aforementioned DragNDrop (CPACK_DMG_xxx) variables, plus the Bundle-specific
# parameters (CPACK_BUNDLE_xxx), and the following variables as well.
##end
#
##variable
#  CPACK_COMMAND_PRODUCTBUILD - Path to the productbuild(1) command used to
#  create a package for the OS X Installer or the Mac App Store. This variable
#  can be used to override the automatically detected command (or specify its
#  location if the auto-detection fails to find it.)
#end

#=============================================================================
# Copyright 2006-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

#Bundle Generator specific code should be put here
