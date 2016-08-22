#include "ProgrammablePriorityEncoder.h"

#include <ctime>

ProgrammablePriorityEncoder::ProgrammablePriorityEncoder(sc_module_name mn, unsigned short nPorts,
        unsigned short XID, unsigned short YID, unsigned short PORT_ID)
    : sc_module(mn), nPorts(nPorts), i_CLK("PPE_iCLK"), i_RST("PPE_iRST"), i_REQUEST("PPE_iR",nPorts),
      i_PRIORITY("PPE_iP",nPorts), o_GRANT("PPE_oG",nPorts), o_IDLE("PPE_oIDLE"),
      w_IMED_IN("PPE_wIMED_IN",nPorts), w_IMED_OUT("PPE_wIMED_OUT",nPorts), w_GRANT("PPE_wGRANT",nPorts),
      r_GRANT("PPE_rGRANT",nPorts),w_IDLE("PPE_wIDLE"),XID(XID),YID(YID),PORT_ID(PORT_ID)
{

    unsigned short i; // Loop iterator

    SC_METHOD(p_IMED_IN);
    for(i = 0; i < nPorts; i++) {
        sensitive << w_IMED_OUT[i];
    }

    SC_METHOD(p_IMED_OUT);
    for(i = 0; i < nPorts; i++) {
        sensitive << w_IMED_IN[i] << i_REQUEST[i] << i_PRIORITY[i];
    }

    SC_METHOD(p_GRANT);
    for(i = 0; i < nPorts; i++) {
        sensitive << w_IMED_IN[i] << i_REQUEST[i] << i_PRIORITY[i];
    }

    SC_METHOD(p_GRANT_REG);
    sensitive << i_CLK.pos() << i_RST;

    SC_METHOD(p_IDLE);
    for(i = 0; i < nPorts; i++) {
        sensitive << r_GRANT[i];
    }

    SC_METHOD(p_OUTPUTS);
    sensitive << w_IDLE;
    for(i = 0; i < nPorts; i++) {
        sensitive << r_GRANT[i];
    }

}

////////////////////////////////////////////////////////////////////////////////
void ProgrammablePriorityEncoder::p_IMED_IN()
////////////////////////////////////////////////////////////////////////////////
// Status from the previous arbitration cell
{
    unsigned short i; // Loop iterator
    std::vector<bool> v_IMED_OUT(nPorts); // A copy of w_IMED_OUT for bit-level operations
    std::vector<bool> v_IMED_IN(nPorts);  // A temp. var. used to calculate Imed_in

    for( i = 0; i < nPorts; i++ ) {
        v_IMED_OUT[i] = w_IMED_OUT[i].read();
    }

    v_IMED_IN[0] = v_IMED_OUT[nPorts-1];
    for( i = 1; i < nPorts; i++) {
        v_IMED_IN[i] = v_IMED_OUT[i-1];
    }

    for( i = 0; i < nPorts; i++ ) {
        w_IMED_IN[i].write(v_IMED_IN[i]);
    }
}


////////////////////////////////////////////////////////////////////////////////
void ProgrammablePriorityEncoder::p_GRANT()
////////////////////////////////////////////////////////////////////////////////
// Grant signal sent to the requesting block
{
    unsigned short i; // Loop iterator

    std::vector<bool> v_IMED_IN(nPorts);  // A copy of Imed_in for bit-level operations
    std::vector<bool> v_REQUEST(nPorts);  // A copy of R for bit-level operations
    std::vector<bool> v_PRIORITY(nPorts); // A copy of P for bit-level operations
    std::vector<bool> v_GRANT(nPorts);    // A temp. var. used to calculate Grant

    for( i = 0; i < nPorts; i++ ) {
        v_IMED_IN[i] = w_IMED_IN[i].read();
        v_REQUEST[i] = i_REQUEST[i].read();
        v_PRIORITY[i] = i_PRIORITY[i].read();
    }

    for( i = 0; i < nPorts; i++ ) {
        v_GRANT[i] = v_REQUEST[i] and (not (v_IMED_IN[i] and (not v_PRIORITY[i]) ) );
    }

    for( i = 0; i < nPorts; i++ ) {
        w_GRANT[i].write( v_GRANT[i] );
    }
}


