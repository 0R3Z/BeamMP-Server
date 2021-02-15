#include "TServer.h"
#include "Client.h"
#include "Common.h"

TServer::TServer(int argc, char** argv) {
    info("BeamMP Server running version " + Application::ServerVersion());
    if (argc > 1) {
        Application::Settings.CustomIP = argv[1];
        size_t n = std::count(Application::Settings.CustomIP.begin(), Application::Settings.CustomIP.end(), '.');
        auto p = Application::Settings.CustomIP.find_first_not_of((".0123456789"));
        if (p != std::string::npos || n != 3 || Application::Settings.CustomIP.substr(0, 3) == ("127")) {
            Application::Settings.CustomIP.clear();
            warn("IP Specified is invalid! Ignoring");
        } else {
            info("server started with custom IP");
        }
    }
}

void TServer::RemoveClient(std::weak_ptr<TClient> WeakClientPtr) {
    if (!WeakClientPtr.expired()) {
        TClient& Client = *WeakClientPtr.lock();
        debug("removing client " + Client.GetName() + " (" + std::to_string(ClientCount()) + ")");
        Client.ClearCars();
        WriteLock Lock(_ClientsMutex);
        _Clients.erase(WeakClientPtr.lock());
    }
}

std::weak_ptr<TClient> TServer::InsertNewClient() {
    debug("inserting new client (" + std::to_string(ClientCount()) + ")");
    WriteLock Lock(_ClientsMutex);
    auto [Iter, Replaced] = _Clients.insert(std::make_shared<TClient>());
    return *Iter;
}

void TServer::ForEachClient(std::function<bool(std::weak_ptr<TClient>)> Fn) {
    ReadLock Lock(_ClientsMutex);
    for (auto& Client : _Clients) {
        if (!Fn(Client)) {
            break;
        }
    }
}

size_t TServer::ClientCount() const {
    ReadLock Lock(_ClientsMutex);
    return _Clients.size();
}