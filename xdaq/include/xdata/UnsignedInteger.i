// $Id: UnsignedInteger.i,v 1.2 2008/07/18 15:28:08 gutleber Exp $

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

template<class T> xdata::UnsignedIntegerType<T>::UnsignedIntegerType(T value): xdata::SimpleType<T>(value)
{		
}

// uninitialized variable
template<class T> xdata::UnsignedIntegerType<T>::UnsignedIntegerType()
{		
}
template<class T> xdata::UnsignedIntegerType<T>::UnsignedIntegerType(const std::string & value) throw (xdata::exception::Exception)
{
	this->fromString(value);
}
template<class T> xdata::UnsignedIntegerType<T>::~UnsignedIntegerType()
{		
}

template<class T> std::string xdata::UnsignedIntegerType<T>::type() const
{
	return "unsigned int";
}

template<class T> std::string xdata::UnsignedIntegerType<T>::toString () throw (xdata::exception::Exception)
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
		sprintf (tmp, "%u", (unsigned int) this->value_);
		return tmp;
	}
}

template<class T> void  xdata::UnsignedIntegerType<T>::fromString (const std::string & value)
throw (xdata::exception::Exception)
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
	this->value_ = strtoul ( value.c_str(), &firstWrong, 0 );

	if ((errno == ERANGE) || (*firstWrong != '\0'))
	{
		this->limits_.set(xdata::NaN);
		std::string msg = "Failure creating unsigned long from string '";
		msg += value;
		msg += "', error at position ";
		msg += firstWrong;
		XCEPT_RAISE (xdata::exception::Exception, msg);
	}
}

template<class T> T& xdata::UnsignedIntegerType<T>::operator++()
{
	if ( std::numeric_limits<xdata::UnsignedInteger>::max()  == this->value_ )
	{
		this->limits_.set(xdata::Infinity);
		return this->value_;
	}
	else
	{
		return (++this->value_);
	}
}

template<class T> T& xdata::UnsignedIntegerType<T>::operator--()
{
	if ( std::numeric_limits<xdata::UnsignedInteger>::min()  == this->value_ )
	{
		this->limits_.set(xdata::Infinity);
		return this->value_;
	}
	else
	{
		return (--this->value_);
	}	
}

template<class T> T xdata::UnsignedIntegerType<T>::operator++(int)
{
	if ( std::numeric_limits<xdata::UnsignedInteger>::max()  == this->value_ )
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

template<class T> T xdata::UnsignedIntegerType<T>::operator--(int)
{
	if ( std::numeric_limits<xdata::UnsignedInteger>::min()  == this->value_ )
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
