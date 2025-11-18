#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <iostream>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
using namespace std;

// Compute standard Internet checksum for ICMP packets
unsigned short checksum(void *b, int len) {
    unsigned short *buf = (unsigned short*)b;
    unsigned int sum = 0;

    for (; len > 1; len -= 2)
        sum += *buf++;

    if (len == 1)
        sum += *(unsigned char*)buf;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return ~sum;
}

// For storing hop number, IP address, and RTT
struct Hop {
    int hop;
    string ip;
    double rtt;
};

int main(int argc, char* argv[]) {
    // Program expects one hostname target
    if (argc != 2) {
        cerr << "Usage: traceroute_cpp <hostname>" << endl;
        return 1;
    }

    char* host = argv[1];

    // Resolve hostname to IPv4 address
    addrinfo hints{}, *res;
    hints.ai_family = AF_INET;
    getaddrinfo(host, NULL, &hints, &res);

    sockaddr_in dest{};
    memcpy(&dest, res->ai_addr, sizeof(sockaddr_in));

    // Use raw socket to send/receive ICMP packets (requires root)
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    // 1-second receive timeout to avoid stalling on dead routers
    struct timeval timeout = {1, 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    vector<Hop> results;

    // Increase TTL from 1 to 30 to trace path hop-by-hop
    for (int ttl = 1; ttl <= 30; ttl++) {

        // Set packet TTL so routers decrement it
        setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));

        // Construct ICMP Echo Request packet
        icmphdr icmp{};
        icmp.type = ICMP_ECHO;
        icmp.code = 0;
        icmp.un.echo.id = getpid();
        icmp.un.echo.sequence = ttl;
        icmp.checksum = checksum(&icmp, sizeof(icmp));

        // Send packet and measure RTT
        auto start = chrono::steady_clock::now();
        sendto(sock, &icmp, sizeof(icmp), 0, (sockaddr*)&dest, sizeof(dest));

        sockaddr_in reply{};
        socklen_t size = sizeof(reply);
        char buf[1024];

        int status = recvfrom(sock, buf, sizeof(buf), 0, (sockaddr*)&reply, &size);
        auto end = chrono::steady_clock::now();
        double rtt = chrono::duration<double, milli>(end - start).count();

        // Default IP is "*", changed only if a response arrives
        char ipstr[INET_ADDRSTRLEN] = "*";
        if (status > 0)
            inet_ntop(AF_INET, &(reply.sin_addr), ipstr, sizeof(ipstr));

        results.push_back({ttl, string(ipstr), rtt});

        // Stop when the final destination responds
        if (status > 0 && reply.sin_addr.s_addr == dest.sin_addr.s_addr)
            break;
    }

    // Emit clean JSON array (used by your WebSocket visualizer)
    cout << "[" << endl;
    for (size_t i = 0; i < results.size(); i++) {
        cout << "  {\"hop\": " << results[i].hop
             << ", \"ip\": \"" << results[i].ip
             << "\", \"rtt\": " << results[i].rtt << "}";
        if (i + 1 < results.size()) cout << ",";
        cout << endl;
    }
    cout << "]" << endl;

    close(sock);
    return 0;
}
