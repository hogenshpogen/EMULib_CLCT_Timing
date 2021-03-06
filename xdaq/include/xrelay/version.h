// $Id: version.h,v 1.4 2008/07/18 15:28:49 gutleber Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _xrelay_version_h_
#define _xrelay_version_h_

#include "config/PackageInfo.h"
// !!! Edit this line to reflect the latest package version !!!

#define XRELAY_VERSION_MAJOR 3
#define XRELAY_VERSION_MINOR 9
#define XRELAY_VERSION_PATCH 0
// If any previous versions available E.g. #define XRELAY_PREVIOUS_VERSIONS "3.8.0,3.8.1"
#undef XRELAY_PREVIOUS_VERSIONS 


//
// Template macros
//
#define XRELAY_VERSION_CODE PACKAGE_VERSION_CODE(XRELAY_VERSION_MAJOR,XRELAY_VERSION_MINOR,XRELAY_VERSION_PATCH)
#ifndef XRELAY_PREVIOUS_VERSIONS
#define XRELAY_FULL_VERSION_LIST  PACKAGE_VERSION_STRING(XRELAY_VERSION_MAJOR,XRELAY_VERSION_MINOR,XRELAY_VERSION_PATCH)
#else 
#define XRELAY_FULL_VERSION_LIST  XRELAY_PREVIOUS_VERSIONS "," PACKAGE_VERSION_STRING(XRELAY_VERSION_MAJOR,XRELAY_VERSION_MINOR,XRELAY_VERSION_PATCH)
#endif 

namespace xrelay 
{
	const std::string package  =  "xrelay";
   	const std::string versions =  XRELAY_FULL_VERSION_LIST;
	const std::string summary = "multi-hop relaying of one-way SOAP";
	const std::string description = "";
	const std::string authors = "Johannes Gutleber, Luciano Orsini";
	const std::string link = "http://xdaqwiki.cern.ch/index.php/XRelay";
	config::PackageInfo getPackageInfo();
	void checkPackageDependencies() throw (config::PackageInfo::VersionException);
	std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
