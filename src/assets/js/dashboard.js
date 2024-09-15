// Dashboard.js for Fall Detection System

// Utility function to format date
function formatDate(date) {
  return date.toISOString().split('T')[0] + ' ' + date.toTimeString().split(' ')[0] + ' UTC';
}

// Simulated data for demonstration
let accelerometerData = [];
let gyroscopeData = [];
let gpsData = [];
let batteryData = [];

// Generate random data for demonstration
function generateData() {
  const now = new Date();
  accelerometerData.push({ x: now.getTime(), y: [Math.random() * 2 - 1, Math.random() * 2 - 1, Math.random() * 2 - 1] });
  gyroscopeData.push({ x: now.getTime(), y: [Math.random() * 360 - 180, Math.random() * 360 - 180, Math.random() * 360 - 180] });
  gpsData.push({ x: now.getTime(), y: [Math.random() * 180 - 90, Math.random() * 360 - 180] });
  batteryData.push({ x: now.getTime(), y: Math.max(0, Math.min(100, batteryData.length ? batteryData[batteryData.length - 1].y - Math.random() * 0.1 : 100)) });

  if (accelerometerData.length > 100) accelerometerData.shift();
  if (gyroscopeData.length > 100) gyroscopeData.shift();
  if (gpsData.length > 100) gpsData.shift();
  if (batteryData.length > 100) batteryData.shift();
}

// Update current readings
function updateReadings() {
  if (accelerometerData.length > 0) {
    const lastAccel = accelerometerData[accelerometerData.length - 1].y;
    document.getElementById('curAccelerometer').textContent = `X: ${lastAccel[0].toFixed(2)}, Y: ${lastAccel[1].toFixed(2)}, Z: ${lastAccel[2].toFixed(2)}`;
  }
  if (gyroscopeData.length > 0) {
    const lastGyro = gyroscopeData[gyroscopeData.length - 1].y;
    document.getElementById('curGyroscope').textContent = `X: ${lastGyro[0].toFixed(2)}, Y: ${lastGyro[1].toFixed(2)}, Z: ${lastGyro[2].toFixed(2)}`;
  }
  if (gpsData.length > 0) {
    const lastGPS = gpsData[gpsData.length - 1].y;
    document.getElementById('curGPS').textContent = `Lat: ${lastGPS[0].toFixed(4)}, Long: ${lastGPS[1].toFixed(4)}`;
  }
  if (batteryData.length > 0) {
    const lastBattery = batteryData[batteryData.length - 1].y;
    document.getElementById('curBattery').textContent = `${lastBattery.toFixed(0)}%`;
  }
}

// Chart options
const chartOptions = {
  chart: {
    height: 350,
    type: 'line',
    animations: {
      enabled: true,
      easing: 'linear',
      dynamicAnimation: {
        speed: 1000
      }
    },
    toolbar: {
      show: false
    },
    zoom: {
      enabled: false
    }
  },
  dataLabels: {
    enabled: false
  },
  stroke: {
    curve: 'smooth'
  },
  markers: {
    size: 0
  },
  xaxis: {
    type: 'datetime',
    range: 60000, // 1 minute
  },
  yaxis: {
    max: 1,
    min: -1,
    decimalsInFloat: 2,
  },
  legend: {
    show: false
  },
};

// Create charts
let accelerometerChart = new ApexCharts(document.querySelector("#accelerometer"), {
  ...chartOptions,
  title: { text: 'Accelerometer', align: 'left' },
  colors: ['#FF0000', '#00FF00', '#0000FF'],
  series: [
    { name: 'X', data: [] },
    { name: 'Y', data: [] },
    { name: 'Z', data: [] }
  ]
});
accelerometerChart.render();

let gyroscopeChart = new ApexCharts(document.querySelector("#gyroscope"), {
  ...chartOptions,
  title: { text: 'Gyroscope', align: 'left' },
  colors: ['#FF0000', '#00FF00', '#0000FF'],
  yaxis: { max: 180, min: -180, decimalsInFloat: 0 },
  series: [
    { name: 'X', data: [] },
    { name: 'Y', data: [] },
    { name: 'Z', data: [] }
  ]
});
gyroscopeChart.render();

let gpsChart = new ApexCharts(document.querySelector("#gps"), {
  ...chartOptions,
  title: { text: 'GPS', align: 'left' },
  colors: ['#FF0000', '#00FF00'],
  yaxis: { max: 90, min: -90, decimalsInFloat: 4 },
  series: [
    { name: 'Latitude', data: [] },
    { name: 'Longitude', data: [] }
  ]
});
gpsChart.render();

let batteryChart = new ApexCharts(document.querySelector("#battery"), {
  ...chartOptions,
  title: { text: 'Battery Level', align: 'left' },
  colors: ['#00FF00'],
  yaxis: { max: 100, min: 0, decimalsInFloat: 0 },
  series: [{ name: 'Battery', data: [] }]
});
batteryChart.render();

