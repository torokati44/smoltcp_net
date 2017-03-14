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

import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.node.ethernet.Eth1G;
import inet.node.ethernet.EtherSwitch;
import inet.node.inet.StandardHost;

network MultiServer
{
    @display("bgb=513,433");
    submodules:
        host0: StandardHost {
            @display("p=142,116");
        }
        smolTcpNode0: SmolTcpNode {
            @display("p=349,116");
        }
        configurator: IPv4NetworkConfigurator {
            @display("p=64,37");
        }
        etherSwitch: EtherSwitch {
            @display("p=241,217");
        }
        host1: StandardHost {
            @display("p=103,176");
        }
        host2: StandardHost {
            @display("p=91,248");
        }
        smolTcpNode1: SmolTcpNode {
            @display("p=358,287");
        }
        host3: StandardHost {
            @display("p=103,318");
        }
        host4: StandardHost {
            @display("p=165,381");
        }
    connections:
        host0.ethg++ <--> Eth1G <--> etherSwitch.ethg++;
        host1.ethg++ <--> Eth1G <--> etherSwitch.ethg++;
        host2.ethg++ <--> Eth1G <--> etherSwitch.ethg++;
        host3.ethg++ <--> Eth1G <--> etherSwitch.ethg++;
        host4.ethg++ <--> Eth1G <--> etherSwitch.ethg++;
        smolTcpNode0.ethg++ <--> Eth1G <--> etherSwitch.ethg++;
        smolTcpNode1.ethg++ <--> Eth1G <--> etherSwitch.ethg++;
}
