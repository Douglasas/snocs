#ifndef __PG_AI_H__
#define __PG_AI_H__

#include "../src/PriorityGenerator.h"
/*!
 * \brief The PG_AI class implements an adaptive
 * priority generator based on an external input file
 */
class PG_AI : public IPriorityGenerator {
public:

    SC_HAS_PROCESS(PG_AI);
    PG_AI(sc_module_name mn, unsigned int priority,
          unsigned short int numReqs_Grants,
          unsigned short int ROUTER_ID,
          unsigned short int PORT_ID);

    ModuleType moduleType() const { return  SoCINModule::TPriorityGenerator; }
    const char* moduleName() const { return "PG_AI"; }
};


#endif // __PG_AI_H__
