#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <math.h>
#include <Adafruit_ADS1X15.h>

#define MPU6050_ADDR 0x68

#define SDA_PIN 21
#define SCL_PIN 22

#define ADS1115_ADDR 0x49

// ===============================
// WIFI
// ===============================
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

WebServer server(80);

// ===============================
// ADS1115
// ===============================
Adafruit_ADS1115 ads;

// ===============================
// STATES
// ===============================
enum State {
  STANDING,
  WALKING,
  FOG
};

State currentState = STANDING;

// ===============================
// COUNTERS
// ===============================
int walkingCount = 0;
int standingCount = 0;
int fogCount = 0;
int normalWalkCount = 0;

// ===============================
// LATEST VALUES FOR DASHBOARD
// ===============================
float latestAx = 0;
float latestAy = 0;
float latestAz = 0;
float latestMovement = 0;
float latestGyro = 0;

int16_t latestFsrRaw = 0;

float latestFsrVoltage = 0;

String latestPressure = "NO PRESSURE";

// ===============================
// STATE TEXT
// ===============================
String stateText() {

  switch (currentState) {

    case STANDING:
      return "STANDING";

    case WALKING:
      return "WALKING";

    case FOG:
      return "FOG-LIKE CONDITION";
  }

  return "UNKNOWN";
}


// =====================================================
// DASHBOARD DATA
// =====================================================
void handleData() {

  String json = "{";

  json += "\"status\":\"" + stateText() + "\",";
  json += "\"ax\":" + String(latestAx, 2) + ",";
  json += "\"ay\":" + String(latestAy, 2) + ",";
  json += "\"az\":" + String(latestAz, 2) + ",";
  json += "\"movement\":" + String(latestMovement, 3) + ",";
  json += "\"gyro\":" + String(latestGyro, 2) + ",";
  json += "\"fsrRaw\":" + String(latestFsrRaw) + ",";
  json += "\"fsrVoltage\":" + String(latestFsrVoltage, 3) + ",";
  json += "\"pressure\":\"" + latestPressure + "\",";
  json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
  json += "\"uptime\":" + String(millis() / 1000);

  json += "}";

  server.send(
    200,
    "application/json",
    json
  );
}


