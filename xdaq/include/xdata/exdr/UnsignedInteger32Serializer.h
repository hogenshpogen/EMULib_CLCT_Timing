// $Id: UnsignedInteger32Serializer.h,v 1.2 2008/07/18 15:28:11 gutleber Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _xdata_exdr_UnsignedInteger32Serializer_h_
#define _xdata_exdr_UnsignedInteger32Serializer_h_

#include "xdata/exdr/ObjectSerializer.h"
#include "xdata/Serializable.h"
#include "xdata/exception/Exception.h"
#include "xdata/UnsignedInteger32.h"



namespace xdata {
namespace exdr {


class UnsignedInteger32Serializer : public xdata::exdr::ObjectSerializer  
{	
	public:
	virtual ~UnsignedInteger32Serializer();

	std::string type() const;
	
	void exportAll(xdata::exdr::Serializer * serializer,  xdata::Serializable * serializable, xdata::exdr::OutputStreamBuffer * sbuf) throw (xdata::exception::Exception);
	void import (xdata::exdr::Serializer * serializer,  xdata::Serializable * serializable, xdata::exdr::InputStreamBuffer * sbuf) throw (xdata::exception::Exception);
};


}}

#endif