// Update charts
function updateCharts() {
  accelerometerChart.updateSeries([
    { data: accelerometerData.map(d => [d.x, d.y[0]]) },
    { data: accelerometerData.map(d => [d.x, d.y[1]]) },
    { data: accelerometerData.map(d => [d.x, d.y[2]]) }
  ]);
  gyroscopeChart.updateSeries([
    { data: gyroscopeData.map(d => [d.x, d.y[0]]) },
    { data: gyroscopeData.map(d => [d.x, d.y[1]]) },
    { data: gyroscopeData.map(d => [d.x, d.y[2]]) }
  ]);
  gpsChart.updateSeries([
    { data: gpsData.map(d => [d.x, d.y[0]]) },
    { data: gpsData.map(d => [d.x, d.y[1]]) }
  ]);
  batteryChart.updateSeries([
    { data: batteryData.map(d => [d.x, d.y]) }
  ]);
}

// Simulated fall detection
function detectFall() {
  if (accelerometerData.length > 0) {
    const lastAccel = accelerometerData[accelerometerData.length - 1].y;
    const magnitude = Math.sqrt(lastAccel[0] ** 2 + lastAccel[1] ** 2 + lastAccel[2] ** 2);
    if (magnitude > 1.8) {  // Threshold for fall detection
      document.getElementById('fallAlert').innerHTML = `
        <img src="../assets/images/alert.png" alt="alert" width="69px" height="69px" />
        <p style="color: red; font-weight: bold;">Fall Detected!</p>
      `;
      addAlertToList('Fall detected');
    } else {
      document.getElementById('fallAlert').innerHTML = `
        <img src="../assets/images/ok.png" alt="ok" width="69px" height="69px" />
        <p style="color: green;">No Fall Detected</p>
      `;
    }
  }
}

// Add alert to the list
function addAlertToList(message) {
  const alertList = document.getElementById('alertList');
  const newAlert = document.createElement('li');
  newAlert.className = 'timeline-item d-flex position-relative overflow-hidden';
  newAlert.innerHTML = `
    <div class="timeline-time text-dark flex-shrink-0 text-end">
      <span class="date">${formatDate(new Date()).split(' ')[0]}</span>
      <span class="time">${formatDate(new Date()).split(' ')[1]}</span>
    </div>
    <div class="timeline-badge-wrap d-flex flex-column align-items-center">
      <span class="timeline-badge border-2 border border-danger flex-shrink-0 my-8"></span>
      <span class="timeline-badge-border d-block flex-shrink-0"></span>
    </div>
    <div class="timeline-desc fs-3 text-dark mt-n1">${message}</div>
  `;
  alertList.insertBefore(newAlert, alertList.firstChild);
}

// Main loop
setInterval(() => {
  generateData();
  updateReadings();
  updateCharts();
  detectFall();
}, 1000);// Dashboard.js for Fall Detection System

// Utility function to format date
function formatDate(date) {
  return date.toISOString().split('T')[0] + ' ' + date.toTimeString().split(' ')[0] + ' UTC';
}

// Simulated data for demonstration
let accelerometerData = [];
let gyroscopeData = [];
let gpsData = [];
let batteryData = [];

// Generate random data for demonstration
function generateData() {
  const now = new Date();
  accelerometerData.push({ x: now.getTime(), y: [Math.random() * 2 - 1, Math.random() * 2 - 1, Math.random() * 2 - 1] });
  gyroscopeData.push({ x: now.getTime(), y: [Math.random() * 360 - 180, Math.random() * 360 - 180, Math.random() * 360 - 180] });
  gpsData.push({ x: now.getTime(), y: [Math.random() * 180 - 90, Math.random() * 360 - 180] });
  batteryData.push({ x: now.getTime(), y: Math.max(0, Math.min(100, batteryData.length ? batteryData[batteryData.length - 1].y - Math.random() * 0.1 : 100)) });

  if (accelerometerData.length > 100) accelerometerData.shift();
  if (gyroscopeData.length > 100) gyroscopeData.shift();
  if (gpsData.length > 100) gpsData.shift();
  if (batteryData.length > 100) batteryData.shift();
}

// Update current readings
function updateReadings() {
  if (accelerometerData.length > 0) {
    const lastAccel = accelerometerData[accelerometerData.length - 1].y;
    document.getElementById('curAccelerometer').textContent = `X: ${lastAccel[0].toFixed(2)}, Y: ${lastAccel[1].toFixed(2)}, Z: ${lastAccel[2].toFixed(2)}`;
  }
  if (gyroscopeData.length > 0) {
    const lastGyro = gyroscopeData[gyroscopeData.length - 1].y;
    document.getElementById('curGyroscope').textContent = `X: ${lastGyro[0].toFixed(2)}, Y: ${lastGyro[1].toFixed(2)}, Z: ${lastGyro[2].toFixed(2)}`;
  }
  if (gpsData.length > 0) {
    const lastGPS = gpsData[gpsData.length - 1].y;
    document.getElementById('curGPS').textContent = `Lat: ${lastGPS[0].toFixed(4)}, Long: ${lastGPS[1].toFixed(4)}`;
  }
  if (batteryData.length > 0) {
    const lastBattery = batteryData[batteryData.length - 1].y;
    document.getElementById('curBattery').textContent = `${lastBattery.toFixed(0)}%`;
  }
}

