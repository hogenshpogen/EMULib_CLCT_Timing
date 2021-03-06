// $Id: VectorSerializer.h,v 1.4 2008/07/18 15:28:11 gutleber Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _xdata_exdr_VectorSerializer_h_
#define _xdata_exdr_VectorSerializer_h_

#include "xdata/exdr/ObjectSerializer.h"
#include "xdata/Serializable.h"
#include "xdata/exception/Exception.h"
#include "xdata/Vector.h"



namespace xdata {
namespace exdr {


class VectorSerializer : public xdata::exdr::ObjectSerializer  
{	
	public:
	virtual ~VectorSerializer();

	std::string type() const;
	
	void exportAll(xdata::exdr::Serializer * serializer,  xdata::Serializable * serializable, xdata::exdr::OutputStreamBuffer * sbuf) throw (xdata::exception::Exception);
	void import (xdata::exdr::Serializer * serializer,  xdata::Serializable * serializable, xdata::exdr::InputStreamBuffer * sbuf) throw (xdata::exception::Exception);
};


}}

#endif
