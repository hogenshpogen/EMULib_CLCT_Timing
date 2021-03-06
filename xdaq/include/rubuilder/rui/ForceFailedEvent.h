#ifndef _rubuilder_rui_ForceFailedEvent_h_
#define _rubuilder_rui_ForceFailedEvent_h_

#include <string>

#include "toolbox/Event.h"


namespace rubuilder { namespace rui { // namespace rubuilder::rui

/**
 * Used by an application to force its finit state machine into the "Failed"
 * state.
 */
class ForceFailedEvent :
public toolbox::Event
{
public:

    /**
     * Constructor.
     */
    ForceFailedEvent
    (
        const std::string &reason,
        void              *originator = 0
    );

    /**
     * Returns the reason the finite state machine is being forced to the
     * "Failed" state.
     */
    std::string getReason();


private:

   /**
    * The reason why the state machine is being foced to the "Failed" state.
    */
   std::string reason_;
};

} } // namespace rubuilder::rui

#endif
