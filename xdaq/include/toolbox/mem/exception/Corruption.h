// $Id: Corruption.h,v 1.2 2008/07/18 15:27:35 gutleber Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _toolbox_mem_exception_Corruption_h_
#define _toolbox_mem_exception_Corruption_h_

#include "toolbox/mem/exception/Exception.h"

namespace toolbox {
namespace mem {
	namespace exception { 
		class Corruption: public toolbox::mem::exception::Exception 
		{
			public: 
			Corruption( std::string name, std::string message, std::string module, int line, std::string function ): 
					toolbox::mem::exception::Exception(name, message, module, line, function) 
			{} 
			
			Corruption( std::string name, std::string message, std::string module, int line, std::string function,
				xcept::Exception& e ): 
					toolbox::mem::exception::Exception(name, message, module, line, function, e) 
			{} 
		}; 
	} 
}
}

#endif
