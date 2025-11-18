#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

// Compute ICMP checksum (Internet checksum algorithm)
unsigned short checksum(void *b, int len) {
    unsigned short *buf = (unsigned short*)b;
    unsigned int sum = 0;

    while (len > 1) {
        sum += *buf++;
        len -= 2;
    }
    if (len == 1)
        sum += *(unsigned char*)buf;

    // Fold 32-bit sum to 16 bits
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return ~sum;
}

int main(int argc, char* argv[]) {
    // Expect exactly one hostname
    if (argc != 2) {
        cerr << "Usage: ping_cpp <hostname>" << endl;
        return 1;
    }

    string host = argv[1];

    // Resolve hostname to IPv4 address
    addrinfo hints{}, *res;
    hints.ai_family = AF_INET;
    getaddrinfo(host.c_str(), NULL, &hints, &res);

    sockaddr_in dest{};
    memcpy(&dest, res->ai_addr, sizeof(sockaddr_in));

    // Create raw ICMP socket (requires root)
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    cout.setf(std::ios::unitbuf); // auto-flush for WebSocket reader

    // Send echo requests in a loop
    for (int seq = 1; seq <= 9999; seq++) {
        icmphdr icmp{};
        icmp.type = ICMP_ECHO;
        icmp.un.echo.id = getpid();    // identifier
        icmp.un.echo.sequence = seq;   // sequence number
        icmp.checksum = checksum(&icmp, sizeof(icmp));

        // Send packet
        auto start = chrono::steady_clock::now();
        sendto(sock, &icmp, sizeof(icmp), 0,
               (sockaddr*)&dest, sizeof(dest));

        // Receive reply
        char buf[2048];
        sockaddr_in reply{};
        socklen_t len = sizeof(reply);

        int status = recvfrom(sock, buf, sizeof(buf), 0,
                              (sockaddr*)&reply, &len);
        auto end = chrono::steady_clock::now();

        // Print RTT in milliseconds (one line per ping)
        if (status > 0) {
            double rtt = chrono::duration<double, milli>(end - start).count();
            cout << rtt << endl;
        }

        usleep(500000); // 500ms between pings
    }

    return 0;
}
