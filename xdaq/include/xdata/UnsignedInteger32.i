// $Id: UnsignedInteger32.i,v 1.2 2008/07/18 15:28:08 gutleber Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2009, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#include <stdlib.h>
#include <limits.h>

template<class T> xdata::UnsignedInteger32Type<T>::UnsignedInteger32Type(T value)
: xdata::SimpleType<T>(value)
{		
}

// uninitialized variable
template<class T> xdata::UnsignedInteger32Type<T>::UnsignedInteger32Type()
{		
}
template<class T> xdata::UnsignedInteger32Type<T>::UnsignedInteger32Type
(
	const std::string & value
) 
throw (xdata::exception::Exception)
: xdata::SimpleType<T>(value)
{
	this->fromString(value);
}
template<class T> xdata::UnsignedInteger32Type<T>::~UnsignedInteger32Type()
{		
}

template<class T> std::string xdata::UnsignedInteger32Type<T>::type() const
{
	return "unsigned int 32";
}

template<class T> std::string xdata::UnsignedInteger32Type<T>::toString () 
	throw (xdata::exception::Exception)
{
	if (this->isInf())
	{
		return "infinite";
	}
	else if (this->isNaN())
	{
		return "NaN";
	}
	else
	{
		char tmp[255];
		sprintf (tmp, "%u", static_cast<uint32_t>(this->value_));
		return tmp;
	}
}

template<class T> void  xdata::UnsignedInteger32Type<T>::fromString (const std::string & value) throw (xdata::exception::Exception)
{
	this->limits_.reset(); // reset any bits
	if ((value == "") || (value == "NaN"))
	{
		// empty string is not a number
		this->limits_.set(xdata::NaN);
		return;
	}
	else if (value == "infinite")
	{
		this->limits_.set(xdata::Infinity);
		return;
	}
	
	// sscanf (value.c_str(), "%lu", (unsigned long*)&value_);
	// The following code will convert an integer
	// from any represenation, e.g. hex, oct, bin or decimal
	//
	char* firstWrong = 0;

	errno = 0;
	
	
	unsigned long int tmp = strtoul ( value.c_str(), &firstWrong, 0 );
	if (	(tmp > UINT_MAX) ||
		(*firstWrong != '\0') || (errno == ERANGE))
	{
		this->limits_.set(xdata::NaN);
		std::string msg = "Invalid long integer format. Value ";
		msg = value;
		msg += ", error at ";
		msg += firstWrong;
		XCEPT_RAISE (xdata::exception::Exception, msg);
	}
	this->value_ = static_cast<uint32_t>(tmp);
}

template<class T> T& xdata::UnsignedInteger32Type<T>::operator++()
{
	if ( std::numeric_limits<xdata::UnsignedInteger32>::max()  == this->value_ )
	{
		this->limits_.set(xdata::Infinity);
		return this->value_;
	}
	else
	{
		return (++this->value_);
	}
}

template<class T> T& xdata::UnsignedInteger32Type<T>::operator--()
{
	if ( std::numeric_limits<xdata::UnsignedInteger32>::min()  == this->value_ )
	{
		this->limits_.set(xdata::Infinity);
		return this->value_;
	}
	else
	{
		return (--this->value_);
	}	
}

template<class T> T xdata::UnsignedInteger32Type<T>::operator++(int)
{
	if ( std::numeric_limits<xdata::UnsignedInteger32>::max()  == this->value_ )
	{
		this->limits_.set(xdata::Infinity);
		return this->value_;
	}
	else
	{
		this->value_++;
		return this->value_;
	}
}

template<class T> T xdata::UnsignedInteger32Type<T>::operator--(int)
{
	if ( std::numeric_limits<xdata::UnsignedInteger32>::min()  == this->value_ )
	{
		this->limits_.set(xdata::Infinity);
		return this->value_;
	}
	else
	{
		this->value_--;
		return this->value_;
	}	
}
