```markdown
# Network Diagnostic Web App  
Traceroute • DNS Lookup • Real-Time Ping • Map Visualization  
Backend: FastAPI + C++ Tools  
Frontend: HTML + CSS + JavaScript + LeafletJS + Chart.js

---

## 🚀 Features

### ✅ Traceroute (C++ raw sockets)
- ICMP-based hop discovery  
- GeoIP lookup per hop  
- Visualized on Leaflet map  
- Hop table with RTT, city, and country  

### ✅ DNS Lookup (C++ resolver)
- A / AAAA  
- CNAME  
- MX  
- NS  
- TXT  
- Parsed with `res_query` and `libresolv`

### ✅ Real-Time Ping (C++ raw sockets + WebSockets)
- Continuous ICMP echo  
- Live RTT streaming through WebSockets  
- Chart.js visualization  
- Calculates Min / Avg / Max RTT  

---

## 📁 Project Structure

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
git clone <your-repo>
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

### DNS Lookup:

```
http://127.0.0.1:8000/dns?host=google.com
```

### WebSocket Ping:

Browser automatically connects via:

```
ws://127.0.0.1:8000/ws_ping?host=google.com
```

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

## 📜 License

MIT License

---

## 👨‍💻 Author

Satvik Singh

````

---

# 📦 **requirements.txt**

Here is the correct list of Python dependencies used by your FastAPI backend.

```txt
fastapi
uvicorn
requests
python-multipart
````

These cover:

* FastAPI server
* WebSockets
* File uploads (optional)
* GeoIP lookups via `requests`

---
