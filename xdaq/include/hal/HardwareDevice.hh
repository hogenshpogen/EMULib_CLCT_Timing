#ifndef __HardwareDevice
#define __HardwareDevice

#include <string>
#include <iostream>

#include "hal/IllegalOperationException.hh"
#include "hal/NoSuchItemException.hh"
#include "hal/VerifyException.hh"
#include "hal/UnsupportedException.hh"
#include "hal/GeneralHardwareAddress.hh"
#include "hal/AddressTableInterface.hh"
#include "hal/AddressTableItem.hh"
#include "hal/HardwareDeviceInterface.hh"
#include "hal/MaskBoundaryException.hh"
#include "hal/linux/StopWatch.hh"
#include "hal/AddressOutOfLimitsException.hh"

namespace HAL {

/**
*
*
*     @short The implementation of the hardwareDeviceInterface.
*            
*            This  class implements  the  HardwareDeviceInterface. The 
*            public methods are documented there.
*
*            Note that  this class is still abstract  since it defines 
*            an  interface to  routines which  do the  actual hardware 
*            access.   The  implementation   of   these  routines   is 
*            technology dependent and  therefore cannot be implemented
*            in  this class.  In  addition the  methods readBlock  and 
*            writeBlock are  not implemented in this  class since this 
*            is an technology dependent  issue  (there are fundamental
*            differences for VME and PCI).
*
*       @see HardwareDeviceInterface 
*    @author Christoph Schwick 
* $Revision: 1.1 $
*     $Date: 2007/03/05 18:02:09 $
*
*
**/

class HardwareDevice : public HardwareDeviceInterface {

public:
  /**
   * The constructor needs a reference to the AddressTable for the hardware Device.
   * The AddressTable MUST exist as long as the HardwareDevice since only
   * a reference is passed to the HardwareDevice (this enables to share an
   * addressTable among many HardwareDevices: imagine a crate full of ADC cards.)
   */
  HardwareDevice( AddressTableInterface & AddressTable );

  virtual ~HardwareDevice();

  void unmaskedWrite( std::string item, 
                      uint32_t data, 
                      HalVerifyOption verifyFlag = HAL_NO_VERIFY,
                      uint32_t offset = 0 ) const
    throw (NoSuchItemException, 
           IllegalOperationException, 
           BusAdapterException,
           VerifyException,
	   AddressOutOfLimitsException);

  void write( std::string item, 
              uint32_t data, 
              HalVerifyOption verifyFlag = HAL_NO_VERIFY,
              uint32_t offset = 0 ) const
    throw (NoSuchItemException,
           BusAdapterException,
           IllegalOperationException, 
           VerifyException,
           MaskBoundaryException,
	   AddressOutOfLimitsException);

  void writePulse( std::string item, 
                   uint32_t offset = 0 ) const
    throw (NoSuchItemException, 
           BusAdapterException,
           IllegalOperationException,
	   AddressOutOfLimitsException);

  void readPulse( std::string item, 
                  uint32_t offset = 0 ) const
    throw (NoSuchItemException,
           BusAdapterException,
           IllegalOperationException,
	   AddressOutOfLimitsException);

  void unmaskedRead( std::string item, 
                     uint32_t* result, 
                     uint32_t offset = 0 ) const
    throw (NoSuchItemException,
           BusAdapterException,
           IllegalOperationException,
	   AddressOutOfLimitsException);

  void read( std::string item, 
             uint32_t* result, 
             uint32_t offset = 0 ) const
    throw (NoSuchItemException,
           BusAdapterException,
           IllegalOperationException,
           MaskBoundaryException,
	   AddressOutOfLimitsException );

  bool check( std::string item,
              uint32_t expected,
              std::string faultMessage = "",
              uint32_t offset = 0,
	      std::ostream& os = std::cout ) const
    throw( NoSuchItemException,
           IllegalOperationException,
           BusAdapterException,
	   AddressOutOfLimitsException);

  void setBit( std::string item, 
               HalVerifyOption verifyFlag = HAL_NO_VERIFY,
               uint32_t offset = 0 ) const
    throw (NoSuchItemException, 
           IllegalOperationException, 
           BusAdapterException,
           VerifyException,
	   AddressOutOfLimitsException);

  void resetBit( std::string item, 
                 HalVerifyOption verifyFlag = HAL_NO_VERIFY,
                 uint32_t offset = 0 ) const
    throw (NoSuchItemException, 
           IllegalOperationException, 
           BusAdapterException,
           VerifyException,
	   AddressOutOfLimitsException);

  bool isSet( std::string item, 
              uint32_t offset = 0 ) const 
    throw (NoSuchItemException,
           BusAdapterException,
           IllegalOperationException,
	   AddressOutOfLimitsException);

