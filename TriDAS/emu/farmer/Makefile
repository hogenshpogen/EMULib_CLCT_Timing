##
#
# This is the emusupervisor Makefile
#
##
include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)
include $(BUILD_HOME)/emu/rpm_version

#
# Packages to be built
#
Project = emu
Package = farmer
PackageName=farmer
Description="Emu (CSC) Farmer"
Summary="Emu (CSC) Farmer"
Authors="Karoly Banicz"
Link=""

Sources = \
	Application.cc \
	Cgi.cc \
	Xalan.cc \
	FileSystem.cc \
	SOAP.cc \
	ProcessDescriptor.cc \
	version.cc

IncludeDirs = \
	$(XDAQ_ROOT)/include

LibraryDirs = $(XDAQ_ROOT)/lib

UserSourcePath =
UserCFlags =
UserCCFlags = -g -Wall -pedantic-errors -Wno-long-long
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries =
ExternalObjects = 

#
# Compile the source files and create a shared library
#
DynamicLibrary = emufarmer
StaticLibrary =

TestLibraries=
TestExecutables=

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules
