#include "ns3/command-line.h"

#include "ns3/config.h"

#include "ns3/double.h"

#include "ns3/string.h"

#include "ns3/log.h"

#include "ns3/yans-wifi-helper.h"

#include "ns3/mobility-helper.h"

#include "ns3/ipv4-address-helper.h"

#include "ns3/yans-wifi-channel.h"

#include "ns3/mobility-model.h"

#include "ns3/internet-stack-helper.h"

#include "ns3/netanim-module.h"

#include "ns3/applications-module.h"

#include "ns3/random-walk-2d-mobility-model.h"

#include <ostream>

#include "ns3/applications-module.h"

#include "ns3/core-module.h"

#include "ns3/csma-module.h"

#include "ns3/internet-module.h"

#define Infected_chance 10 //โอกาศติด 10%
#define Save_pos 2

int infected_list[] = {
    0,
    1,
    2,
    3,
    4
};

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("WifiSimpleAdhoc");

AnimationInterface * pAnim = 0;

struct rgb {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct rgb colors[] = {
    {
        252,
        44,
        44
    }, // ติดเชื้อ
    {
        127,
        255,
        46
    }, // ลูกค้า
    {
        46,
        131,
        255
    } // พ่อค้า
};
float speedLimit(float vector) {

    if (vector > 1) {
        return 1;
    } else if (vector < -1) {
        return -1;
    }
    return vector;

}

void ReceivePacket(Ptr < Socket > socket) {

    while (socket -> Recv()) {
        NS_LOG_UNCOND("Received one packet!");
        Time currentTime = Simulator::Now();


        Vector3D pos = socket->GetNode()->GetObject<MobilityModel>()->GetPosition();
        double distance = sqrt(pow(pos.x, 2) + pow(pos.y, 2));

        //duration time to infected ต้องหาข้อมูลมาเเก้
        if (currentTime.GetSeconds() >= 10) {
			if (distance > 100) {

				pAnim -> UpdateNodeColor(socket -> GetNode(), 0, 0, 255);
	//        	NS_LOG_UNCOND(distance);
			}
			else if (distance > 80) {
				pAnim -> UpdateNodeColor(socket -> GetNode(), 0, 170, 255);
	//        	NS_LOG_UNCOND(distance);
			 }
			else if (distance > 50) {
				pAnim -> UpdateNodeColor(socket -> GetNode(), 0, 255, 255);
	//        	NS_LOG_UNCOND(distance);
			}
        }




		NS_LOG_UNCOND(distance);
		Simulator::Schedule(Seconds(1), & ReceivePacket, socket);

    }

    //  Time currentTime = Simulator::Now();
    //  Vector currentPos = mob->GetPosition();
    // random 10% chance of infecting covid-19
    //  if(Infected_chance)  {
    //   set that node as infected!
    //
    //}
    //

}

static void GenerateTraffic(Ptr < Socket > socket, uint32_t pktSize,
    uint32_t pktCount, Time pktInterval) {
    if (pktCount > 0) {
        socket -> Send(Create < Packet > (pktSize));
        Simulator::Schedule(pktInterval, & GenerateTraffic,
            socket, pktSize, pktCount - 1, pktInterval);
    } else {
        socket -> Close();
    }

}
static void
CourseChange(std::string context, Ptr <
    const MobilityModel > mobility) {
    //  Vector pos = mobility->GetPosition ();
    //  Vector vel = mobility->GetVelocity ();
    //  std::cout << Simulator::Now () << ", model=" << mobility << ", POS: x=" << pos.x << ", y=" << pos.y
    //            << ", z=" << pos.z << "; VEL:" << vel.x << ", y=" << vel.y
    //            << ", z=" << vel.z << std::endl;
}

//void FilmSayYouNeedToMove(Ptr<RandomWalk2dMobilityModel> mob) {
////	Vector vel= mob->GetVelocity();
////	vel.x *= 10;
////	vel.y *= 100;
//	Vector currentPos = mob->GetPosition();
//
//
//	// walk to class room
//	Vector distPos = Vector(190, 200, 0);
////	float moveSpeed = 1.0f; // 1 unit per seconds
//
//	std::cout << "CALLED" << std::endl;
//
//	mob->SetPosition(Vector(
//			currentPos.x - (currentPos.x - distPos.x),
//			currentPos.y - (currentPos.y - distPos.y),
//			0
//		));
//
//}

void StudentMobility(Ptr < RandomWalk2dMobilityModel > mob) {

    // walking to class

    Time currentTime = Simulator::Now();

    Vector currentPos = mob -> GetPosition();

    //110 sec at class

    if (currentTime.GetSeconds() <= 110) {
        mob -> SetPosition(Vector(currentPos.x + 2.3, currentPos.y + 2.7, 0));

    }
    else if (200 >= currentTime.GetSeconds() && currentTime.GetSeconds() >= 110) {

    		mob -> SetPosition(Vector(currentPos.x - 1.5, currentPos.y - 1.9, 0));


     }

    Simulator::Schedule(Seconds(1), & StudentMobility, mob);

}

void TeacherMobility(Ptr < RandomWalk2dMobilityModel > mob) {

    Time currentTime = Simulator::Now();

    // walking to class

    Vector currentPos = mob -> GetPosition();

    if (currentTime.GetSeconds() <= 110) {
        mob -> SetPosition(Vector(currentPos.x + 1, currentPos.y + 1, 0));
    }

    //

    Simulator::Schedule(Seconds(1), & TeacherMobility, mob);
}

void SetNodeColor(Ptr < Node > node, uint8_t r, uint8_t g, uint8_t b) {

    pAnim -> UpdateNodeColor(node, r, g, b);

}

void SetNodeSize(uint32_t node, double width, double height) {

    pAnim -> UpdateNodeSize(node, width, height);

}



int main(int argc, char * argv[]) {
    std::string phyMode("DsssRate1Mbps");
    double rss = -80; // -dBm
    uint32_t packetSize = 1000; // bytes
    uint32_t numPackets = 200;
    double interval = 1.0; // seconds
    bool verbose = false;

    CommandLine cmd(__FILE__);
    cmd.AddValue("phyMode", "Wifi Phy mode", phyMode);
    cmd.AddValue("rss", "received signal strength", rss);
    cmd.AddValue("packetSize", "size of application packet sent", packetSize);
    cmd.AddValue("numPackets", "number of packets generated", numPackets);
    cmd.AddValue("interval", "interval (seconds) between packets", interval);
    cmd.AddValue("verbose", "turn on all WifiNetDevice log components", verbose);
    cmd.Parse(argc, argv);
    // Convert to time object
    Time interPacketInterval = Seconds(interval);

    // Fix non-unicast data rate to be the same as that of unicast
    Config::SetDefault("ns3::WifiRemoteStationManager::NonUnicastMode",
        StringValue(phyMode));

    NodeContainer c;
    c.Create(10);

    NodeContainer c2;
    c2.Create(3);
    NodeContainer t2;
    t2.Create(2);

    NodeContainer c3;
    c3.Create(15);

    // The below set of helpers will help us to put together the wifi NICs we want
    WifiHelper wifi;
    if (verbose) {
        wifi.EnableLogComponents(); // Turn on all Wifi logging
    }
    wifi.SetStandard(WIFI_STANDARD_80211b);

    YansWifiPhyHelper wifiPhy;

    wifiPhy.Set("RxGain", DoubleValue(0));
    wifiPhy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);

    YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss("ns3::FixedRssLossModel", "Rss", DoubleValue(rss));
    wifiPhy.SetChannel(wifiChannel.Create());

    // Add a mac and disable rate control
    WifiMacHelper wifiMac;
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
        "DataMode", StringValue(phyMode),
        "ControlMode", StringValue(phyMode));
    // Set it to adhoc mode
    wifiMac.SetType("ns3::AdhocWifiMac");
    NetDeviceContainer devices1 = wifi.Install(wifiPhy, wifiMac, c);
    NetDeviceContainer devices3 = wifi.Install(wifiPhy, wifiMac, c3);

    MobilityHelper mobility;
    mobility.SetPositionAllocator("ns3::RandomDiscPositionAllocator",
        "X", StringValue("60.0"),
        "Y", StringValue("30.0"),
        "Rho", StringValue("ns3::UniformRandomVariable[Min=15|Max=50]"));
    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
        "Mode", StringValue("Time"),
        "Time", StringValue("2s"),
        "Speed", StringValue("ns3::ConstantRandomVariable[Constant=1.0]"),
        "Bounds", StringValue("0|400|0|400"));

    mobility.Install(c);

    MobilityHelper mobility2;
    mobility2.SetPositionAllocator("ns3::RandomDiscPositionAllocator",
        "X", DoubleValue(100.0),
        "Y", DoubleValue(180.0),
        "Rho", StringValue("ns3::UniformRandomVariable[Min=15|Max=20]"));
    mobility2.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
        "Mode", StringValue("Time"),
        "Time", StringValue("2s"),
        "Speed", StringValue("ns3::ConstantRandomVariable[Constant=1.0]"),
        "Bounds", StringValue("0|400|0|400"));

    mobility2.Install(c2);

    MobilityHelper mobilityT2;
    mobilityT2.SetPositionAllocator("ns3::RandomDiscPositionAllocator",
        "X", DoubleValue(100.0),
        "Y", DoubleValue(180.0),
        "Rho", StringValue("ns3::UniformRandomVariable[Min=15|Max=20]"));
    mobilityT2.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
        "Mode", StringValue("Time"),
        "Time", StringValue("2s"),
        "Speed", StringValue("ns3::ConstantRandomVariable[Constant=1.0]"),
        "Bounds", StringValue("0|400|0|400"));

    mobilityT2.Install(t2);

    MobilityHelper mobility3;
    mobility3.SetPositionAllocator("ns3::RandomDiscPositionAllocator",
        "X", DoubleValue(80.0),
        "Y", DoubleValue(50.0),
        "Rho", StringValue("ns3::UniformRandomVariable[Min=20|Max=50]"));
    mobility3.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
        "Mode", StringValue("Time"),
        "Time", StringValue("2s"),
        "Speed", StringValue("ns3::ConstantRandomVariable[Constant=1.0]"),
        "Bounds", StringValue("0|400|0|400"));

    mobility3.Install(c3);

    Config::Connect("/NodeList/*/$ns3::MobilityModel/CourseChange",
        MakeCallback( & CourseChange));

    for (uint i = 0; i < c3.GetN(); i++) {
        Ptr < RandomWalk2dMobilityModel > mob = c3.Get(i) -> GetObject < RandomWalk2dMobilityModel > ();
        Simulator::ScheduleWithContext(c3.Get(i) -> GetId(), Seconds(50), & StudentMobility, mob);

    }

    InternetStackHelper internet3;
    internet3.Install(c);
    internet3.Install(c3);

    for (uint j = 0; j < t2.GetN(); j++) {
        Ptr < RandomWalk2dMobilityModel > mob = t2.Get(j) -> GetObject < RandomWalk2dMobilityModel > ();
        Simulator::ScheduleWithContext(t2.Get(j) -> GetId(), Seconds(10), & TeacherMobility, mob);

        Simulator::Schedule(Seconds(1.f), & SetNodeColor, t2.Get(j), 0, 255, 0);

    }

    for (uint i = 0; i < c.GetN(); i++) {
        Simulator::Schedule(Seconds(0.f), &SetNodeSize, c.Get(i) -> GetId(), 9.0, 9.0);
    }
    for (uint i = 0; i < c2.GetN(); i++) {
        Simulator::Schedule(Seconds(0.f), &SetNodeSize, c2.Get(i) -> GetId(), 9.0, 9.0);
    }
    for (uint i = 0; i < c3.GetN(); i++) {
        Simulator::Schedule(Seconds(0.f), &SetNodeSize, c3.Get(i) -> GetId(), 9.0, 9.0);
    }
    for (uint i = 0; i < t2.GetN(); i++) {
        Simulator::Schedule(Seconds(0.f), &SetNodeSize, t2.Get(i) -> GetId(), 9.0, 9.0);
    }


    Ipv4AddressHelper ipv4;
    NS_LOG_INFO("Assign IP Addresses.");
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer i = ipv4.Assign(devices1);

    ipv4.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer i3 = ipv4.Assign(devices3);

    TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");


    for (int i = 0; i < 15; i++) {
    	Ptr < Socket > recvSink = Socket::CreateSocket(c3.Get(i), tid);
    		InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), 80);
    		recvSink -> Bind(local);
    		recvSink -> SetRecvCallback(MakeCallback( & ReceivePacket));
    }

    for (int i = 0; i < 10; i++) {
       	Ptr < Socket > recvSink = Socket::CreateSocket(c.Get(i), tid);
       		InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), 80);
       		recvSink -> Bind(local);
       		recvSink -> SetRecvCallback(MakeCallback( & ReceivePacket));
       }
//    for (int i = 0; i < 3; i++) {
//           	Ptr < Socket > recvSink = Socket::CreateSocket(c2.Get(i), tid);
//           		InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), 80);
//           		recvSink -> Bind(local);
//           		recvSink -> SetRecvCallback(MakeCallback( & ReceivePacket));
//           }
//
//    for (int i = 0; i < 2; i++) {
//           	Ptr < Socket > recvSink = Socket::CreateSocket(t2.Get(i), tid);
//           		InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), 80);
//           		recvSink -> Bind(local);
//           		recvSink -> SetRecvCallback(MakeCallback( & ReceivePacket));
//           }





    Ptr < Socket > source = Socket::CreateSocket(c3.Get(1), tid);

    InetSocketAddress remote = InetSocketAddress(Ipv4Address("255.255.255.255"), 80);
    source -> SetAllowBroadcast(true);
    source -> Connect(remote);


    // Tracing
    wifiPhy.EnablePcap("wifi-simple-adhoc", devices1);
    wifiPhy.EnablePcap("wifi-simple-adhoc", devices3);

    // Output what we are doing
    NS_LOG_UNCOND("Testing " << numPackets << " packets sent with receiver rss " << rss);

    Simulator::ScheduleWithContext(source -> GetNode() -> GetId(),
        Seconds(0.5), & GenerateTraffic,
        source, packetSize, numPackets, interPacketInterval);


    //
    //  std::string animFile = "situation-1.xml" ;
    //
    //  pAnim = new AnimationInterface (animFile);

    pAnim = new AnimationInterface("situation.xml");

    Simulator::Stop(Seconds(200.0));

    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