// =====================================================
// BEAUTIFUL DASHBOARD
// =====================================================
void handleRoot() {

  String page = R"rawliteral(

<!DOCTYPE html>

<html lang="en">

<head>

<meta charset="UTF-8">

<meta name="viewport"
content="width=device-width, initial-scale=1.0">

<title>STEPUE Dashboard</title>


<style>

/* ===============================
   GLOBAL
================================ */

* {
  box-sizing: border-box;
}

body {

  margin: 0;

  font-family:
    Arial,
    Helvetica,
    sans-serif;

  color: #17213a;

  background:
    linear-gradient(
      135deg,
      #eef7ff,
      #f9f7ff 48%,
      #f1fff8
    );
}


/* ===============================
   HEADER
================================ */

.header {

  padding: 20px 30px;

  background:
    rgba(255,255,255,0.94);

  border-bottom:
    1px solid #dce8f5;

  display: flex;

  align-items: center;

  justify-content: space-between;

  gap: 18px;

  box-shadow:
    0 4px 18px
    rgba(30,70,120,.08);
}


.brand {

  display: flex;

  align-items: center;

  gap: 14px;
}


.logo {

  width: 56px;

  height: 56px;

  border-radius: 18px;

  background:
    linear-gradient(
      135deg,
      #087df2,
      #38a7ff
    );

  color: white;

  display: flex;

  align-items: center;

  justify-content: center;

  font-size: 28px;

  font-weight: 800;

  box-shadow:
    0 8px 20px
    rgba(13,110,253,.22);
}


.brand h1 {

  margin: 0;

  font-size: 30px;

  color: #15366f;
}


.brand p {

  margin: 3px 0 0;

  color: #60708f;

  font-size: 14px;
}


/* ===============================
   CONNECTION
================================ */

.connection {

  display: flex;

  align-items: center;

  gap: 12px;

  background: #e5f9ef;

  border:
    1px solid #bdeed5;

  border-radius: 18px;

  padding: 10px 17px;
}


.dot {

  width: 13px;

  height: 13px;

  background: #14b866;

  border-radius: 50%;

  box-shadow:
    0 0 0 5px
    rgba(20,184,102,.12);
}


.connection strong {

  color: #0a9551;
}


.connection small {

  display: block;

  margin-top: 2px;

  color: #53677f;
}


/* ===============================
   CONTAINER
================================ */

.container {

  max-width: 1500px;

  margin: 22px auto;

  padding:
    0 20px 28px;
}


/* ===============================
   TOP CARDS
================================ */

.top {

  display: grid;

  grid-template-columns:
    1.05fr
    repeat(5,1fr);

  gap: 16px;
}


.card {

  background:
    rgba(255,255,255,.95);

  border:
    1px solid #dfe9f5;

  border-radius: 20px;

  padding: 18px;

  box-shadow:
    0 8px 25px
    rgba(35,70,110,.08);
}


.metric {

  min-height: 185px;

  position: relative;

  overflow: hidden;
}


.metric h3,
.state h3,
.title {

  margin: 0;

  font-size: 17px;
}


.icon {

  width: 42px;

  height: 42px;

  border-radius: 50%;

  background: white;

  display: flex;

  align-items: center;

  justify-content: center;

  font-size: 21px;

  box-shadow:
    0 4px 12px
    rgba(0,0,0,.08);

  margin-bottom: 12px;
}


.value {

  margin-top: 14px;

  font-size: 27px;

  font-weight: 800;

  color: #15213c;
}


.unit {

  margin-top: 4px;

  color: #6b7892;

  font-size: 13px;
}


/* ===============================
   CARD COLORS
================================ */

.pressure {

  background:
    linear-gradient(
      145deg,
      #fff1f3,
      #fff9f9
    );

  border-color: #ffd7dc;
}


.fsr {

  background:
    linear-gradient(
      145deg,
      #f7efff,
      #fbf8ff
    );

  border-color: #e3d2ff;
}


.voltage {

  background:
    linear-gradient(
      145deg,
      #fff9e9,
      #fffdf5
    );

  border-color: #f8e3a8;
}


.movement {

  background:
    linear-gradient(
      145deg,
      #edf6ff,
      #f7fbff
    );

  border-color: #cfe5ff;
}


.gyro {

  background:
    linear-gradient(
      145deg,
      #eafbf4,
      #f7fffb
    );

  border-color: #c9efdc;
}


/* ===============================
   STATE CARD
================================ */

.state {

  min-height: 185px;

  text-align: center;

  background:
    linear-gradient(
      145deg,
      #e8fff3,
      #f7fffb
    );

  border-color: #c9efdc;
}


.stateCircle {

  width: 86px;

  height: 86px;

  margin:
    16px auto 12px;

  border-radius: 50%;

  display: flex;

  align-items: center;

  justify-content: center;

  background:
    radial-gradient(
      circle,
      #17ad64 0 18%,
      #c8f4dd 19% 100%
    );

  color: white;

  font-size: 30px;

  box-shadow:
    0 0 0 8px
    rgba(23,173,100,.1);
}


.pill {

  display: inline-block;

  min-width: 170px;

  padding: 11px 18px;

  border-radius: 30px;

  background: #16b965;

  color: white;

  font-weight: 800;
}


.walking {

  background: #0d82f5;
}


.fog {

  background: #e5484d;
}


/* ===============================
   SPARKLINES
================================ */

.spark {

  position: absolute;

  left: 16px;

  right: 16px;

  bottom: 10px;

  height: 38px;

  opacity: .65;
}


/* ===============================
   CHARTS
================================ */

.charts {

  margin-top: 16px;

  display: grid;

  grid-template-columns: 1fr 1fr;

  gap: 16px;
}


.chart {

  min-height: 290px;
}


canvas {

  width: 100%;

  height: 220px;

  margin-top: 12px;
}


/* ===============================
   BOTTOM
================================ */

.bottom {

  margin-top: 16px;

  display: grid;

  grid-template-columns:
    2fr 1fr;

  gap: 16px;
}


.live {

  display: grid;

  grid-template-columns:
    1fr 1fr;

  gap: 9px 16px;

  margin-top: 16px;
}


.row {

  display: grid;

  grid-template-columns:
    34px 1fr auto auto;

  align-items: center;

  gap: 9px;

  padding:
    12px 13px;

  border-radius: 13px;

  background: #f7faff;
}


.rowIcon {

  width: 29px;

  height: 29px;

  border-radius: 50%;

  background: white;

  display: flex;

  align-items: center;

  justify-content: center;
}


.label {

  color: #647594;
}


.val {

  font-weight: 800;
}


.unit2 {

  color: #72809a;

  font-size: 12px;
}


/* ===============================
   DEVICE INFO
================================ */

.info {

  margin-top: 16px;
}


.infoRow {

  display: grid;

  grid-template-columns:
    30px 1fr auto;

  gap: 9px;

  align-items: center;

  background: #f6f9fd;

  border-radius: 12px;

  padding:
    11px 13px;

  margin-bottom: 8px;
}


.infoLabel {

  color: #687895;
}


.infoVal {

  font-weight: 700;

  text-align: right;
}


/* ===============================
   FOOTER
================================ */

.footer {

  text-align: center;

  color: #75839c;

  padding-top: 15px;

  font-size: 12px;
}


/* ===============================
   RESPONSIVE
================================ */

@media(max-width:1100px) {

  .top {

    grid-template-columns:
      repeat(3,1fr);
  }

  .state {

    grid-column:
      span 3;
  }

  .bottom {

    grid-template-columns: 1fr;
  }
}


@media(max-width:720px) {

  .header {

    padding: 16px;

    flex-direction: column;

    align-items:
      flex-start;
  }

  .top,
  .charts,
  .bottom,
  .live {

    grid-template-columns:
      1fr;
  }

  .state {

    grid-column: auto;
  }

  .container {

    padding:
      0 10px 22px;
  }
}

</style>

</head>


<body>


<!-- ===============================
     HEADER
================================ -->

<header class="header">

<div class="brand">

<div class="logo">
S
</div>

<div>

<h1>
STEPUE
</h1>

<p>
Pre-FOG Detection &amp; Cueing Device
</p>

</div>

</div>


<div class="connection">

<span class="dot"></span>

<div>

<strong id="conn">
CONNECTED
</strong>

<small id="ip">
ESP32
</small>

</div>

</div>

</header>



<main class="container">


<!-- ===============================
     TOP
================================ -->

<section class="top">


<!-- STATE -->

<div class="card state">

<h3>
Current State
</h3>

<div class="stateCircle">
●
</div>

<div id="state"
     class="pill">

STANDING

</div>

</div>



<!-- PRESSURE -->

<div class="card metric pressure">

<div class="icon">
👣
</div>

<h3>
Pressure
</h3>

<div id="pressure"
     class="value">

NO PRESSURE

</div>

<div class="unit">
FSR level
</div>

<canvas
  class="spark"
  id="sp1">
</canvas>

</div>



<!-- FSR -->

<div class="card metric fsr">

<div class="icon">
◉
</div>

<h3>
FSR Raw
</h3>

<div id="fsr"
     class="value">

0

</div>

<div class="unit">
Counts
</div>

<canvas
  class="spark"
  id="sp2">
</canvas>

</div>



<!-- VOLTAGE -->

<div class="card metric voltage">

<div class="icon">
⚡
</div>

<h3>
FSR Voltage
</h3>

<div id="volt"
     class="value">

0.00

</div>

<div class="unit">
V
</div>

<canvas
  class="spark"
  id="sp3">
</canvas>

</div>



<!-- MOVEMENT -->

<div class="card metric movement">

<div class="icon">
〽
</div>

<h3>
Movement
</h3>

<div id="move"
     class="value">

0.000

</div>

<div class="unit">
g
</div>

<canvas
  class="spark"
  id="sp4">
</canvas>

</div>



<!-- GYRO -->

<div class="card metric gyro">

<div class="icon">
◌
</div>

<h3>
Gyro
</h3>

<div id="gyro"
     class="value">

0.00

</div>

<div class="unit">
°/s
</div>

<canvas
  class="spark"
  id="sp5">
</canvas>

</div>

</section>



<!-- ===============================
     CHARTS
================================ -->

<section class="charts">


<div class="card chart">

<h2 class="title">
👣 Pressure (FSR)
</h2>

<canvas
  id="pressureChart">
</canvas>

</div>



<div class="card chart">

<h2 class="title">
〽 Movement (Acceleration)
</h2>

<canvas
  id="movementChart">
</canvas>

</div>


</section>



<!-- ===============================
     BOTTOM
================================ -->

<section class="bottom">


<!-- LIVE SENSOR -->

<div class="card">

<h2 class="title">
▣ Sensor Readings (Live)
</h2>


<div class="live">


<div class="row">

<div class="rowIcon">
👣
</div>

<div class="label">
Pressure
</div>

<div id="pressure2"
     class="val">

NO PRESSURE

</div>

<div class="unit2">
level
</div>

</div>



<div class="row">

<div class="rowIcon">
〽
</div>

<div class="label">
Movement
</div>

<div id="move2"
     class="val">

0.000

</div>

<div class="unit2">
g
</div>

</div>



<div class="row">

<div class="rowIcon">
◉
</div>

<div class="label">
FSR Raw
</div>

<div id="fsr2"
     class="val">

0

</div>

<div class="unit2">
counts
</div>

</div>



<div class="row">

<div class="rowIcon">
◌
</div>

<div class="label">
Gyro
</div>

<div id="gyro2"
     class="val">

0.00

</div>

<div class="unit2">
°/s
</div>

</div>



<div class="row">

<div class="rowIcon">
⚡
</div>

<div class="label">
FSR Voltage
</div>

<div id="volt2"
     class="val">

0.00

</div>

<div class="unit2">
V
</div>

</div>



<div class="row">

<div class="rowIcon">
✓
</div>

<div class="label">
System Status
</div>

<div id="sys"
     class="val">

Running

</div>

<div class="unit2">
ESP32
</div>

</div>


</div>

</div>



<!-- DEVICE INFORMATION -->

<div class="card">

<h2 class="title">
⚙ Device Information
</h2>


<div class="info">


<div class="infoRow">

<div>▣</div>

<div class="infoLabel">
Device Name
</div>

<div class="infoVal">
STEPUE
</div>

</div>



<div class="infoRow">

<div>▦</div>

<div class="infoLabel">
ESP32 IP
</div>

<div id="ip2"
     class="infoVal">

--

</div>

</div>



<div class="infoRow">

<div>⌁</div>

<div class="infoLabel">
Wi-Fi SSID
</div>

<div class="infoVal">

rohit-Aspire-A324-51

</div>

</div>



<div class="infoRow">

<div>◷</div>

<div class="infoLabel">
Uptime
</div>

<div id="uptime"
     class="infoVal">

00:00:00

</div>

</div>


</div>

</div>


</section>



<div class="footer">

STEPUE POC • Live sensor dashboard • Data served directly by ESP32

</div>


</main>



<script>

/* ===============================
   DATA ARRAYS
================================ */

const MAX_POINTS = 50;

let pressureData = [];

let fsrData = [];

let voltageData = [];

let movementData = [];

let gyroData = [];



function pushData(array, value) {

  array.push(
    Number(value) || 0
  );

  if (
    array.length >
    MAX_POINTS
  ) {

    array.shift();

  }
}



/* ===============================
   MAIN CHART
================================ */

function drawChart(
  canvasId,
  data,
  lineColor,
  fillColor
) {

  const canvas =
    document.getElementById(
      canvasId
    );

  const rect =
    canvas.getBoundingClientRect();

  const dpr =
    window.devicePixelRatio || 1;

  const width =
    Math.max(300, rect.width);

  const height = 220;

  canvas.width =
    width * dpr;

  canvas.height =
    height * dpr;

  const ctx =
    canvas.getContext("2d");

  ctx.setTransform(
    dpr,
    0,
    0,
    dpr,
    0,
    0
  );

  ctx.clearRect(
    0,
    0,
    width,
    height
  );


  const left = 45;

  const right = 15;

  const top = 15;

  const bottom = 30;

  const chartWidth =
    width - left - right;

  const chartHeight =
    height - top - bottom;


  /* GRID */

  ctx.strokeStyle =
    "#dbe5f0";

  ctx.lineWidth = 1;


  for (
    let i = 0;
    i <= 4;
    i++
  ) {

    const y =
      top +
      chartHeight *
      i / 4;

    ctx.beginPath();

    ctx.moveTo(
      left,
      y
    );

    ctx.lineTo(
      left + chartWidth,
      y
    );

    ctx.stroke();
  }


  for (
    let i = 0;
    i <= 6;
    i++
  ) {

    const x =
      left +
      chartWidth *
      i / 6;

    ctx.beginPath();

    ctx.moveTo(
      x,
      top
    );

    ctx.lineTo(
      x,
      top + chartHeight
    );

    ctx.stroke();
  }


  if (
    data.length < 2
  ) {

    return;

  }


  let min =
    Math.min(...data);

  let max =
    Math.max(...data);


  if (
    max - min < 0.0001
  ) {

    max += 1;

    min -= 1;

  }


  const points = [];


  for (
    let i = 0;
    i < data.length;
    i++
  ) {

    const x =
      left +
      (
        i /
        (MAX_POINTS - 1)
      ) *
      chartWidth;


    const y =
      top +
      (
        1 -
        (
          (data[i] - min) /
          (max - min)
        )
      ) *
      chartHeight;


    points.push(
      [x,y]
    );

  }


  /* AREA */

  ctx.beginPath();


  points.forEach(
    (p,i) => {

      if (i === 0) {

        ctx.moveTo(
          p[0],
          p[1]
        );

      }
      else {

        ctx.lineTo(
          p[0],
          p[1]
        );

      }

    }
  );


  ctx.lineTo(
    points[points.length-1][0],
    top + chartHeight
  );


  ctx.lineTo(
    points[0][0],
    top + chartHeight
  );


  ctx.closePath();

  ctx.fillStyle =
    fillColor;

  ctx.fill();


  /* LINE */

  ctx.beginPath();


  points.forEach(
    (p,i) => {

      if (i === 0) {

        ctx.moveTo(
          p[0],
          p[1]
        );

      }
      else {

        ctx.lineTo(
          p[0],
          p[1]
        );

      }

    }
  );


  ctx.strokeStyle =
    lineColor;

  ctx.lineWidth = 2.5;

  ctx.stroke();


  /* LABELS */

  ctx.fillStyle =
    "#62738f";

  ctx.font =
    "12px Arial";

  ctx.fillText(
    max.toFixed(2),
    4,
    top + 5
  );

  ctx.fillText(
    min.toFixed(2),
    4,
    top + chartHeight
  );

  ctx.fillText(
    "Time",
    left + chartWidth / 2 - 14,
    height - 7
  );
}



/* ===============================
   SPARKLINE
================================ */

function drawSpark(
  canvasId,
  data,
  lineColor,
  fillColor
) {

  const canvas =
    document.getElementById(
      canvasId
    );

  const rect =
    canvas.getBoundingClientRect();

  const dpr =
    window.devicePixelRatio || 1;

  const width =
    Math.max(
      160,
      rect.width
    );

  const height = 38;


  canvas.width =
    width * dpr;

  canvas.height =
    height * dpr;


  const ctx =
    canvas.getContext("2d");


  ctx.setTransform(
    dpr,
    0,
    0,
    dpr,
    0,
    0
  );


  ctx.clearRect(
    0,
    0,
    width,
    height
  );


  if (
    data.length < 2
  ) {

    return;

  }


  let min =
    Math.min(...data);

  let max =
    Math.max(...data);


  if (
    max - min < 0.0001
  ) {

    max += 1;

    min -= 1;

  }


  ctx.beginPath();


  data.forEach(
    (value,i) => {

      const x =
        (
          i /
          (MAX_POINTS - 1)
        ) *
        width;


      const y =
        height -
        5 -
        (
          (value - min) /
          (max - min)
        ) *
        (height - 10);


      if (i === 0) {

        ctx.moveTo(
          x,
          y
        );

      }
      else {

        ctx.lineTo(
          x,
          y
        );

      }

    }
  );


  ctx.lineTo(
    width,
    height
  );

  ctx.lineTo(
    0,
    height
  );

  ctx.closePath();


  ctx.fillStyle =
    fillColor;

  ctx.fill();


  ctx.beginPath();


  data.forEach(
    (value,i) => {

      const x =
        (
          i /
          (MAX_POINTS - 1)
        ) *
        width;


      const y =
        height -
        5 -
        (
          (value - min) /
          (max - min)
        ) *
        (height - 10);


      if (i === 0) {

        ctx.moveTo(
          x,
          y
        );

      }
      else {

        ctx.lineTo(
          x,
          y
        );

      }

    }
  );


  ctx.strokeStyle =
    lineColor;

  ctx.lineWidth = 2;

  ctx.stroke();
}



/* ===============================
   UPTIME
================================ */

function formatUptime(
  seconds
) {

  seconds =
    Math.floor(
      Number(seconds) || 0
    );


  const hours =
    Math.floor(
      seconds / 3600
    );

  seconds %= 3600;


  const minutes =
    Math.floor(
      seconds / 60
    );

  const secs =
    seconds % 60;


  return (
    String(hours).padStart(2,"0")
    + ":" +
    String(minutes).padStart(2,"0")
    + ":" +
    String(secs).padStart(2,"0")
  );
}



/* ===============================
   UPDATE DASHBOARD
================================ */

async function updateDashboard() {

  try {

    const response =
      await fetch(
        "/data",
        {
          cache: "no-store"
        }
      );


    const data =
      await response.json();


    /* CONNECTION */

    document.getElementById(
      "conn"
    ).textContent =
      "CONNECTED";


    document.getElementById(
      "ip"
    ).textContent =
      data.ip;


    document.getElementById(
      "ip2"
    ).textContent =
      data.ip;



    /* STATE */

    const state =
      document.getElementById(
        "state"
      );


    state.textContent =
      data.status;


    state.className =
      "pill";


    if (
      data.status === "WALKING"
    ) {

      state.classList.add(
        "walking"
      );

    }


    if (
      data.status ===
      "FOG-LIKE CONDITION"
    ) {

      state.classList.add(
        "fog"
      );

    }



    /* VALUES */

    document.getElementById(
      "pressure"
    ).textContent =
      data.pressure;


    document.getElementById(
      "fsr"
    ).textContent =
      data.fsrRaw;


    document.getElementById(
      "volt"
    ).textContent =
      Number(
        data.fsrVoltage
      ).toFixed(2);


    document.getElementById(
      "move"
    ).textContent =
      Number(
        data.movement
      ).toFixed(3);


    document.getElementById(
      "gyro"
    ).textContent =
      Number(
        data.gyro
      ).toFixed(2);



    /* SECONDARY VALUES */

    document.getElementById(
      "pressure2"
    ).textContent =
      data.pressure;


    document.getElementById(
      "fsr2"
    ).textContent =
      data.fsrRaw;


    document.getElementById(
      "volt2"
    ).textContent =
      Number(
        data.fsrVoltage
      ).toFixed(2);


    document.getElementById(
      "move2"
    ).textContent =
      Number(
        data.movement
      ).toFixed(3);


    document.getElementById(
      "gyro2"
    ).textContent =
      Number(
        data.gyro
      ).toFixed(2);



    /* DATA HISTORY */

    pushData(
      pressureData,
      data.fsrRaw
    );


    pushData(
      fsrData,
      data.fsrRaw
    );


    pushData(
      voltageData,
      data.fsrVoltage
    );


    pushData(
      movementData,
      data.movement
    );


    pushData(
      gyroData,
      data.gyro
    );



    /* CHARTS */

    drawChart(
      "pressureChart",
      pressureData,
      "#ef3340",
      "rgba(239,51,64,.10)"
    );


    drawChart(
      "movementChart",
      movementData,
      "#087df2",
      "rgba(8,125,242,.10)"
    );



    /* SPARKLINES */

    drawSpark(
      "sp1",
      pressureData,
      "#ef6670",
      "rgba(239,102,112,.12)"
    );


    drawSpark(
      "sp2",
      fsrData,
      "#8c49ef",
      "rgba(140,73,239,.12)"
    );


    drawSpark(
      "sp3",
      voltageData,
      "#e5a500",
      "rgba(229,165,0,.12)"
    );


    drawSpark(
      "sp4",
      movementData,
      "#1683f4",
      "rgba(22,131,244,.12)"
    );


    drawSpark(
      "sp5",
      gyroData,
      "#1ab66c",
      "rgba(26,182,108,.12)"
    );



    /* UPTIME */

    document.getElementById(
      "uptime"
    ).textContent =
      formatUptime(
        data.uptime
      );


    document.getElementById(
      "sys"
    ).textContent =
      "Running";

  }

  catch(error) {

    document.getElementById(
      "conn"
    ).textContent =
      "DISCONNECTED";


    document.getElementById(
      "sys"
    ).textContent =
      "Offline";
  }

}


/* ===============================
   START
================================ */

updateDashboard();


setInterval(
  updateDashboard,
  500
);

</script>


</body>

</html>

)rawliteral";


  server.send(
    200,
    "text/html",
    page
  );
}


