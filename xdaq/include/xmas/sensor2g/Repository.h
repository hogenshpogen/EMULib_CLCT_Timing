// $Id: Repository.h,v 1.2 2008/07/18 15:28:33 gutleber Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _xmas_sensor2g_RepositoryImpl_h_
#define _xmas_sensor2g_RepositoryImpl_h_

#include <vector>
#include <map>
#include <string>
#include "xmas/sensor2g/exception/Exception.h"
#include "xdata/Table.h"
#include "toolbox/TimeVal.h"

namespace xmas
{
	namespace sensor2g 
	{
	
	class Repository
	{
		public:

		Repository();
		~Repository();
		
		void add ( const toolbox::TimeVal& time, xdata::Table::Reference& table )
			throw (xmas::sensor2g::exception::Exception);
			
		void add ( xdata::Table::Reference& table ) 
			throw (xmas::sensor2g::exception::Exception);
		
		// Remove all tables of a flashlist, older than or equal the given absolute time
		void remove( const toolbox::TimeVal& time )
			throw (xmas::sensor2g::exception::Exception);
		
		void clear() throw (xmas::sensor2g::exception::Exception);
		
		// Get all tables of a flashlist that are younger than "from" and older than "to"
		std::map<toolbox::TimeVal, xdata::Table::Reference> getHistory
			(
				const toolbox::TimeVal& from, 
				const toolbox::TimeVal& to
			);
		
		// Return the latest added table, throw if nothing found
		xdata::Table::Reference last() 
			throw (xmas::sensor2g::exception::Exception);
				
		protected:

		std::map<toolbox::TimeVal, xdata::Table::Reference> tables_;
	};
	
	}
}

#endif
