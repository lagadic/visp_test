#############################################################################
#
# $Id: CTestConfig.cmake,v 1.9 2008-12-11 13:19:44 fspindle Exp $
#
# This file is part of the ViSP software.
# Copyright (C) 2005 - 2014 by INRIA. All rights reserved.
# 
# This software is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# ("GPL") version 2 as published by the Free Software Foundation.
# See the file LICENSE.txt at the root directory of this source
# distribution for additional information about the GNU GPL.
#
# For using ViSP with software that can not be combined with the GNU
# GPL, please contact INRIA about acquiring a ViSP Professional 
# Edition License.
#
# See http://www.irisa.fr/lagadic/visp/visp.html for more information.
# 
# This software was developed at:
# INRIA Rennes - Bretagne Atlantique
# Campus Universitaire de Beaulieu
# 35042 Rennes Cedex
# France
# http://www.irisa.fr/lagadic
#
# If you have questions regarding the use of this file, please contact
# INRIA at visp@inria.fr
# 
# This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
# WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#
# Description:
# CDash configuration.
#
# Authors:
# Fabien Spindler
#
#############################################################################

set(CTEST_PROJECT_NAME "ViSP")
set(CTEST_NIGHTLY_START_TIME "00:00:00 GMT")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "cdash.inria.fr")
set(CTEST_DROP_LOCATION "/CDash/submit.php?project=ViSP-3")
set(CTEST_DROP_SITE_CDASH TRUE)

#--------------------------------------------------------------------
# BUILNAME variable construction
# This variable will be used to set the build name which will appear 
# on the ViSP dashboard http://cdash.irisa.fr/CDash/
#--------------------------------------------------------------------
# Start with the short system name, e.g. "Linux", "FreeBSD" or "Windows"
if(BUILDNAME)
  set(BUILDNAME "${BUILDNAME}-${CMAKE_SYSTEM_NAME}")
else(BUILDNAME)
  # To suppress the first space if BUILDNAME is not set
  set(BUILDNAME "${CMAKE_SYSTEM_NAME}")
endif(BUILDNAME)

# Add i386 or amd64
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(BUILDNAME "${BUILDNAME}-amd64")
else()
  set(BUILDNAME "${BUILDNAME}-i386")
endif()

# Add the compiler name, e.g. "g++, msvc7..."
if(MSVC70)
  set(BUILDNAME "${BUILDNAME}-msvc70")
elseif(MSVC71)
  set(BUILDNAME "${BUILDNAME}-msvc71")
elseif(MSVC80)
  set(BUILDNAME "${BUILDNAME}-msvc80")
elseif(MSVC90)
  set(BUILDNAME "${BUILDNAME}-msvc90")
elseif(MSVC10)
  set(BUILDNAME "${BUILDNAME}-msvc10")
elseif(MSVC11)
  set(BUILDNAME "${BUILDNAME}-msvc11")
elseif(MSVC12)
  set(BUILDNAME "${BUILDNAME}-msvc12")
elseif(MSVC)
  set(BUILDNAME "${BUILDNAME}-msvc")
elseif(BORLAND)
  set(BUILDNAME "${BUILDNAME}-borland")
elseif(MINGW)
  set(BUILDNAME "${BUILDNAME}-mingw")
else()
  # g++
  set(BUILDNAME "${BUILDNAME}-${CMAKE_BASE_NAME}")
endif()

# Find out the version of gcc being used.
if(CMAKE_COMPILER_IS_GNUCC)
  exec_program(${CMAKE_CXX_COMPILER}
    ARGS -dumpversion
    OUTPUT_VARIABLE COMPILER_VERSION
  )
  #message("COMPILER_VERSION 1: ${COMPILER_VERSION}")
  string(REGEX REPLACE ".* ([0-9])\\.([0-9])\\.[0-9].*" "\\1\\2"
    COMPILER_VERSION ${COMPILER_VERSION})
  #message("COMPILER_VERSION 2: ${COMPILER_VERSION}")

  set(BUILDNAME "${BUILDNAME}${COMPILER_VERSION}")
  
endif(CMAKE_COMPILER_IS_GNUCC)

#message("BUILDNAME=${BUILDNAME}")
