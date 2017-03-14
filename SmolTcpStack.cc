
#include "SmolTcpStack.h"

#include "inet/common/serializer/headerserializers/ethernet/EthernetSerializer.h"
#include "inet/networklayer/contract/IInterfaceTable.h"


extern "C" {
    Stack *make_smoltcp_stack(unsigned long moduleID, const char *macAddress, const char *ipAddress);
    void poll_smoltcp_stack(Stack *stack, unsigned long timestamp_ms);

    void smoltcp_send_eth_frame(unsigned long moduleId, const unsigned char *data, unsigned int size) {
        auto smolStack = check_and_cast<SmolTcpStack *>(getSimulation()->getModule(moduleId));
        smolStack->sendEthernetFrame(data, size);
    }

    unsigned int smoltcp_recv_eth_frame(unsigned long moduleId, unsigned char *buffer) {
        auto smolStack = check_and_cast<SmolTcpStack *>(getSimulation()->getModule(moduleId));
        return smolStack->recvEthernetFrame(buffer);
    }
}

Define_Module(SmolTcpStack);


void SmolTcpStack::sendEthernetFrame(unsigned const char *data, unsigned int size) {
    EV_TRACE << "smoltcp wants to send " << size << " bytes in module " << getId() << endl;

    inet::serializer::EthernetSerializer ser;
    inet::serializer::Context ctx;

    const int N = 2048;
    unsigned char bytes[N];
    // have to pad with zeros
    for(int i = 0; i < N; ++i)
        bytes[i] = data[i];

    // because the frame can't be shorter than 64 bytes, even if the payload is less
    inet::serializer::Buffer buf((void*)bytes, std::max(size+4, 64u));

    send(ser.deserializePacket(buf, ctx), "ethOut");
    EV_TRACE << "packet parsed and sent out on gate ethOut" << endl;
}

unsigned int SmolTcpStack::recvEthernetFrame(unsigned char *buffer) {
    EV_TRACE << "smoltcp wants to recv a frame" << endl;

    if (rxBuffer.empty()) {
        EV_TRACE << "but the rx buffer is empty" << endl;
        return 0;
    }

    auto &b = rxBuffer.front();

    for (int i = 0; i < b.size(); ++i)
        buffer[i] = b[i];

    EV_TRACE << "and we got a " << b.size() << " byte long frame for it" << endl;

    rxBuffer.pop_front();
    return b.size();
}

void SmolTcpStack::initialize(int stage)
{
    if (stage == inet::INITSTAGE_LAST) {
        auto ift = check_and_cast<inet::IInterfaceTable *>(getModuleByPath("^.interfaceTable"));
        auto intf = ift->getInterfaceByName("eth");
        stack = make_smoltcp_stack(getId(), intf->getMacAddress().str().c_str(), intf->getIPv4Address().str().c_str());
    }
    EV_TRACE << "stack made for module " << getId() << endl;
}

void SmolTcpStack::handleMessage(cMessage *msg)
{
    if (auto frame = dynamic_cast<inet::EthernetIIFrame*>(msg)) {
        EV_TRACE << "received an ethernet frame" << endl;

        inet::serializer::EthernetSerializer ser;
        inet::serializer::Context ctx;
        int N = 2048;
        unsigned char bytes[N];
        inet::serializer::Buffer buf(bytes, N);
        ser.serializePacket(frame, buf, ctx);

        std::vector<unsigned char> b;
        b.resize(N - buf.getRemainingSize());
        for (int i = 0; i < N - buf.getRemainingSize(); ++i)
            b[i] =  bytes[i];

        rxBuffer.push_back(b);


        EV_TRACE << "added a buffer of " << N - buf.getRemainingSize() << " bytes to rxbuffer" << endl;
        poll_smoltcp_stack(stack, simTime().inUnit(SIMTIME_MS));
    } else {
        EV_WARN << "got message of unsupported class " << msg->getClassName() << endl;
    }

    delete msg;
}
