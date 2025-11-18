# 🌐 Network Diagnostic Web App  
### **Traceroute • DNS Lookup • Real-Time Ping • Geo-Visualization**  
**Backend:** FastAPI + C++ (Raw Sockets)  
**Frontend:** HTML • CSS • JavaScript • LeafletJS • Chart.js  

---

## 📘 Overview

This project is a full-stack network diagnostic suite that integrates **high-performance C++ networking tools**, a **FastAPI backend**, and a **modern interactive JavaScript frontend**. It enables users to perform and visualize critical network diagnostics, including:

- 🔍 Traceroute with hop-wise Geo-Mapping  
- 🌐 DNS Record Lookup  
- 📡 Real-Time Ping with WebSocket streaming  
- 🗺 Interactive Map Visualization of network paths  

The system is optimized for speed, accuracy, and visual clarity—ideal for learning networking fundamentals or debugging latency and routing issues.

---

# 🚀 Features

## 🛰 Traceroute (C++ + Raw ICMP Sockets)

A custom traceroute tool built using raw ICMP sockets for precise hop discovery.

### **Capabilities**
- Sends ICMP Echo Requests with incrementing TTL  
- Measures RTT (milliseconds) for each hop  
- Performs GeoIP lookup (City, Country, Coordinates)  
- Visualizes traceroute path on a world map  
- Displays hop-wise IP, RTT, and geolocation  

### **Tech Used**
- Raw Sockets: `AF_INET`, `SOCK_RAW`  
- ICMP packet crafting & parsing  
- GeoIP API via Python `requests`  
- FastAPI to execute the C++ binary  

---

## 🔐 DNS Lookup (C++ + libresolv)

A high-performance DNS resolver built using `res_query()` and `libresolv`.

### **Supported Record Types**
| Type | Purpose |
|------|---------|
| **A / AAAA** | IPv4 & IPv6 Address Records |
| **CNAME** | Canonical Domain Name |
| **MX** | Mail Exchange Servers |
| **NS** | Authoritative Name Servers |
| **TXT** | Arbitrary text (SPF, domain verification, etc.) |

### **Tech Used**
- `res_query()`, `ns_msg`, `ns_rr` structures  
- Low-level DNS packet parsing  
- FastAPI wrapper returning JSON responses  

---

## 📡 Real-Time Ping (C++ Raw Sockets + WebSockets)

A continuous ICMP ping tool with live streaming to the frontend.

### **Features**
- Continuous RTT sampling  
- FastAPI WebSocket endpoint  
- Chart.js real-time dynamic graphs  
- Minimum / Average / Maximum latency calculation  

### **Tech Used**
- ICMP Echo packet creation  
- Async WebSockets for real-time data feed  
- Chart.js for smooth animated graphs  

---

## 🗺 Interactive Frontend (LeafletJS + Chart.js)

A clean, modern frontend offering real-time maps and charts.

### **LeafletJS**
- Displays traceroute hops on a world map  
- Connects hops with polylines  
- Popup showing hop details (RTT, Location, IP)  

### **Chart.js**
- Real-time latency graph  
- Smooth animations  
- Auto-updating line chart during ping  

### **Additional Frontend Stack**
- Vanilla JavaScript (ES6)  
- HTML5 + CSS3  
- `fetch()` for REST APIs  
- WebSocket for ping stream  

---

# 📁 Project Structure

```

traceroute-visualizer/
│
├── backend/
│   ├── main.py
│   ├── traceroute_cpp
│   ├── dns_tool
│   ├── ping_tool
│   └── *.cpp sources
│
├── frontend/
│   ├── index.html
│   ├── style.css
│   └── app.js
│
└── Makefile

````

---

## 🔧 Installation & Setup

### 1. Clone the repository
```bash
git clone https://github.com/Satvik-Singh-22/traceroute-visualizer.git
cd traceroute-visualizer
````

---

## 2. Install Python dependencies

Create a virtual environment:

```bash
python3 -m venv venv
source venv/bin/activate
```

Install packages:

```bash
pip install -r requirements.txt
```

---

## 3. Build the C++ Tools

Run:

```bash
make
```
### 🔐 Raw Socket Permissions (Required)

The C++ tools (`traceroute_cpp`, `ping_tool`) use raw ICMP sockets.  
Linux requires special capabilities to allow a binary to send ICMP without using sudo.

Run the following after building:

```bash
sudo setcap cap_net_raw+ep backend/traceroute_cpp
sudo setcap cap_net_raw+ep backend/ping_tool

This will compile:

* `traceroute_cpp`
* `dns_tool`
* `ping_tool`

All binaries will be placed automatically inside `backend/`.

The Makefile also applies:

```
setcap cap_net_raw+ep <binary>
```

which is required for raw ICMP sockets.

---

## 4. Run the FastAPI Backend

From the `backend/` directory:

```bash
cd backend
uvicorn main:app --reload --host 0.0.0.0 --port 8000
```

Backend runs at:

```
http://127.0.0.1:8000
```

---

## 5. Run the Frontend

From the `frontend/` folder:

```bash
cd frontend
python3 -m http.server 8080
```

Open in browser:

```
http://127.0.0.1:8080
```

---

## 🧪 Testing

### Traceroute:

```
http://127.0.0.1:8000/trace?host=google.com


```
![Alt text](images/image1.png)
![Alt text](images/image2.png)


### DNS Lookup:

```
http://127.0.0.1:8000/dns?host=google.com
```
![Alt text](images/image3.png)

### WebSocket Ping:

Browser automatically connects via:

```
ws://127.0.0.1:8000/ws_ping?host=google.com
```
![Alt text](images/image4.png)
---

## 🛠 Troubleshooting

### Binary not found:

Make sure binaries exist in `backend/`:

```bash
ls backend/
```

You should see:

```
traceroute_cpp
dns_tool
ping_tool
```

If not, run:

```bash
make clean
make
```

### Permission errors

Run:

```bash
sudo setcap cap_net_raw+ep backend/ping_tool
sudo setcap cap_net_raw+ep backend/traceroute_cpp
```

---
## 👨‍💻 Group Members - Group 22
- Satvik Singh (2023494)
- Rahul Kumar (2023420)
- Praveer Singh Chauhan (2023397)
- Rishabh Kumar (2023436)
- Sahil Amrawat (2023462)
````