// =====================================================
// READ MPU6050
// =====================================================

void readMPU(

  float &ax,
  float &ay,
  float &az,

  float &gx,
  float &gy,
  float &gz

) {

  Wire.beginTransmission(
    MPU6050_ADDR
  );

  Wire.write(0x3B);

  Wire.endTransmission(
    false
  );


  Wire.requestFrom(
    MPU6050_ADDR,
    14,
    true
  );


  if (
    Wire.available() < 14
  ) {

    ax = 0;
    ay = 0;
    az = 0;

    gx = 0;
    gy = 0;
    gz = 0;

    return;
  }


  int16_t rawAx =
    (Wire.read() << 8)
    | Wire.read();


  int16_t rawAy =
    (Wire.read() << 8)
    | Wire.read();


  int16_t rawAz =
    (Wire.read() << 8)
    | Wire.read();


  // Temperature
  Wire.read();
  Wire.read();


  int16_t rawGx =
    (Wire.read() << 8)
    | Wire.read();


  int16_t rawGy =
    (Wire.read() << 8)
    | Wire.read();


  int16_t rawGz =
    (Wire.read() << 8)
    | Wire.read();


  // Accelerometer

  ax =
    rawAx / 16384.0;

  ay =
    rawAy / 16384.0;

  az =
    rawAz / 16384.0;


  // Gyroscope

  gx =
    rawGx / 131.0;

  gy =
    rawGy / 131.0;

  gz =
    rawGz / 131.0;
}


// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(
    115200
  );


  Wire.begin(
    SDA_PIN,
    SCL_PIN
  );


  delay(500);


  // ===============================
  // WAKE MPU6050
  // ===============================

  Wire.beginTransmission(
    MPU6050_ADDR
  );

  Wire.write(0x6B);

  Wire.write(0x00);

  byte error =
    Wire.endTransmission();


  Serial.println();

  Serial.println(
    "================================="
  );

  Serial.println(
    "          STEPUE - POC"
  );

  Serial.println(
    "================================="
  );


  // ===============================
  // MPU6050 CHECK
  // ===============================

  if (
    error == 0
  ) {

    Serial.println(
      "MPU6050: CONNECTED"
    );

  }

  else {

    Serial.println(
      "MPU6050: NOT FOUND"
    );

    Serial.print(
      "I2C ERROR: "
    );

    Serial.println(
      error
    );


    while(1) {

      delay(1000);

    }
  }


  // ===============================
  // ADS1115 CHECK
  // ===============================

  if (
    ads.begin(
      ADS1115_ADDR
    )
  ) {

    Serial.println(
      "ADS1115: CONNECTED"
    );

  }

  else {

    Serial.println(
      "ADS1115: NOT FOUND"
    );


    while(1) {

      delay(1000);

    }
  }


  // ±4.096V range

  ads.setGain(
    GAIN_ONE
  );


  Serial.println(
    "FSR: CONNECTED TO ADS1115 A0"
  );


  // ===============================
  // WIFI
  // ===============================

  Serial.println();

  Serial.println(
    "Connecting to laptop hotspot..."
  );


  WiFi.mode(
    WIFI_STA
  );


  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );


  int wifiAttempts = 0;


  while (

    WiFi.status()
    != WL_CONNECTED

    &&

    wifiAttempts < 30

  ) {

    delay(500);

    Serial.print(".");

    wifiAttempts++;

  }


  Serial.println();


  if (
    WiFi.status()
    == WL_CONNECTED
  ) {

    Serial.println(
      "WiFi: CONNECTED"
    );


    Serial.print(
      "ESP32 IP ADDRESS: "
    );


    Serial.println(
      WiFi.localIP()
    );


    server.on(
      "/",
      handleRoot
    );


    server.on(
      "/data",
      handleData
    );


    server.begin();


    Serial.println(
      "Web server started"
    );

  }

  else {

    Serial.println(
      "WiFi: NOT CONNECTED"
    );


    Serial.println(
      "Check hotspot name/password."
    );

  }


  Serial.println();

  Serial.println(
    "System Ready"
  );

  Serial.println(
    "---------------------------------"
  );
}


