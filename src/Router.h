/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : IRouter
FILE   : Router.h
--------------------------------------------------------------------------------
DESCRIPTION: Router interface (abstract class)
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 08/09/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from ParIS
--------------------------------------------------------------------------------
*/

#ifndef __ROUTER_H__
#define __ROUTER_H__

#include "../src/XIN.h"
#include "../src/XOUT.h"

/////////////////////////////////////////////////////////////////////////////////
/// Simple Router interface
/////////////////////////////////////////////////////////////////////////////////
/*!
 * \brief The IRouter class is an interface (abstract class) for router
 * implementations.
 *
 * This interface is based on Paris router.
 */
class IRouter : public SoCINModule {
public:
    unsigned short numPorts;
    // Interface
    // System signals
    sc_in<bool> i_CLK;  // Clock
    sc_in<bool> i_RST;  // Reset

    // Ports
    sc_vector<sc_in<Flit> >  i_DATA_IN;     // Data of input channels
    sc_vector<sc_in<bool> >  i_VALID_IN;    // Valid of input channels
    sc_vector<sc_out<bool> > o_RETURN_IN;   // Return of input channels
    sc_vector<sc_out<Flit> > o_DATA_OUT;    // Data of output channels
    sc_vector<sc_out<bool> > o_VALID_OUT;   // Valid of output channels
    sc_vector<sc_in<bool> >  i_RETURN_OUT;  // Return of output channels

    // Internal data structures
    unsigned short ROUTER_ID;

    IRouter(sc_module_name mn,
            unsigned short nPorts,
            unsigned short ROUTER_ID);

   ModuleType moduleType() const { return SoCINModule::TRouter; }
    ~IRouter() = 0;
};

inline IRouter::~IRouter(){}

inline IRouter::IRouter(sc_module_name mn,
                        unsigned short nPorts,
                        unsigned short ROUTER_ID)
    : SoCINModule(mn),
      numPorts(nPorts),
      i_CLK("IRouter_iCLK"),
      i_RST("IRouter_iRST"),
      i_DATA_IN("IRouter_iDATA_IN",nPorts),
      i_VALID_IN("IRouter_iVALID_IN",nPorts),
      o_RETURN_IN("IRouter_oRETURN_IN",nPorts),
      o_DATA_OUT("IRouter_oDATA_OUT",nPorts),
      o_VALID_OUT("IRouter_oVALID_OUT",nPorts),
      i_RETURN_OUT("IRouter_iRETURN_OUT",nPorts),
      ROUTER_ID(ROUTER_ID) {}



/////////////////////////////////////////////////////////////////////////////////
/// Router interface for router that implements virtual channels
/////////////////////////////////////////////////////////////////////////////////
class IRouter_VC : public IRouter {
public:
    unsigned short numVirtualChannels;
    unsigned short widthVcSelector;
    // Inherits common interfaces of router without virtual channel

    // Interface - virtual channels
    sc_vector<sc_vector<sc_in<bool> > >  i_VC_IN;  // IN[VirtualChannel][BitSelector]
    sc_vector<sc_vector<sc_out<bool> > > o_VC_OUT; // OUT[VirtualChanne][BitSelector]

    IRouter_VC(sc_module_name mn,
               unsigned short nPorts,
               unsigned short nVirtualChannels,
               unsigned short ROUTER_ID);
    ~IRouter_VC() = 0;

};

inline IRouter_VC::~IRouter_VC(){}

inline IRouter_VC::IRouter_VC(sc_module_name mn,
                              unsigned short nPorts,
                              unsigned short nVirtualChannels,
                              unsigned short ROUTER_ID)
    : IRouter(mn,nPorts,ROUTER_ID),
      numVirtualChannels(nVirtualChannels),
      widthVcSelector( (unsigned short)ceil(log2(nVirtualChannels)) ),
      i_VC_IN("IRouter_VC_iVC_IN"),
      o_VC_OUT("IRouter_VC_oVC_OUT")
{
    // Initializing Virtual Channel ports with the width of the selector
    if( widthVcSelector > 0 ) {
        i_VC_IN.init(nPorts);
        o_VC_OUT.init(nPorts);
        for(unsigned short i = 0; i < nPorts; i++) {
            o_VC_OUT[i].init(widthVcSelector);
            i_VC_IN[i].init(widthVcSelector);
        }
    }
}


/////////////////////////////////////////////////////////////
/// Typedefs for Factory Methods of concrete Routers
/////////////////////////////////////////////////////////////
/*!
 * \brief create_Router Typedef for instantiate a Router
 * \param sc_simcontext A pointer of simulation context (required for correct plugins use)
 * \param sc_module_name Name for the module to be instantiated
 * \param nPorts Number of ports of the Router
 * \param nVirtualChannels Number of virtual channels in the router
 * \param ROUTER_ID Router identifier in the network
 * \return A method for instantiate a Router
 */
typedef IRouter* create_Router(sc_simcontext*,
                               sc_module_name,
                               unsigned short int nPorts,
                               unsigned short int nVirtualChannels,
                               unsigned short int ROUTER_ID);

/*!
 * \brief destroy_Router Typedef for deallocating a Router
 */
typedef void destroy_Router(IRouter*);
/////////////////////////////////////////////////////////////

#endif // __ROUTER_H__
