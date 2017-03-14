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


package smoltcp_net;

import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.node.ethernet.Eth1G;
import inet.node.ethernet.EtherSwitch;
import inet.node.inet.StandardHost;

network MultiClient
{
    @display("bgb=489,340");
    submodules:
        host0: StandardHost {
            @display("p=81,133");
        }
        smolTcpNode: SmolTcpNode {
            @display("p=406,172");
        }
        configurator: IPv4NetworkConfigurator {
            @display("p=64,37");
        }
        etherSwitch: EtherSwitch {
            @display("p=236,178");
        }
        host1: StandardHost {
            @display("p=81,211");
        }
        host2: StandardHost {
            @display("p=91,292");
        }
    connections:
        host0.ethg++ <--> Eth1G <--> etherSwitch.ethg++;
        host1.ethg++ <--> Eth1G <--> etherSwitch.ethg++;
        host2.ethg++ <--> Eth1G <--> etherSwitch.ethg++;
        smolTcpNode.ethg++ <--> Eth1G <--> etherSwitch.ethg++;
}
