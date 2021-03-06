// $Id: archBSemPrivate.i,v 1.2 2008/07/18 15:27:30 gutleber Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/
 
pthread_mutex_t      mutex_;    // the mutex object
pthread_mutexattr_t  attr_;    // the mutex attribute object
State		     state_;   // current value of the mutex
