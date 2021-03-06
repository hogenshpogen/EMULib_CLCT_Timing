// $Id: Exception.h,v 1.2 2008/07/18 15:26:34 gutleber Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _b2in_nub_exception_Exception_h_
#define _b2in_nub_exception_Exception_h_

#include "xcept/Exception.h"

namespace b2in {
namespace nub {
	namespace exception { 
		class Exception: public xcept::Exception 
		{
			public: 
			Exception( std::string name, std::string message, std::string module, int line, std::string function ): 
					xcept::Exception(name, message, module, line, function) 
			{} 
			Exception( std::string name, std::string message, std::string module, int line, std::string function, xcept::Exception & e ): 
					xcept::Exception(name, message, module, line, function, e) 
			{} 
		}; 
	} 
}}

#endif
