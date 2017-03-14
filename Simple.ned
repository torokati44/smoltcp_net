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
import inet.node.inet.StandardHost;


network Simple
{
    @display("bgb=349,193");
    submodules:
        standardHost: StandardHost {
            @display("p=64,82");
        }
        smolTcpNode: SmolTcpNode {
            @display("p=249,74");
        }
        configurator: IPv4NetworkConfigurator {
            @display("p=95,145");
        }
    connections:
        standardHost.ethg++ <--> Eth1G <--> smolTcpNode.ethg++;
}
