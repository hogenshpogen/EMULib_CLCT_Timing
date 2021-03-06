// $Id: Repository.h,v 1.15 2008/11/25 13:21:03 lorsini Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _sentinel_spotlight2g_Repository_h_
#define _sentinel_spotlight2g_Repository_h_

#include <string>
#include <map>
#include <sqlite3.h>

#include "xgi/Output.h"
#include "sentinel/spotlight2g/exception/FailedToStore.h"
#include "sentinel/spotlight2g/exception/FailedToRead.h"
#include "sentinel/spotlight2g/exception/FailedToOpen.h"
#include "sentinel/spotlight2g/exception/FailedToClose.h"
#include "sentinel/spotlight2g/exception/NotFound.h"

#include "sentinel/spotlight2g/DataBase.h"

#include "xdaq/Object.h"


namespace sentinel 
{
	namespace spotlight2g 
	{
		class Repository: public xdaq::Object
		{
			public:

			Repository(xdaq::Application * owner, const std::string  &filename, toolbox::TimeInterval & archiveWindow)
				throw (sentinel::spotlight2g::exception::Exception);
			~Repository();
			
			sentinel::spotlight2g::DataBase * getArchive(toolbox::TimeVal & date)
				throw (sentinel::spotlight2g::exception::FailedToOpen);;

					
			/*! Store an exception 
				This function is thread safe (read/write lock)
			*/
			void store (xcept::Exception& ex)
				throw (sentinel::spotlight2g::exception::FailedToStore);
			
			void rearm (const std::string & exception,const std::string & source )
				throw (sentinel::spotlight2g::exception::FailedToStore);
			
			void revoke (xcept::Exception& ex)
				throw (sentinel::spotlight2g::exception::FailedToStore);

			
			/*! Retrieve a single exception by uuid in the format it has been stored.
				The name of the format is returned in \param format.
				This function is thread safe (read lock)
			*/
			void retrieve (const std::string& uuid, const std::string& datetime, const std::string& format, xgi::Output* out) 
				throw (sentinel::spotlight2g::exception::NotFound);

			/*! Return a list of stored exceptions in the \param ex set that match the search query 
				This function is thread safe (read lock)
			*/
			void catalog (xgi::Output* out, toolbox::TimeVal & start, toolbox::TimeVal &  end, const std::string& format)
				throw (sentinel::spotlight2g::exception::NotFound);
			
			void events (xgi::Output* out, toolbox::TimeVal & start, toolbox::TimeVal &  end, const std::string& format)
				throw (sentinel::spotlight2g::exception::NotFound);
	
				
			void lastStoredEvents (xgi::Output* out, toolbox::TimeVal & since, const std::string& format)
				throw (sentinel::spotlight2g::exception::NotFound);
				
			/*! Retrieve the number of exceptions stored */
			std::string rerieveExceptionCount();
			
			/*! Retrieve the time at which the last exception has been stored */
			toolbox::TimeVal& lastExceptionTime();
			
			/* Retrieve all "spotlight2g*.db" files in the path */
			std::vector<std::string> getFiles()
				throw (sentinel::spotlight2g::exception::NotFound);
			
			sentinel::spotlight2g::DataBase * getCurrentDataBase();
	
			void archive (toolbox::TimeInterval & window)
					throw (sentinel::spotlight2g::exception::FailedToArchive);

			std::list<toolbox::Properties> getOpenArchivesInfo();

			/*! Returns the average time it takes to archive data from current.db */
			double getAverageArchiveTime();
			
			// Perform complete archivation and maintenance operations
			//
			void reset();
	
			private:

			std::string path_; // path to repository database files			
			toolbox::BSem repositoryLock_; // used for db management operations
			toolbox::TimeVal lastExceptionTime_;
			toolbox::TimeInterval archiveWindow_;
			sentinel::spotlight2g::DataBase * currentDataBase_;
			std::map<std::string,sentinel::spotlight2g::DataBase *> archiveDataBases_;
			double archiveTime_;	
		};
	}
}
#endif
