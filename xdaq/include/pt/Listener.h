// $Id: Listener.h,v 1.3 2008/07/18 15:27:18 gutleber Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _pt_Listener_h_
#define _pt_Listener_h_

#include <string>


namespace pt
{

//! A concrete Listener inherits from this class and implements a callback
//! corresponding to the service type
//
class Listener
{
	public:
	virtual ~Listener()
	{
	}

	//! Return the type of listener according the service for which it is implemented
	//
	virtual std::string getService() = 0;
	
	
};

}

#endif