  void writeBlock( std::string startItem, 
                   uint32_t length,
                   char *buffer,
                   HalVerifyOption verifyFlag = HAL_NO_VERIFY,
                   HalAddressIncrement addressBehaviour = HAL_DO_INCREMENT,
                   uint32_t offset = 0 ) const
    throw ( NoSuchItemException,
            IllegalValueException, 
            UnsupportedException, 
            VerifyException,
            BusAdapterException,
            IllegalOperationException,
	    AddressOutOfLimitsException );

  void readBlock( std::string startItem, 
                  uint32_t length,
                  char *buffer,
                  HalAddressIncrement addressBehaviour = HAL_DO_INCREMENT,
                  uint32_t offset = 0 ) const
    throw ( NoSuchItemException,
            IllegalValueException, 
            UnsupportedException,
            BusAdapterException,
            IllegalOperationException,
	    AddressOutOfLimitsException );

  void pollItem( std::string item,
                 uint32_t referenceValue,
                 uint32_t timeout,
                 uint32_t *result,
                 HalPollMethod pollMethod = HAL_POLL_UNTIL_EQUAL,
                 uint32_t offset = 0 ) const
    throw ( NoSuchItemException,
            IllegalOperationException,
            BusAdapterException,
            MaskBoundaryException,
            TimeoutException,
	    AddressOutOfLimitsException );

  const AddressTableInterface&  getAddressTableInterface() const; 

  void printAddressTable() const;


  /**
   * Abstraction of the hardware read access.
   * This  interface is called  by the   implementation  of the
   * HardwareDeviceInterface.   Since the implementation  depends on
   * the technology  of the  hardware device (VME  or PCI)  they are
   * implemented  in  the  classes  inheriting from  this  interface
   * (PCIDevice and VMEDevice).
   *
   * @param address is a generalized address from which the  program 
   *        should read. It's precise content depend on the 
   *        device technology.
   */
  virtual void hardwareRead( const GeneralHardwareAddress& address, 
                             uint32_t* result,
                             uint32_t offset = 0 ) const
    throw (IllegalValueException,
           BusAdapterException) = 0;


  /**
   * Abstraction of the hardware write access.
   * This  interface is called  by the  above implementation  of the
   * HardwareDeviceInterface.   Since the implementation  depends on
   * the technology  of the  hardware device (VME  or PCI)  they are
   * implemented  in  the  classes  inheriting from  this  interface
   * (PCIDevice and VMEDevice).
   *
   * @param address is a generalized address to which the  program 
   *        should write. It's precise content depend on the 
   *        device technology.
   */
  virtual void hardwareWrite( const GeneralHardwareAddress& address, 
                              uint32_t data,
                              uint32_t offset = 0 ) const
    throw (IllegalValueException,
           BusAdapterException) = 0;

  /**
   * Abstraction of the hardware block-read access.
   * This interface is called by the readBlock implementation of the
   * HardwareDeviceInterface.   Since the implementation  depends on
   * the technology  of the  hardware device (VME  or PCI)  they are
   * implemented  in  the  classes  inheriting from  this  interface
   * (PCIDevice and VMEDevice).
   *
   * @param address is a generalized address from which the  program 
   *        should read. It's precise content depend on the 
   *        device technology.
   */
  virtual void hardwareReadBlock( const GeneralHardwareAddress& address,
                                  uint32_t length,
                                  char *buffer,
                                  HalAddressIncrement addressBehaviour = HAL_DO_INCREMENT,
                                  uint32_t offset = 0 ) const
    throw (IllegalValueException, 
           UnsupportedException,
           IllegalOperationException,
           BusAdapterException) = 0;

  /**
   * Abstraction of the hardware block-write access.
   * This interface is called by the writeBlock implementation of the
   * HardwareDeviceInterface.   Since the implementation  depends on
   * the technology  of the  hardware device (VME  or PCI)  they are
   * implemented  in  the  classes  inheriting from  this  interface
   * (PCIDevice and VMEDevice).
   *
   * @param address is a generalized address from which the  program 
   *        should read. It's precise content depend on the 
   *        device technology.
   */
  virtual void hardwareWriteBlock( const GeneralHardwareAddress& address,
                                   uint32_t length,
                                   char *buffer,
                                   HalAddressIncrement addressBehaviour = HAL_DO_INCREMENT,
                                   uint32_t offset = 0 ) const
    throw (IllegalValueException, 
           UnsupportedException, 
           IllegalOperationException,
           BusAdapterException) = 0;

private:
  void verify( uint32_t readBack,
               uint32_t data,
               std::string methodName ) const
    throw( VerifyException );
  
  void writeAddressItem( const AddressTableItem& addressItem,
                         uint32_t data,
                         uint32_t offset,
                         HalVerifyOption verify) const
    throw( MaskBoundaryException,
           IllegalValueException,
           BusAdapterException,
	   VerifyException,
	   AddressOutOfLimitsException );

  AddressTableInterface& addressTable;
};

} /* namespace HAL */

#endif /* __HardwareDevice */
