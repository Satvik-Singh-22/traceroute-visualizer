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

struct Hop {
    int hop;
    string ip;
    double rtt;
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: traceroute_cpp <hostname>" << endl;
        return 1;
    }

    char* host = argv[1];
    addrinfo hints{}, *res;
    hints.ai_family = AF_INET;
    getaddrinfo(host, NULL, &hints, &res);

    sockaddr_in dest{};
    memcpy(&dest, res->ai_addr, sizeof(sockaddr_in));

    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct timeval timeout = {1, 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    vector<Hop> results;

    for (int ttl = 1; ttl <= 30; ttl++) {
        setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));

        icmphdr icmp{};
        icmp.type = ICMP_ECHO;
        icmp.code = 0;
        icmp.un.echo.id = getpid();
        icmp.un.echo.sequence = ttl;
        icmp.checksum = checksum(&icmp, sizeof(icmp));

        auto start = chrono::steady_clock::now();
        sendto(sock, &icmp, sizeof(icmp), 0, (sockaddr*)&dest, sizeof(dest));

        sockaddr_in reply{};
        socklen_t size = sizeof(reply);
        char buf[1024];

        int status = recvfrom(sock, buf, sizeof(buf), 0, (sockaddr*)&reply, &size);
        auto end = chrono::steady_clock::now();
        double rtt = chrono::duration<double, milli>(end - start).count();

        char ipstr[INET_ADDRSTRLEN] = "*";
        if (status > 0)
            inet_ntop(AF_INET, &(reply.sin_addr), ipstr, sizeof(ipstr));

        results.push_back({ttl, string(ipstr), rtt});

        if (status > 0 && reply.sin_addr.s_addr == dest.sin_addr.s_addr)
            break;
    }

    // Print valid JSON with no trailing commas
    cout << "[" << endl;
    for (size_t i = 0; i < results.size(); i++) {
        cout << "  {\"hop\": " << results[i].hop
             << ", \"ip\": \"" << results[i].ip
             << "\", \"rtt\": " << results[i].rtt << "}";

        if (i + 1 < results.size())
            cout << ",";
        cout << endl;
    }
    cout << "]" << endl;

    close(sock);
    return 0;
}
