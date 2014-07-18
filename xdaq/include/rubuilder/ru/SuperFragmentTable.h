#ifndef _rubuilder_ru_SuperFragmentTable_h_
#define _rubuilder_ru_SuperFragmentTable_h_

#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

#include <set>
#include <stdint.h>

#include "i2o/i2oDdmLib.h"
#include "interface/evb/i2oEVBMsgs.h"
#include "rubuilder/utils/Constants.h"
#include "toolbox/mem/Reference.h"


namespace rubuilder { namespace ru { // namespace rubuilder::ru

  class BUproxy;

  /**
   * \ingroup xdaqApps
   * \brief Keep track of complete super-fragments and BU requests
   */
 
  class SuperFragmentTable
  {
  public:

    struct Request
    {
      I2O_TID  buTid;                 ///< I2O TID of the requesting BU
      uint32_t buIndex;               ///< Index of the requesting BU
      uint32_t eventId;               ///< Event id
      uint32_t eventNumber;           ///< Trigger event number
      uint32_t buResourceId;          ///< BU resource id
    };

    SuperFragmentTable();

    /**
     * Register the BU proxy to be used to send complete super fragments
     */
    void registerBUproxy(boost::shared_ptr<BUproxy>);
    
    /**
     * Add event data for the given readout pair
     */
    void addPairAndBlock(const EvtIdTrigPair&, toolbox::mem::Reference*);

    /**
     * Add a BU request for data
     */
    void addRequest(const Request&);

    /**
     * Remove all data
     */
    void clear();
    
    /**
     * Return the number of super fragments ready to be served
     */
    uint32_t getNbSuperFragmentsReady() const
    { return nbSuperFragmentsReady_; }

    
  private:

    void dataReady(const Request&, toolbox::mem::Reference*);

    boost::shared_ptr<BUproxy> buProxy_;

    // Lookup table of super fragments, indexed by event id
    typedef std::map<uint32_t,toolbox::mem::Reference*> Data;
    Data data_;

    // Lookup table of requests, indexed by event id
    typedef std::map<uint32_t,Request> Requests;
    Requests requests_;

    mutable boost::mutex mutex_;

    uint32_t nbSuperFragmentsReady_;
    
  }; // SuperFragmentTable
    
  typedef boost::shared_ptr<SuperFragmentTable> SuperFragmentTablePtr;
    
    
  inline std::ostream& operator<<
  (
    std::ostream& str,
    const SuperFragmentTable::Request &request
  )
  {
    str << "buTid="        << request.buTid       << " ";
    str << "buIndex="      << request.buIndex     << " ";
    str << "eventId="      << request.eventId     << " ";
    str << "eventNumber="  << request.eventNumber << " ";
    str << "buResourceId=" << request.buResourceId;
    
    return str;
  }
  
} } // namespace rubuilder::ru

#endif // _rubuilder_ru_SuperFragmentTable_h_


/// emacs configuration
/// Local Variables: -
/// mode: c++ -
/// c-basic-offset: 2 -
/// indent-tabs-mode: nil -
/// End: -
