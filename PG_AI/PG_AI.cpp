#include "PG_AI.h"
#include "../export.h"

#include "../SoCINDefines.h"
#include "../Parameters/Parameters.h"

#include "constant.h"

PG_AI::PG_AI(sc_module_name mn, unsigned int priority,
             unsigned short numReqs_Grants,
             unsigned short int ROUTER_ID,
             unsigned short int PORT_ID)
        : IPriorityGenerator(mn,numReqs_Grants,ROUTER_ID,PORT_ID)
{
    if(priority >= numReqs_Grants) {
        o_PRIORITIES[0].initialize(true);
        for(unsigned int i = 1; i < numReqs_Grants; i++) {
            o_PRIORITIES[i].initialize(false);
        }
    } else {
        for(unsigned int i = 0; i < numReqs_Grants; i++) {
            if(i != priority) {
                o_PRIORITIES[i].initialize(false);
            } else {
                o_PRIORITIES[i].initialize(true);
            }
        }
    }

}

//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
    SS_EXP IPriorityGenerator* new_PG(sc_simcontext* simcontext,
                              sc_module_name moduleName,
                              unsigned short int numReqs_Grants,
                              unsigned short int ROUTER_ID,
                              unsigned short int PORT_ID) {
        // Simcontext is needed because in shared library a
        // new and different simcontext will be created if
        // the main application simcontext is not passed to
        // this shared library.
        // IMPORTANT: The simcontext assignment shall be
        // done before component instantiation.
        sc_curr_simcontext = simcontext;
        sc_default_global_context = simcontext;

        return new PG_AI(
          moduleName,
          priorities[ROUTER_ID][PORT_ID],
          numReqs_Grants,
          ROUTER_ID,
          PORT_ID
        );
    }
    SS_EXP void delete_PG(IPriorityGenerator* pg) {
        delete pg;
    }
}
