
#include "SmolTcpStack.h"

#include "inet/networklayer/contract/IInterfaceTable.h"
#include "inet/common/packet/chunk/BytesChunk.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/Units.h"

#include "inet/common/serializer/EthernetCRC.h"
#include "inet/linklayer/ethernet/EtherFrame_m.h"
#include "inet/linklayer/ethernet/Ethernet.h"

extern "C" {
    void init_smoltcp_logging();

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

    // "level" corresponds to Rust's Log::LogLevel enum
    void smoltcp_log_line(uint8_t level, const char *text) {
        // maps the Rust loglevel to opp loglevel
        static const LogLevel levelMapping[] = { LOGLEVEL_OFF, LOGLEVEL_ERROR,
                LOGLEVEL_WARN, LOGLEVEL_INFO, LOGLEVEL_DEBUG, LOGLEVEL_TRACE };
        printf("LOOOG %s\n", text);
        EV_LOG(levelMapping[level], "smoltcp_c") << text << std::endl;
    }

    Socket *make_smoltcp_tcp_socket();
    uint64_t add_smoltcp_tcp_socket(Stack *stack, Socket *socket);

    uint64_t make_add_smoltcp_tcp_socket(Stack *stack);
}

Define_Module(SmolTcpStack);


void SmolTcpStack::sendEthernetFrame(unsigned const char *data, unsigned int size) {
    EV_TRACE << "smoltcp wants to send " << size << " bytes in module " << getId() << endl;

    const int N = 2048;
    unsigned char bytes[N];

    // have to pad with zeros
    for(int i = 0; i < size; ++i)
        bytes[i] = data[i];

    for (; size < MIN_ETHERNET_FRAME_BYTES-4; ++size)
        bytes[size] = 0;

    auto chunk = std::make_shared<inet::BytesChunk>(bytes, size); // bytes, N ??
    chunk->markImmutable();
    auto pkt = new inet::Packet("aa", chunk);

    auto ethFcs = std::make_shared<inet::EthernetFcs>();
    ethFcs->setFcsMode(inet::FCS_COMPUTED);

    auto computedFcs = inet::serializer::ethernetCRC(bytes, size);
    ethFcs->setFcs(computedFcs);


    ethFcs->markImmutable();
    pkt->pushTrailer(ethFcs);
    send(pkt, "ethOut");
    EV_TRACE << "packet parsed and sent out on gate ethOut" << endl;
}

unsigned int SmolTcpStack::recvEthernetFrame(unsigned char *buffer) {
    EV_TRACE << "smoltcp wants to recv a frame" << endl;

    if (rxBuffer.empty()) {
        EV_TRACE << "but the rx buffer is empty" << endl;
        return 0;
    }

    auto &b = rxBuffer.front();

    for (size_t i = 0; i < b.size(); ++i)
        buffer[i] = b[i];

    EV_TRACE << "and we got a " << b.size() << " byte long frame for it" << endl;

    rxBuffer.pop_front();
    return b.size();
}

void SmolTcpStack::initialize(int stage)
{
    static bool first = true;
    if (first)
        init_smoltcp_logging();
    first = false;

    if (stage == inet::INITSTAGE_LAST) {
        auto ift = check_and_cast<inet::IInterfaceTable *>(getModuleByPath("^.interfaceTable"));
        auto intf = ift->getInterfaceByName("eth");

        std::cout << getId() << intf->getMacAddress() << intf->getIPv4Address();

        stack = make_smoltcp_stack(getId(), intf->getMacAddress().str().c_str(), intf->getIPv4Address().str().c_str());
        EV_TRACE << "stack made for module " << getId() << stack << endl;

        Socket *socket = make_smoltcp_tcp_socket();
        EV_TRACE << "made socket " << socket << endl;

        auto b = add_smoltcp_tcp_socket(stack, socket);
        EV_TRACE << "result:" << b << endl;

        b = make_add_smoltcp_tcp_socket(stack);
        EV_TRACE << "result:" << b << endl;
    }
}

void SmolTcpStack::handleMessage(cMessage *msg)
{
    if (auto packet = dynamic_cast<inet::Packet*>(msg)) {
        EV_TRACE << "received a packet" << endl;

        int N = 2048;
        unsigned char bytes[N];

        //packet->popHeader(B(PREAMBLE_BYTES + SFD_BYTES));
        auto chunk = packet->peekDataBytes();/* AllBytes();*/
        auto b = chunk->getBytes();
/*
        // add FCS
        const auto& fcsChunk = std::make_shared<inet::EthernetFcs>();
        uint32_t fcs = inet::serializer::ethernetCRC(b.data(), b.size());
        fcsChunk->setFcs(fcs);
        fcsChunk->setFcsMode(inet::FCS_COMPUTED);
        packet->insertTrailer(fcsChunk);*/

        chunk = packet->peekDataBytes();
        b = chunk->getBytes();

        rxBuffer.push_back(b);

        //EV_TRACE << "added a buffer of " << N - buf.getRemainingSize() << " bytes to rxbuffer" << endl;

        EV_TRACE << "start poll" << endl;
        poll_smoltcp_stack(stack, simTime().inUnit(SIMTIME_MS));
        EV_TRACE << "end poll" << endl;
    } else {
        EV_WARN << "got message of unsupported class " << msg->getClassName() << endl;
    }

    delete msg;
}
