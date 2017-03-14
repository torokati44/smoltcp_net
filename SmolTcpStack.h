//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __SMOLTCP_NET_SMOLTCPSTACK_H_
#define __SMOLTCP_NET_SMOLTCPSTACK_H_

#include <omnetpp.h>
#include <deque>

#include "inet/common/InitStages.h"

using namespace omnetpp;

struct Stack { /* opaque Rust struct */ };

// wrapper and adapter to the Rust implementation
class SmolTcpStack : public cSimpleModule
{
    // opaque reference to the actual Rust Stack instance
    Stack *stack = nullptr;
    std::deque<std::vector<unsigned char>> rxBuffer;

  public:
    // the C API will forward the callback from Rust (from poll...) to these, in the appropriate module instance
    void sendEthernetFrame(unsigned const char *data, unsigned int size);
    unsigned int recvEthernetFrame(unsigned char *buffer);

  protected:
    int numInitStages() const { return inet::NUM_INIT_STAGES; }
    void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
};

#endif
