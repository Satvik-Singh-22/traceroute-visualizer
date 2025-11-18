# Compiler & flags
CXX = g++
CXXFLAGS = -O2 -Wall

# Directories
BACKEND_DIR = backend

# Source files
TRACER_SRC = tracer.cpp
DNS_BASIC_SRC = dns_lookup_basic.cpp
DNS_FULL_SRC = dns_lookup_full.cpp
DNS_TOOL_SRC = dns_tool.cpp
PING_SRC = ping_tool.cpp

# Binaries
TRACER_BIN = $(BACKEND_DIR)/traceroute_cpp
DNS_BIN = $(BACKEND_DIR)/dns_tool
PING_BIN = $(BACKEND_DIR)/ping_tool

# Build all tools
all: $(TRACER_BIN) $(DNS_BIN) $(PING_BIN)
	@echo "Build complete!"

############################
#  TRACEROUTE (ICMP)
############################
$(TRACER_BIN): $(TRACER_SRC)
	$(CXX) $(CXXFLAGS) $(TRACER_SRC) -o $(TRACER_BIN)
	sudo setcap cap_net_raw+ep $(TRACER_BIN)

############################
#  DNS TOOL (A, AAAA, MX, NS, TXT)
############################
$(DNS_BIN): $(DNS_TOOL_SRC) $(DNS_BASIC_SRC) $(DNS_FULL_SRC)
	$(CXX) $(CXXFLAGS) $(DNS_TOOL_SRC) $(DNS_BASIC_SRC) $(DNS_FULL_SRC) -o $(DNS_BIN) -lresolv

############################
#  PING TOOL (ICMP, real-time)
############################
$(PING_BIN): $(PING_SRC)
	$(CXX) $(CXXFLAGS) $(PING_SRC) -o $(PING_BIN)
	sudo setcap cap_net_raw+ep $(PING_BIN)

############################
#  CLEAN
############################
clean:
	rm -f $(TRACER_BIN) $(DNS_BIN) $(PING_BIN)

.PHONY: all clean