////////////////////////////////////////////////////////////////////////////////
void ProgrammablePriorityEncoder::p_IMED_OUT()
////////////////////////////////////////////////////////////////////////////////
// Status to the next arbitration cell
{
    unsigned short i; // Loop iterator

    std::vector<bool> v_IMED_IN(nPorts);  // A copy of Imed_in for bit-level operations
    std::vector<bool> v_REQUEST(nPorts);  // A copy of R for bit-level operations
    std::vector<bool> v_PRIORITY(nPorts); // A copy of P for bit-level operations
    std::vector<bool> v_IMED_OUT(nPorts); // A temp. var. used to calculate Imed_out

    for( i = 0; i < nPorts; i++ ) {
        v_IMED_IN[i] = w_IMED_IN[i].read();
        v_REQUEST[i] = i_REQUEST[i].read();
        v_PRIORITY[i] = i_PRIORITY[i].read();
    }

    for( i = 0; i < nPorts; i++ ) {
        v_IMED_OUT[i] = v_REQUEST[i] or (v_IMED_IN[i] and (not v_PRIORITY[i]) );
    }

    for( i = 0; i < nPorts; i++ ) {
        w_IMED_OUT[i].write(v_IMED_OUT[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////
void ProgrammablePriorityEncoder::p_GRANT_REG()
////////////////////////////////////////////////////////////////////////////////
// Implements the register that stores the grant for a selected request
{
    unsigned short i;

    std::vector<bool> v_GRANT(nPorts);     // A copy of Grant_tmp for bit-level operations
    std::vector<bool> v_REQUEST(nPorts);   // A copy of R for bit-level operations
    std::vector<bool> v_GRANT_REG(nPorts); // A temp. var. used to calculate Grant_reg

    for( i = 0; i < nPorts; i++ ) {
        v_GRANT[i] = w_GRANT[i].read();
        v_REQUEST[i] = i_REQUEST[i].read();
        v_GRANT_REG[i] = r_GRANT[i].read();
    }

    for ( i = 0; i < nPorts; i++ ) {
        if (i_RST.read()) {
            v_GRANT_REG[i] = 0;
        } else { // A register bit can be updated when the arbiter is idle
            if (w_IDLE.read()) {
                v_GRANT_REG[i] = v_GRANT[i];
            } else { // or when a req. is low, specially when a granted request is reset
                if (!v_REQUEST[i]) {
                    v_GRANT_REG[i] = 0;
                }
            }
        }
    }

    for( i = 0; i < nPorts; i++ ) {
        r_GRANT[i].write( v_GRANT_REG[i] );
    }
}


////////////////////////////////////////////////////////////////////////////////
void ProgrammablePriorityEncoder::p_IDLE()
////////////////////////////////////////////////////////////////////////////////
// It signals if the channel scheduled by the arb. is idle (there is no grant)
{
    unsigned short i;

    std::vector<bool> v_GRANT_REG(nPorts); // A copy of Grant_reg for bit-level operations
    bool  v_IDLE;                          // A temp. var. used to calculate s_idle

    for( i = 0; i < nPorts; i++ ) {
        v_GRANT_REG[i] = r_GRANT[i].read();
    }

    v_IDLE = 0;
    for ( i = 0; i < nPorts; i++ ) {
        v_IDLE = v_IDLE or v_GRANT_REG[i];
    }

    w_IDLE.write( not v_IDLE );
}


////////////////////////////////////////////////////////////////////////////////
void ProgrammablePriorityEncoder::p_OUTPUTS()
////////////////////////////////////////////////////////////////////////////////
// It updates the output
{
    o_IDLE.write(w_IDLE);
    for( unsigned short i = 0; i < nPorts; i++) {
        o_GRANT[i].write( r_GRANT[i].read() );
    }

}

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Testbench //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
tst_PPE::tst_PPE(sc_module_name mn, unsigned short nPorts)
    : sc_module(mn)
    , nPorts(nPorts){

    w_PRIORITY.init(nPorts);
    w_REQUEST.init(nPorts);
    w_GRANT.init(nPorts);

    u_PPE = new ProgrammablePriorityEncoder("PPE_DUT",nPorts,0,0,0);
    u_PPE->i_CLK(i_CLK);
    u_PPE->i_RST(w_RST);
    u_PPE->i_PRIORITY(w_PRIORITY);
    u_PPE->i_REQUEST(w_REQUEST);
    u_PPE->o_GRANT(w_GRANT);
    u_PPE->o_IDLE(w_IDLE);

    tf = sc_create_vcd_trace_file("ppe_waves");
    sc_trace(tf,i_CLK,"CLK");
    sc_trace(tf,w_RST,"RST");
    sc_trace(tf,w_IDLE,"o_IDLE");
    for(unsigned short i = 0; i < nPorts; i++) {
        char strPri[10];
        sprintf(strPri,"i_P(%u)",i);
        sc_trace(tf,w_PRIORITY[i],strPri);

        char strReq[10];
        sprintf(strReq,"i_R(%u)",i);
        sc_trace(tf,w_REQUEST[i],strReq);

        char strGnt[10];
        sprintf(strGnt,"o_G(%u)",i);
        sc_trace(tf,w_GRANT[i],strGnt);

        // Internal PPE signals
        char strWimedIn[15];
        sprintf(strWimedIn,"w_IMED_IN(%u)",i);
        sc_trace(tf,u_PPE->w_IMED_IN[i],strWimedIn);

        char strWimedOut[16];
        sprintf(strWimedOut,"w_IMED_OUT(%u)",i);
        sc_trace(tf,u_PPE->w_IMED_OUT[i],strWimedOut);

        char strWGnt[14];
        sprintf(strWGnt,"w_GRANT(%u)",i);
        sc_trace(tf,u_PPE->w_GRANT[i],strWGnt);

        char strRGnt[14];
        sprintf(strRGnt,"r_GRANT(%u)",i);
        sc_trace(tf,u_PPE->r_GRANT[i],strRGnt);
    }
    sc_trace(tf,u_PPE->w_IDLE,"w_IDLE");

    SC_CTHREAD(p_stimulus,i_CLK.pos());
    sensitive << i_CLK;
}

tst_PPE::~tst_PPE() {
    sc_close_vcd_trace_file(tf);
    delete u_PPE;
}

void tst_PPE::p_stimulus() {

    srand(time(NULL));
    unsigned short i;

    w_RST.write(true);
    wait();

    w_RST.write(false);
    wait();

    for(unsigned short x = 0; x < 3; x++) {

        unsigned short rnd = rand() % nPorts;
        w_PRIORITY[rnd].write(true);
        // All request on
        for(i = 0; i < nPorts; i++) {
            w_REQUEST[i].write(true);
        }
        wait();
        // All request off
        for(i = 0; i < nPorts; i++) {
            w_REQUEST[i].write(false);
        }
        wait();
        w_PRIORITY[rnd].write(false);
    }

    for(i = 0; i < nPorts; i++) {
        w_REQUEST[i].write(true);
    }
    wait();

    for(i = 0; i < nPorts; i++) {
        w_REQUEST[i].write(false);
        unsigned short rnd = i + (nPorts-i-1) * ((rand() % nPorts)/10);
        w_PRIORITY[rnd].write(true);
        wait();
        w_PRIORITY[rnd].write(false);
    }

    sc_stop();
}