// Chart options
const chartOptions = {
  chart: {
    height: 350,
    type: 'line',
    animations: {
      enabled: true,
      easing: 'linear',
      dynamicAnimation: {
        speed: 1000
      }
    },
    toolbar: {
      show: false
    },
    zoom: {
      enabled: false
    }
  },
  dataLabels: {
    enabled: false
  },
  stroke: {
    curve: 'smooth'
  },
  markers: {
    size: 0
  },
  xaxis: {
    type: 'datetime',
    range: 60000, // 1 minute
  },
  yaxis: {
    max: 1,
    min: -1,
    decimalsInFloat: 2,
  },
  legend: {
    show: false
  },
};

// Create charts
let accelerometerChart = new ApexCharts(document.querySelector("#accelerometer"), {
  ...chartOptions,
  title: { text: 'Accelerometer', align: 'left' },
  colors: ['#FF0000', '#00FF00', '#0000FF'],
  series: [
    { name: 'X', data: [] },
    { name: 'Y', data: [] },
    { name: 'Z', data: [] }
  ]
});
accelerometerChart.render();

let gyroscopeChart = new ApexCharts(document.querySelector("#gyroscope"), {
  ...chartOptions,
  title: { text: 'Gyroscope', align: 'left' },
  colors: ['#FF0000', '#00FF00', '#0000FF'],
  yaxis: { max: 180, min: -180, decimalsInFloat: 0 },
  series: [
    { name: 'X', data: [] },
    { name: 'Y', data: [] },
    { name: 'Z', data: [] }
  ]
});
gyroscopeChart.render();

let gpsChart = new ApexCharts(document.querySelector("#gps"), {
  ...chartOptions,
  title: { text: 'GPS', align: 'left' },
  colors: ['#FF0000', '#00FF00'],
  yaxis: { max: 90, min: -90, decimalsInFloat: 4 },
  series: [
    { name: 'Latitude', data: [] },
    { name: 'Longitude', data: [] }
  ]
});
gpsChart.render();

let batteryChart = new ApexCharts(document.querySelector("#battery"), {
  ...chartOptions,
  title: { text: 'Battery Level', align: 'left' },
  colors: ['#00FF00'],
  yaxis: { max: 100, min: 0, decimalsInFloat: 0 },
  series: [{ name: 'Battery', data: [] }]
});
batteryChart.render();

// Update charts
function updateCharts() {
  accelerometerChart.updateSeries([
    { data: accelerometerData.map(d => [d.x, d.y[0]]) },
    { data: accelerometerData.map(d => [d.x, d.y[1]]) },
    { data: accelerometerData.map(d => [d.x, d.y[2]]) }
  ]);
  gyroscopeChart.updateSeries([
    { data: gyroscopeData.map(d => [d.x, d.y[0]]) },
    { data: gyroscopeData.map(d => [d.x, d.y[1]]) },
    { data: gyroscopeData.map(d => [d.x, d.y[2]]) }
  ]);
  gpsChart.updateSeries([
    { data: gpsData.map(d => [d.x, d.y[0]]) },
    { data: gpsData.map(d => [d.x, d.y[1]]) }
  ]);
  batteryChart.updateSeries([
    { data: batteryData.map(d => [d.x, d.y]) }
  ]);
}

// Simulated fall detection
function detectFall() {
  if (accelerometerData.length > 0) {
    const lastAccel = accelerometerData[accelerometerData.length - 1].y;
    const magnitude = Math.sqrt(lastAccel[0] ** 2 + lastAccel[1] ** 2 + lastAccel[2] ** 2);
    if (magnitude > 1.8) {  // Threshold for fall detection
      document.getElementById('fallAlert').innerHTML = `
        <img src="../assets/images/alert.png" alt="alert" width="69px" height="69px" />
        <p style="color: red; font-weight: bold;">Fall Detected!</p>
      `;
      addAlertToList('Fall detected');
    } else {
      document.getElementById('fallAlert').innerHTML = `
        <img src="../assets/images/ok.png" alt="ok" width="69px" height="69px" />
        <p style="color: green;">No Fall Detected</p>
      `;
    }
  }
}

// Add alert to the list
function addAlertToList(message) {
  const alertList = document.getElementById('alertList');
  const newAlert = document.createElement('li');
  newAlert.className = 'timeline-item d-flex position-relative overflow-hidden';
  newAlert.innerHTML = `
    <div class="timeline-time text-dark flex-shrink-0 text-end">
      <span class="date">${formatDate(new Date()).split(' ')[0]}</span>
      <span class="time">${formatDate(new Date()).split(' ')[1]}</span>
    </div>
    <div class="timeline-badge-wrap d-flex flex-column align-items-center">
      <span class="timeline-badge border-2 border border-danger flex-shrink-0 my-8"></span>
      <span class="timeline-badge-border d-block flex-shrink-0"></span>
    </div>
    <div class="timeline-desc fs-3 text-dark mt-n1">${message}</div>
  `;
  alertList.insertBefore(newAlert, alertList.firstChild);
}

// Main loop
setInterval(() => {
  generateData();
  updateReadings();
  updateCharts();
  detectFall();
}, 1000);