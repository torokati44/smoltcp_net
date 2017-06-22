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

package smoltcp_net;


import inet.linklayer.configurator.L2NodeConfigurator;
import inet.linklayer.ethernet.EtherQoSQueue;
import inet.linklayer.ethernet.EtherMAC;
import inet.linklayer.ethernet.EthernetInterface;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.configurator.ipv4.IPv4NodeConfigurator;
import inet.networklayer.ipv4.IPv4RoutingTable;


module SmolTcpNode
{
    @networkNode;
    @labels(node,ethernet-node);
    @display("bgb=286.96,325.72");
    gates:
        inout ethg[] @labels(EtherFrame-conn);
    submodules:
        stack: SmolTcpStack {
            @display("p=237.32,98.6");
        }
        interfaceTable: InterfaceTable {
            @display("p=61,40");
        }
        configurator: IPv4NodeConfigurator {
            @display("p=56.440002,112.880005");
            interfaceTableModule = "^.interfaceTable";
        }
        routingTable: IPv4RoutingTable {
            @display("p=40.12,178.16");
            interfaceTableModule = "^.interfaceTable";
        }
        eth: EthernetInterface {
            @display("p=202.64,214.88");
            interfaceTableModule = "^.interfaceTable";
            encapType = "EtherEncapDummy";
        }
    connections:
        eth.phys <--> { @display("m=s"); } <--> ethg++;
        eth.upperLayerOut --> stack.ethIn;
        stack.ethOut --> eth.upperLayerIn;
}
