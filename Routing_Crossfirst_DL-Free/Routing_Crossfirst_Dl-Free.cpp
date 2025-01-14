#include "Routing_Crossfirst_Dl-Free.h"

//#define DEBUG_ROUTING

Routing_Crossfirst::Routing_Crossfirst(sc_module_name mn,
                                     unsigned short nPorts,
                                     unsigned short ROUTER_ID,
                                     unsigned short PORT_ID)
    : IRouting(mn,nPorts,ROUTER_ID,PORT_ID),
      REQ_NONE(0),
      REQ_LOCAL(1),
      REQ_CLOCKWISE(2),
      REQ_ANTICLOCKWISE(4),
      REQ_ACROSS(8)
{
    sysSize = (NUM_ELEMENTS%2 == 0? NUM_ELEMENTS:NUM_ELEMENTS+1);
    maxHops = ceil(sysSize/4);

    SC_METHOD(p_REQUEST);
    sensitive << i_READ_OK << i_DATA;
}

/*!
 * \brief Routing_Ring::p_REQUEST Process that generate the requests
 */
void Routing_Crossfirst::p_REQUEST() {
    UIntVar   v_DATA;                // Used to extract fields from data
    UIntVar   v_DEST(0,RIB_WIDTH);   // Destination address
    bool      v_BOP;                 // packet framing bit: begin of packet
    bool      v_HEADER_PRESENT;      // A header is in the FIFO's output
    UIntVar   v_REQUEST(0,numPorts); // Encoded request
    int v_LOCAL, v_OFFSET;           // Aux. variables used for routing

    Flit f = i_DATA.read();
    v_DATA = f.data;

    // It extracts the RIB fields and the framing bits
    v_BOP   = v_DATA[FLIT_WIDTH-2];

    // It determines if a header is present
    if ((v_BOP==1) && (i_READ_OK.read()==1)) {
        v_HEADER_PRESENT = 1;
    } else {
        v_HEADER_PRESENT = 0;
    }

    // It runs the routing algorithm
    if (v_HEADER_PRESENT) {
        v_DEST = v_DATA.range(RIB_WIDTH-1, 0);
        v_LOCAL = ROUTER_ID;

        v_OFFSET = v_DEST.to_int() - v_LOCAL;

        if (v_OFFSET != 0) {
            unsigned short v_LAST_ID = sysSize-1;
            if (v_OFFSET > 0) {
                // Use VC 0
                f.data[CLS_POS] = 0;
                if( v_OFFSET > v_LAST_ID/2 ) {
                    if( (sysSize - v_OFFSET) > maxHops ) {
                        v_REQUEST = REQ_ACROSS;
                    } else {
                        v_REQUEST = REQ_ANTICLOCKWISE;
                    }
                } else {
                    if( v_OFFSET > maxHops ) {
                        v_REQUEST = REQ_ACROSS;
                    } else {
                        v_REQUEST = REQ_CLOCKWISE;
                    }
                }
            } else {
                // Use VC 1
                f.data[CLS_POS] = 1;
                v_OFFSET = v_OFFSET*-1; // Or use abs(v_OFFSET) the result is the same
                if( v_OFFSET <= v_LAST_ID/2 ) {
                    if(v_OFFSET > maxHops) {
                        v_REQUEST = REQ_ACROSS;
                    } else {
                        v_REQUEST = REQ_ANTICLOCKWISE;
                    }
                } else {
                    if( (sysSize - v_OFFSET) > maxHops ) {
                        v_REQUEST = REQ_ACROSS;
                    } else {
                        v_REQUEST = REQ_CLOCKWISE;
                    }
                }
            }
        } else { // Current == Destination
            v_REQUEST = REQ_LOCAL;
        }
        if( f.packet_ptr != NULL ) {
            f.packet_ptr->hops++;
        }

#ifdef DEBUG_ROUTING
        std::cout << "\n[Routing_Crossfirst]"
                  << " LOCAL: " << v_LOCAL << ", DEST: " << v_DEST
                  << ", Req: ";
        switch(v_REQUEST.to_uint()) {
            case 1:
                std::cout << "LOCAL";
                break;
            case 2:
                std::cout << "CLOCKWISE";
                break;
            case 4:
                std::cout << "ANTICLOCKWISE";
                break;
            case 8:
                std::cout << "ACROSS";
                break;
            default:
                std::cout << "NONE";
        }
#endif
    } else {
        v_REQUEST = REQ_NONE;
    }

    // Outputs
    for( unsigned short i = 0; i < numPorts; i++ ) {
        o_REQUEST[i].write( v_REQUEST[i] );
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////
/*!
 * Factory Method to plugin use
 */
extern "C" {
    SS_EXP IRouting* new_Routing(sc_simcontext* simcontext,
                              sc_module_name moduleName,
                              unsigned short int nPorts,
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

        if( NUM_VC < 2 ){
            std::cout << "Chordal ring deadlock free routing needs at least two virtual channels"<< std::endl;
            return NULL;
        } else {
            return new Routing_Crossfirst(moduleName,nPorts,ROUTER_ID,PORT_ID);
        }
    }
    SS_EXP void delete_Routing(IRouting* routing) {
        delete routing;
    }
}
