from fastapi import FastAPI, WebSocket
from fastapi.middleware.cors import CORSMiddleware
import subprocess
import asyncio
import subprocess
import json
import requests

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)

# GeoIP lookup
def geoip(ip):
    if ip == "*" or ip is None:
        return None
    try:
        url = f"http://ip-api.com/json/{ip}?fields=lat,lon,city,country,status"
        data = requests.get(url).json()
        if data["status"] == "success":
            return {
                "lat": data["lat"],
                "lon": data["lon"],
                "city": data["city"],
                "country": data["country"]
            }
    except:
        pass
    return None

@app.get("/trace")
def trace(host: str):

    # Call the C++ binary
    result = subprocess.run(
        ["./traceroute_cpp", host],
        capture_output=True,
        text=True
    )

    hops = json.loads(result.stdout)

    # Add geo data to each hop
    for h in hops:
        h["geo"] = geoip(h["ip"])

    return hops

@app.get("/dns")
def dns_lookup(host: str):
    result = subprocess.run(
        ["./dns_tool", host],
        capture_output=True,
        text=True
    )
    return json.loads(result.stdout)


@app.websocket("/ws_ping")
async def ws_ping(websocket: WebSocket):
    await websocket.accept()
    host = websocket.query_params.get("host")

    # Start C++ ping process
    proc = await asyncio.create_subprocess_exec(
        "./ping_tool", host,
        stdout=asyncio.subprocess.PIPE
    )

    try:
        while True:
            line = await proc.stdout.readline()
            if not line:
                break
            rtt = line.decode().strip()
            await websocket.send_text(rtt)

    except Exception:
        pass

    finally:
        proc.kill()