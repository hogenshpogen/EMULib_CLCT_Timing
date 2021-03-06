// $Id: StandardObjectPolicy.h,v 1.4 2008/07/18 15:28:41 gutleber Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _xoap_StandardObjectPolicy_h_
#define _xoap_StandardObjectPolicy_h_

#include <iostream>
#include "xoap/Lock.h"
namespace xoap
{

class StandardObjectPolicy 
{
  public:
    template<typename T> void dispose (T* object) 
    {
		xoap::lock();
        delete object;
		xoap::unlock();
		
    }
};

}

#endif
