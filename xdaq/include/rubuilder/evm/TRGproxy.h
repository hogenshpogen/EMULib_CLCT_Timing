#ifndef _rubuilder_evm_TRGproxy_h_
#define _rubuilder_evm_TRGproxy_h_

#include <boost/scoped_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

#include "log4cplus/logger.h"

#include "rubuilder/utils/InfoSpaceItems.h"
#include "rubuilder/utils/PerformanceMonitor.h"
#include "rubuilder/utils/OneToOneQueue.h"
#include "toolbox/mem/Reference.h"
#include "xdaq/Application.h"
#include "xdata/Boolean.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/UnsignedInteger64.h"
#include "xdata/Serializable.h"
#include "xdata/String.h"


namespace rubuilder { namespace evm { // namespace rubuilder::evm

  class TRGproxy;

  namespace TRGproxyHandlers {

    class Handler
    {
    public:
      
      Handler(TRGproxy* parent) :
      parent_(parent) {}
      
      virtual ~Handler() {};
      
      virtual void I2Ocallback(toolbox::mem::Reference**) const {};
      virtual bool requestTriggers(const uint32_t&) {return false;}
      virtual bool sendOldTriggerMessage() {return false;}
      virtual void printHtml(xgi::Output*) {};
      virtual void reset() {};
      
    protected:
      
      TRGproxy* parent_;
      
    };
    
    class GTPhandler;
    class GTPehandler;
    class TAhandler;
    class DummyTrigger;
  }

  /**
   * \ingroup xdaqApps
   * \brief Abstract class representing trigger interface
   */
  
  class TRGproxy
  {
    
  public:

    TRGproxy
    (
      xdaq::Application*,
      toolbox::mem::Pool*
    );

    virtual ~TRGproxy() {};

    /**
     * Notify the proxy of an input source change.
     * The input source is taken from the info space
     * parameter 'triggerSource'.
     *
     * Valid input strings are:
     * GTP  : the global trigger processor sends
     *        triggers via RUI
     * GTPe : trigger emulator sends triggers in 
     *        emulator format
     * TA   : triggers are simulated by the TA
     *        (trigger adapter) using the TA hand-shake
     * Local: dummy triggers are generated locally 
     */
    void triggerSourceChanged();
    
    /**
     * Callback for I2O message received from trigger
     */
    void I2Ocallback(toolbox::mem::Reference*);

    /**
     * Send any old trigger messages
     */
    bool sendOldTriggerMessage();
    
    /**
     * Request/create as many triggers as fit into the triggerFIFO.
     * Only implemented for test setups, not for production system.
     * Returns true if new triggers were generated.
     */
    bool requestTriggers();
    
    /**
     * Request/create specified number of triggers.
     * Only implemented for test setups, not for production system.
     * Returns true if new triggers were generated.
     */
    bool requestTriggers(const uint32_t& nbTriggers);

    /**
     * Get next available trigger message in trigBufRef.
     * If no trigger is available, return false.
     */
    bool getNextTrigger(toolbox::mem::Reference** trigBufRef)
    { return triggerFIFO_.deq(*trigBufRef); }

    /**
     * Return true if no triggers are queued
     */
    bool empty() const
    { return triggerFIFO_.empty(); }

    /**
     * Append the info space parameters used for the
     * configuration to the InfoSpaceItems
     */
    void appendConfigurationItems(utils::InfoSpaceItems&);
    
    /**
     * Append the info space items to be published in the 
     * monitoring info space to the InfoSpaceItems
     */
    void appendMonitoringItems(utils::InfoSpaceItems&);
    
    /**
     * Update all values of the items put into the monitoring
     * info space. The caller has to make sure that the info
     * space where the items reside is locked and properly unlocked
     * after the call.
     */
    void updateMonitoringItems();
    
    /**
     * Reset the monitoring counters
     */
    void resetMonitoringCounters();
  
    /**
     * Configure
     */
    void configure();

    /**
     * Remove all data
     */
    void clear();

    /**
     * Print monitoring/configuration as HTML snipped
     */
    void printHtml(xgi::Output*);

    /**
     * Print the content of the trigger FIFO as HTML snipped
     */
    inline void printTriggerFIFO(xgi::Output* out)
    { triggerFIFO_.printVerticalHtml(out); }

    /**
     * Return trigger performance monitoring struct
     */
    void getTriggerPerformance(utils::PerformanceMonitor&);

  private:

    friend class TRGproxyHandlers::Handler;
    friend class TRGproxyHandlers::GTPhandler;
    friend class TRGproxyHandlers::GTPehandler;
    friend class TRGproxyHandlers::TAhandler;
    friend class TRGproxyHandlers::DummyTrigger;

    void dumpTriggerToLogger(toolbox::mem::Reference*) const;
    void updateTriggerCounters(toolbox::mem::Reference*);
    void pushOntoTriggerFIFO(toolbox::mem::Reference*);
  
    xdaq::Application* app_;
    toolbox::mem::Pool* fastCtrlMsgPool_;
    log4cplus::Logger& logger_;

    typedef utils::OneToOneQueue<toolbox::mem::Reference*> TriggerFIFO;
    TriggerFIFO triggerFIFO_;

    boost::scoped_ptr<TRGproxyHandlers::Handler> handler_;
    
    struct TriggerMonitoring
    {
      uint64_t payload;
      uint64_t payloadSquared;
      uint64_t nbTriggers;
      uint64_t i2oCount;
      uint32_t lastEventNumberFromTrigger;
    } triggerMonitoring_;
    mutable boost::mutex triggerMonitoringMutex_;

    utils::InfoSpaceItems trgParams_;
    xdata::String triggerSource_;
    xdata::Boolean dumpTriggersToLogger_;
    xdata::Boolean dropInputData_;
    xdata::UnsignedInteger32 triggerFIFOCapacity_;
    xdata::String taClass_;
    xdata::UnsignedInteger32 taInstance_;
    
    xdata::UnsignedInteger32 I2O_TA_CREDIT_Packing_;
    xdata::Boolean generateDummyTriggers_;
    xdata::UnsignedInteger32 fedPayloadSize_;
    xdata::UnsignedInteger32 dummyTriggerSourceId_;
    xdata::UnsignedInteger32 orbitsPerLS_;

    xdata::UnsignedInteger32 lastEventNumberFromTrigger_;
    xdata::UnsignedInteger64 i2oEVMTriggerCount_;
  };
  
  
} } //namespace rubuilder::evm

#endif // _rubuilder_evm_TRGproxy_h_

/// emacs configuration
/// Local Variables: -
/// mode: c++ -
/// c-basic-offset: 2 -
/// indent-tabs-mode: nil -
/// End: -
