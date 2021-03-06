/**
 *      @file version.h
 *
 *     @short Provides run-time versioning and dependency checking of libraries
 *
 *       @see ---
 *    @author Johannes Gutleber, Luciano Orsini, Hannes Sakulin
 * $Revision: 1.1 $
 *     $Date: 2007/03/27 08:51:48 $
 *
 *
 **/


#ifndef _jaljtagSVFSequencer_version_h_
#define _jaljtagSVFSequencer_version_h_

#include "config/PackageInfo.h"
// !!! Edit this line to reflect the latest package version !!!

#define JALJTAGSVFSEQUENCER_VERSION_MAJOR 1
#define JALJTAGSVFSEQUENCER_VERSION_MINOR 3
#define JALJTAGSVFSEQUENCER_VERSION_PATCH 1
// If any previous versions available E.g. #define JALJTAGSVFSEQUENCER_PREVIOUS_VERSIONS "3.8.0,3.8.1"
#undef JALJTAGSVFSEQUENCER_PREVIOUS_VERSIONS 


//
// Template macros
//
#define JALJTAGSVFSEQUENCER_VERSION_CODE PACKAGE_VERSION_CODE(JALJTAGSVFSEQUENCER_VERSION_MAJOR,JALJTAGSVFSEQUENCER_VERSION_MINOR,JALJTAGSVFSEQUENCER_VERSION_PATCH)
#ifndef JALJTAGSVFSEQUENCER_PREVIOUS_VERSIONS
#define JALJTAGSVFSEQUENCER_FULL_VERSION_LIST  PACKAGE_VERSION_STRING(JALJTAGSVFSEQUENCER_VERSION_MAJOR,JALJTAGSVFSEQUENCER_VERSION_MINOR,JALJTAGSVFSEQUENCER_VERSION_PATCH)
#else 
#define JALJTAGSVFSEQUENCER_FULL_VERSION_LIST  JALJTAGSVFSEQUENCER_PREVIOUS_VERSIONS "," PACKAGE_VERSION_STRING(JALJTAGSVFSEQUENCER_VERSION_MAJOR,JALJTAGSVFSEQUENCER_VERSION_MINOR,JALJTAGSVFSEQUENCER_VERSION_PATCH)
#endif 
namespace jaljtagSVFSequencer
{
	const std::string package  =  "jaljtagSVFSequencer";
	const std::string versions =  JALJTAGSVFSEQUENCER_FULL_VERSION_LIST;
	const std::string description = "";
	const std::string authors = "Hannes Sakulin";
	const std::string summary = "jaljtagSVFSequencer";
	const std::string link = "http://xdaqwiki.cern.ch/index.php";
	config::PackageInfo getPackageInfo();
	void checkPackageDependencies() throw (config::PackageInfo::VersionException);
	std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif
