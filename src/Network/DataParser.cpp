///
/// Created by Anonymous275 on 4/2/2020
///

#include <string>
#include "enet.hpp"
#include <vector>
#include <iostream>
#include <thread>
#include "../logger.h"
#include "../Settings.hpp"

void SendToAll(ENetHost *server, ENetPeer*peer,const std::string& Data,bool All, bool Reliable);
std::string HTTP_REQUEST(const std::string& IP,int port);
void Respond(const std::string& MSG, ENetPeer*peer);

void VehicleParser(std::string Packet,ENetPeer*peer,ENetHost*server){
    char Code = Packet.at(1);
    std::string Data = Packet.substr(3);
    switch(Code){ //Spawned Destroyed Switched/Moved Reset
        case 's':
            if(Data.at(0) == '0'){
                Packet = "Os:"+peer->Role+":"+peer->Name+":"+std::to_string(peer->serverVehicleID[0])+Packet.substr(4);
                peer->VehicleData = Packet;
            }
            SendToAll(server,peer,Packet,true,true);
            break;
        case 'd':
            peer->VehicleData.clear();
            SendToAll(server,peer,Packet,true,true);
            break;
        case 'm':
            break;
        case 'r':
            SendToAll(server,peer,Packet,false,true);
            break;
    }
}

void SyncVehicles(ENetHost*server,ENetPeer*peer){
    ENetPeer*ENetClient;
    for (int i = 0; i < server->connectedPeers; i++) {
        ENetClient = &server->peers[i];
        if (ENetClient != peer) {
            if(!ENetClient->VehicleData.empty()){
                enet_peer_send(peer, 0, enet_packet_create(ENetClient->VehicleData.c_str(),ENetClient->VehicleData.length()+1,1));
                enet_host_flush(server);
            }
        }
    }
}

void HTTP(ENetPeer*peer){
    if(peer != nullptr && !peer->DID.empty()){
        std::string a = HTTP_REQUEST("https://beamng-mp.com/entitlement?did="+peer->DID,443);
        if(!a.empty()){
            int pos = a.find('"');
            peer->Role = a.substr(pos+1,a.find('"',pos+1)-2);
            if(Debug)debug("ROLE -> " + peer->Role);
        }
    }
}

void GrabRole(ENetPeer*peer){
    std::thread t1(HTTP,peer);
    t1.detach();
}

void ParseData(ENetPacket*packet, ENetPeer*peer, ENetHost*server){
    std::string Packet = (char*)packet->data;
    if(Packet == "TEST")SyncVehicles(server,peer);
    char Code = Packet.at(0),SubCode = 0;
    if(Packet.length() > 1)SubCode = Packet.at(1);
    switch (Code) {
        case 'p':
            Respond("p",peer);
            return;
        case 'N':
            if(SubCode == 'R'){
                peer->Name = Packet.substr(2,Packet.find(':')-2);
                peer->DID = Packet.substr(Packet.find(':')+1);
                GrabRole(peer);
            }
            std::cout << "Name : " << peer->Name << std::endl;
            return;
        case 'O':
            if(Packet.length() > 1000) {
                std::cout << "Received data from: " << peer->Name << " Size: " << Packet.length() << std::endl;
            }
            VehicleParser(Packet,peer,server);
            return;
    }
    //V to Z
    if(Packet.length() > 1000){
        std::cout << "Received data from: " << peer->Name << " Size: " << Packet.length() << std::endl;
    }

    if(Code <= 90 && Code >= 86)SendToAll(server,peer,Packet,false,false);
    if(Debug)debug("Data : " + Packet);
}
