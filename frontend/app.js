let map = L.map('map').setView([20, 0], 2);
L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png').addTo(map);


function clearMap() {
    map.eachLayer(layer => {
        if (layer instanceof L.Marker || layer instanceof L.Polyline)
            map.removeLayer(layer);
    });
}


// ======================
//      TRACEROUTE
// ======================
async function runTrace() {
    const host = document.getElementById('host').value;
    if (!host) return alert("Enter a host!");

    clearMap();

    const resp = await fetch(`http://127.0.0.1:8000/trace?host=${host}`);
    const hops = await resp.json();

    let latlngs = [];
    const tbody = document.querySelector("#hopTable tbody");
    tbody.innerHTML = "";

    hops.forEach(h => {
        let city = h.geo?.city ?? "-";
        let country = h.geo?.country ?? "-";
        let lat = h.geo?.lat;
        let lon = h.geo?.lon;

        // Add table row
        tbody.innerHTML += `
            <tr>
                <td>${h.hop}</td>
                <td>${h.ip}</td>
                <td>${h.rtt}</td>
                <td>${city}</td>
                <td>${country}</td>
            </tr>
        `;

        // Map
        if (lat && lon) {
            latlngs.push([lat, lon]);

            L.marker([lat, lon])
                .addTo(map)
                .bindPopup(`
                    <b>Hop ${h.hop}</b><br>
                    IP: ${h.ip}<br>
                    RTT: ${h.rtt} ms<br>
                    ${city}, ${country}
                `);
        }
    });

    if (latlngs.length > 1) {
        L.polyline(latlngs).addTo(map);
        map.fitBounds(latlngs);
    }
}


// ======================
//      DNS LOOKUP
// ======================
async function lookupDNS() {
    const host = document.getElementById("host").value;
    if (!host) return alert("Enter a host!");

    const resp = await fetch(`http://127.0.0.1:8000/dns?host=${host}`);
    const records = await resp.json();

    const tbody = document.querySelector("#dnsTable tbody");
    tbody.innerHTML = "";

    records.forEach(r => {
        tbody.innerHTML += `
            <tr>
                <td>${r.type}</td>
                <td>${r.value}</td>
            </tr>
        `;
    });
}

let ws = null;
let pingChart = null;
let rtts = [];

function startPing() {
    const host = document.getElementById("host").value;
    if (!host) return alert("Enter a host!");

    ws = new WebSocket(`ws://127.0.0.1:8000/ws_ping?host=${host}`);
    rtts = [];

    ws.onmessage = (event) => {
        const rtt = parseFloat(event.data);
        rtts.push(rtt);

        updateChart(rtt);
        updateStats();
    };

    setupChart();
}

function stopPing() {
    if (ws) ws.close();
    ws = null;
}

function setupChart() {
    const ctx = document.getElementById("pingChart").getContext("2d");
    pingChart = new Chart(ctx, {
        type: "line",
        data: {
            labels: [],
            datasets: [{
                label: "RTT (ms)",
                data: [],
                borderWidth: 2,
                borderColor: "green",
                tension: 0.2
            }]
        },
        options: {
            animation: false,
            scales: { y: { beginAtZero: true } }
        }
    });
}

function updateChart(rtt) {
    pingChart.data.labels.push("");
    pingChart.data.datasets[0].data.push(rtt);
    pingChart.update();
}

function updateStats() {
    const min = Math.min(...rtts);
    const max = Math.max(...rtts);
    const avg = rtts.reduce((a, b) => a + b, 0) / rtts.length;

    let loss = 0; // C++ currently doesn’t detect loss

    document.getElementById("pingStats").innerHTML =
        `Min: ${min.toFixed(2)} ms | 
         Avg: ${avg.toFixed(2)} ms | 
         Max: ${max.toFixed(2)} ms |
         Packets: ${rtts.length}`;
}
