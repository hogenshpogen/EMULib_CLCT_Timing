// $Id: CommandStateMismatch.h,v 1.4 2008/07/18 15:28:03 gutleber Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _xdaq_exception_CommandStateMismatch_h_
#define _xdaq_exception_CommandStateMismatch_h_

#include "xdaq/exception/Exception.h"

namespace xdaq {
	namespace exception { 
		class CommandStateMismatch: public xdaq::exception::Exception 
		{
			public: 
			CommandStateMismatch( std::string name, std::string message, std::string module, int line, std::string function ): 
					xdaq::exception::Exception(name, message, module, line, function) 
			{} 
			
			CommandStateMismatch( std::string name, std::string message, std::string module, int line, std::string function, xcept::Exception & e ): 
					xdaq::exception::Exception(name, message, module, line, function, e) 
			{} 
		}; 
	} 
}

#endif
