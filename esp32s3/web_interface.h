#ifndef WEB_INTERFACE_H
#define WEB_INTERFACE_H

// HTML网页界面
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32S3 舵机控制</title>
  <style>
    body { font-family: Arial; padding: 20px; background: #f0f0f0; margin: 0; }
    .container { max-width: 1200px; margin: 0 auto; background: white; padding: 20px; border-radius: 8px; }
    h1 { text-align: center; color: #333; margin-bottom: 15px; }
    .main-layout { display: flex; gap: 20px; }
    .left-panel { flex: 1; min-width: 400px; }
    .right-panel { flex: 1; min-width: 400px; }
    .servo { margin: 10px 0; padding: 12px; background: #f9f9f9; border-radius: 5px; }
    .servo h3 { margin: 0 0 8px 0; font-size: 16px; }
    .controls { display: flex; justify-content: center; gap: 5px; align-items: center; flex-wrap: wrap; }
    .btn { padding: 8px 15px; border: none; border-radius: 5px; cursor: pointer; font-weight: bold; color: white; font-size: 14px; }
    .btn-left { background: #ff6b6b; }
    .btn-right { background: #4ecdc4; }
    .angle { min-width: 55px; text-align: center; font-size: 16px; font-weight: bold; }
    .log-section h3 { margin: 0 0 10px 0; }
    #log { background: #000; color: #0f0; padding: 10px; height: 500px; overflow-y: auto; font-family: monospace; font-size: 12px; border-radius: 5px; }
    .status { text-align: center; padding: 8px; margin-bottom: 15px; border-radius: 5px; font-weight: bold; }
    .connected { background: #d4edda; color: #155724; }
    .disconnected { background: #f8d7da; color: #721c24; }
    .batch-buttons { text-align: center; margin-top: 15px; padding-top: 15px; border-top: 2px solid #ddd; }
    input[type="number"] { width: 55px; padding: 5px; border: 1px solid #ccc; border-radius: 3px; }
  </style>
</head>
<body>
  <div class="container">
    <h1>ESP32S3 舵机控制</h1>
    <div id="status" class="status disconnected">断开连接</div>

    <div class="main-layout">
      <div class="left-panel">
        <h2 style="margin-top:0; font-size:18px;">舵机控制</h2>

        <div class="servo">
          <h3>舵机 0</h3>
          <div class="controls">
            <button class="btn btn-left" onclick="move(0, -1)">◀</button>
            <div class="angle" id="angle0">135°</div>
            <button class="btn btn-right" onclick="move(0, 1)">▶</button>
            <input type="number" id="input0" min="0" max="270" value="135">
            <button class="btn" style="background:#666;" onclick="setAngle(0)">设定</button>
          </div>
        </div>

        <div class="servo">
          <h3>舵机 1</h3>
          <div class="controls">
            <button class="btn btn-left" onclick="move(1, -1)">◀</button>
            <div class="angle" id="angle1">135°</div>
            <button class="btn btn-right" onclick="move(1, 1)">▶</button>
            <input type="number" id="input1" min="0" max="270" value="135">
            <button class="btn" style="background:#666;" onclick="setAngle(1)">设定</button>
          </div>
        </div>

        <div class="servo">
          <h3>舵机 2</h3>
          <div class="controls">
            <button class="btn btn-left" onclick="move(2, -1)">◀</button>
            <div class="angle" id="angle2">135°</div>
            <button class="btn btn-right" onclick="move(2, 1)">▶</button>
            <input type="number" id="input2" min="0" max="270" value="135">
            <button class="btn" style="background:#666;" onclick="setAngle(2)">设定</button>
          </div>
        </div>

        <div class="servo">
          <h3>舵机 3</h3>
          <div class="controls">
            <button class="btn btn-left" onclick="move(3, -1)">◀</button>
            <div class="angle" id="angle3">135°</div>
            <button class="btn btn-right" onclick="move(3, 1)">▶</button>
            <input type="number" id="input3" min="0" max="270" value="135">
            <button class="btn" style="background:#666;" onclick="setAngle(3)">设定</button>
          </div>
        </div>

        <div class="batch-buttons">
          <button class="btn" style="background:#ff9800; padding:10px 30px;" onclick="resetAll()">全部归零</button>
          <button class="btn" style="background:#9c27b0; padding:10px 30px; margin-left:10px;" onclick="centerAll()">全部居中</button>

          <div style="margin-top:15px;">
            <input type="number" id="allAngle" min="0" max="270" value="135" style="width:70px; padding:8px; font-size:14px;">
            <button class="btn" style="background:#2196F3; padding:10px 30px; margin-left:10px;" onclick="setAllAngle()">全部转到同角度</button>
          </div>

          <div style="margin-top:15px; padding-top:15px; border-top:1px solid #ddd;">
            <div style="font-weight:bold; margin-bottom:10px;">分别设定角度并同时执行：</div>
            <div style="display:flex; gap:10px; justify-content:center; align-items:center; flex-wrap:wrap;">
              <div>
                <label style="font-size:12px;">舵机0:</label>
                <input type="number" id="batch0" min="0" max="270" value="135" style="width:60px; padding:5px;">
              </div>
              <div>
                <label style="font-size:12px;">舵机1:</label>
                <input type="number" id="batch1" min="0" max="270" value="135" style="width:60px; padding:5px;">
              </div>
              <div>
                <label style="font-size:12px;">舵机2:</label>
                <input type="number" id="batch2" min="0" max="270" value="135" style="width:60px; padding:5px;">
              </div>
              <div>
                <label style="font-size:12px;">舵机3:</label>
                <input type="number" id="batch3" min="0" max="270" value="135" style="width:60px; padding:5px;">
              </div>
            </div>
            <button class="btn" style="background:#00bcd4; padding:10px 30px; margin-top:10px;" onclick="batchSetAll()">同时执行</button>
          </div>
        </div>
      </div>

      <div class="right-panel">
        <div class="log-section">
          <h2 style="margin-top:0; font-size:18px;">串口输出</h2>
          <div id="log"></div>
        </div>
      </div>
    </div>
  </div>

  <script>
    var ws;
    var angles = [135, 135, 135, 135];

    function initWebSocket() {
      ws = new WebSocket('ws://' + window.location.hostname + ':81');

      ws.onopen = function() {
        document.getElementById('status').textContent = '已连接';
        document.getElementById('status').className = 'status connected';
        addLog('连接成功');
      };

      ws.onclose = function() {
        document.getElementById('status').textContent = '断开连接';
        document.getElementById('status').className = 'status disconnected';
        addLog('断开连接，3秒后重连...');
        setTimeout(initWebSocket, 3000);
      };

      ws.onerror = function() {
        addLog('WebSocket 错误');
      };

      ws.onmessage = function(event) {
        addLog(event.data);
      };
    }

    function move(servo, direction) {
      fetch('/move?servo=' + servo + '&dir=' + direction)
        .then(response => response.json())
        .then(data => {
          if (data.angle !== undefined) {
            angles[servo] = data.angle;
            document.getElementById('angle' + servo).textContent = data.angle + '°';
            document.getElementById('input' + servo).value = data.angle;
          }
        });
    }

    function setAngle(servo) {
      var angle = parseInt(document.getElementById('input' + servo).value);
      if (angle < 0 || angle > 270) {
        alert('角度必须在0-270度之间');
        return;
      }
      fetch('/set?servo=' + servo + '&angle=' + angle)
        .then(response => response.json())
        .then(data => {
          if (data.angle !== undefined) {
            angles[servo] = data.angle;
            document.getElementById('angle' + servo).textContent = data.angle + '°';
          }
        });
    }

    function resetAll() {
      for (let i = 0; i < 4; i++) {
        fetch('/set?servo=' + i + '&angle=0')
          .then(response => response.json())
          .then(data => {
            if (data.angle !== undefined) {
              angles[data.servo] = data.angle;
              document.getElementById('angle' + data.servo).textContent = data.angle + '°';
              document.getElementById('input' + data.servo).value = data.angle;
            }
          });
      }
    }

    function centerAll() {
      for (let i = 0; i < 4; i++) {
        fetch('/set?servo=' + i + '&angle=135')
          .then(response => response.json())
          .then(data => {
            if (data.angle !== undefined) {
              angles[data.servo] = data.angle;
              document.getElementById('angle' + data.servo).textContent = data.angle + '°';
              document.getElementById('input' + data.servo).value = data.angle;
            }
          });
      }
    }

    function setAllAngle() {
      var angle = parseInt(document.getElementById('allAngle').value);
      if (angle < 0 || angle > 270) {
        alert('角度必须在0-270度之间');
        return;
      }
      for (let i = 0; i < 4; i++) {
        fetch('/set?servo=' + i + '&angle=' + angle)
          .then(response => response.json())
          .then(data => {
            if (data.angle !== undefined) {
              angles[data.servo] = data.angle;
              document.getElementById('angle' + data.servo).textContent = data.angle + '°';
              document.getElementById('input' + data.servo).value = data.angle;
            }
          });
      }
    }

    function batchSetAll() {
      var batchAngles = [];
      for (let i = 0; i < 4; i++) {
        var angle = parseInt(document.getElementById('batch' + i).value);
        if (angle < 0 || angle > 270) {
          alert('舵机' + i + '的角度必须在0-270度之间');
          return;
        }
        batchAngles.push(angle);
      }

      // 同时发送所有请求
      for (let i = 0; i < 4; i++) {
        fetch('/set?servo=' + i + '&angle=' + batchAngles[i])
          .then(response => response.json())
          .then(data => {
            if (data.angle !== undefined) {
              angles[data.servo] = data.angle;
              document.getElementById('angle' + data.servo).textContent = data.angle + '°';
              document.getElementById('input' + data.servo).value = data.angle;
            }
          });
      }
    }

    function addLog(message) {
      var log = document.getElementById('log');
      var timestamp = new Date().toLocaleTimeString();
      log.innerHTML += '[' + timestamp + '] ' + message + '<br>';
      log.scrollTop = log.scrollHeight;
    }

    // 初始化
    initWebSocket();

    // 获取初始角度
    fetch('/angles')
      .then(response => response.json())
      .then(data => {
        for (let i = 0; i < 4; i++) {
          angles[i] = data.angles[i];
          document.getElementById('angle' + i).textContent = data.angles[i] + '°';
          document.getElementById('input' + i).value = data.angles[i];
        }
      });
  </script>
</body>
</html>
)rawliteral";

#endif