// =====================================================
// LOOP
// =====================================================

void loop() {


  // Keep dashboard responsive

  if (
    WiFi.status()
    == WL_CONNECTED
  ) {

    server.handleClient();

  }


  // ===============================
  // SENSOR VARIABLES
  // ===============================

  float ax, ay, az;

  float gx, gy, gz;


  readMPU(

    ax,
    ay,
    az,

    gx,
    gy,
    gz

  );


  // =================================
  // ACCELERATION MAGNITUDE
  // =================================

  float acceleration =

    sqrt(

      ax * ax +

      ay * ay +

      az * az

    );


  // =================================
  // GYRO MAGNITUDE
  // =================================

  float gyroMagnitude =

    sqrt(

      gx * gx +

      gy * gy +

      gz * gz

    );


  // =================================
  // MOVEMENT
  // =================================

  float movement =

    fabs(
      acceleration - 1.0
    );


  // =================================
  // BASIC CONDITIONS
  // =================================

  bool moving =

    (
      movement > 0.35
      ||
      gyroMagnitude > 20
    );


  bool still =

    (
      movement < 0.12
      &&
      gyroMagnitude < 10
    );


  // =================================
  // ABNORMAL / DISTURBED MOVEMENT
  // =================================

  /*
     High acceleration OR high rotation
     can indicate sudden / irregular movement.

     This is only a POC heuristic,
     not clinical FOG detection.
  */

  bool abnormalMovement =

    (
      movement > 0.70
      ||
      gyroMagnitude > 70
    );


  // =================================
  // AXIS-BASED DISTURBANCE
  // =================================

  bool sideDisturbance =

    (
      fabs(ay) > 0.65
    );


  bool forwardBackwardDisturbance =

    (
      fabs(ax) > 0.65
    );


  bool rotationDisturbance =

    (
      fabs(gx) > 50
      ||
      fabs(gy) > 50
      ||
      fabs(gz) > 50
    );


  bool disturbedMovement =

    abnormalMovement
    ||
    sideDisturbance
    ||
    forwardBackwardDisturbance
    ||
    rotationDisturbance;


  // =================================
  // FSR
  // =================================

  int16_t fsrRaw =

    ads.readADC_SingleEnded(
      0
    );


  float fsrVoltage =

    ads.computeVolts(
      fsrRaw
    );


  // =================================
  // PRESSURE
  // =================================

  String pressureLevel;


  if (
    fsrVoltage < 0.20
  ) {

    pressureLevel =
      "NO PRESSURE";

  }

  else if (
    fsrVoltage < 0.60
  ) {

    pressureLevel =
      "LOW PRESSURE";

  }

  else if (
    fsrVoltage < 1.20
  ) {

    pressureLevel =
      "MEDIUM PRESSURE";

  }

  else {

    pressureLevel =
      "HIGH PRESSURE";

  }


  // =================================
  // STATE MACHINE
  // =================================

  switch (
    currentState
  ) {


    // =================================
    // STANDING
    // =================================

    case STANDING:


      Serial.println(
        "STATUS: STANDING"
      );


      if (moving) {

        walkingCount++;

      }

      else {

        walkingCount = 0;

      }


      if (
        walkingCount >= 3
      ) {

        currentState =
          WALKING;


        walkingCount = 0;

        standingCount = 0;

        fogCount = 0;


        Serial.println();

        Serial.println(
          ">>>>>>>> WALKING DETECTED <<<<<<<<"
        );

        Serial.println();

      }


      break;



    // =================================
    // WALKING
    // =================================

    case WALKING:


      Serial.println(
        "STATUS: WALKING"
      );


      // STOPPED

      if (still) {

        standingCount++;

        fogCount = 0;

        normalWalkCount = 0;

      }


      // DISTURBED

      else if (
        disturbedMovement
      ) {

        fogCount++;

        standingCount = 0;

        normalWalkCount = 0;

      }


      // NORMAL WALKING

      else if (moving) {

        normalWalkCount++;

        fogCount = 0;

        standingCount = 0;

      }


      // FOG-LIKE

      if (
        fogCount >= 4
      ) {

        currentState =
          FOG;


        fogCount = 0;

        standingCount = 0;

        walkingCount = 0;

        normalWalkCount = 0;


        Serial.println();

        Serial.println(
          "********************************"
        );

        Serial.println(
          "     FOG-LIKE CONDITION"
        );

        Serial.println(
          "          DETECTED"
        );

        Serial.println(
          "********************************"
        );

        Serial.println();

      }


      // STANDING

      if (
        standingCount >= 5
      ) {

        currentState =
          STANDING;


        standingCount = 0;

        fogCount = 0;

        walkingCount = 0;

        normalWalkCount = 0;


        Serial.println();

        Serial.println(
          ">>>>>>>> MOVEMENT STOPPED <<<<<<<<"
        );

        Serial.println(
          ">>>>>>>> STANDING <<<<<<<<"
        );

        Serial.println();

      }


      break;



    // =================================
    // FOG
    // =================================

    case FOG:


      Serial.println(
        "STATUS: FOG-LIKE CONDITION"
      );


      // NORMAL WALKING RESUMED

      if (
        moving
        &&
        !disturbedMovement
      ) {

        normalWalkCount++;

      }

      else {

        normalWalkCount = 0;

      }


      // STANDING AFTER FOG

      if (still) {

        standingCount++;

      }

      else {

        standingCount = 0;

      }


      // RETURN TO WALKING

      if (
        normalWalkCount >= 4
      ) {

        currentState =
          WALKING;


        normalWalkCount = 0;

        standingCount = 0;

        fogCount = 0;


        Serial.println();

        Serial.println(
          ">>>>>>>> NORMAL WALKING RESUMED <<<<<<<<"
        );

        Serial.println();

      }


      // RETURN TO STANDING

      if (
        standingCount >= 5
      ) {

        currentState =
          STANDING;


        standingCount = 0;

        normalWalkCount = 0;

        fogCount = 0;


        Serial.println();

        Serial.println(
          ">>>>>>>> STANDING <<<<<<<<"
        );

        Serial.println();

      }


      break;

  }


  // =================================
  // SAVE DASHBOARD VALUES
  // =================================

  latestAx =
    ax;

  latestAy =
    ay;

  latestAz =
    az;

  latestMovement =
    movement;

  latestGyro =
    gyroMagnitude;

  latestFsrRaw =
    fsrRaw;

  latestFsrVoltage =
    fsrVoltage;

  latestPressure =
    pressureLevel;


  // =================================
  // SERIAL DEBUG
  // =================================

  Serial.print(
    "ACC: "
  );

  Serial.print(
    ax,
    2
  );

  Serial.print(
    ", "
  );

  Serial.print(
    ay,
    2
  );

  Serial.print(
    ", "
  );

  Serial.print(
    az,
    2
  );


  Serial.print(
    " | Movement: "
  );

  Serial.print(
    movement,
    3
  );


  Serial.print(
    " | Gyro: "
  );

  Serial.print(
    gyroMagnitude,
    2
  );


  Serial.print(
    " | Moving: "
  );

  Serial.print(
    moving
      ? "YES"
      : "NO"
  );


  Serial.print(
    " | Still: "
  );

  Serial.print(
    still
      ? "YES"
      : "NO"
  );


  Serial.print(
    " | Disturbed: "
  );

  Serial.print(
    disturbedMovement
      ? "YES"
      : "NO"
  );


  Serial.print(
    " | FSR Raw: "
  );

  Serial.print(
    fsrRaw
  );


  Serial.print(
    " | FSR Voltage: "
  );

  Serial.print(
    fsrVoltage,
    3
  );

  Serial.print(
    " V"
  );


  Serial.print(
    " | Pressure: "
  );

  Serial.print(
    pressureLevel
  );


  Serial.println();

  Serial.println(
    "---------------------------------"
  );


  // ~10 Hz

  delay(100);
}
