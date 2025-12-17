// web_interface.h - Web界面 (改编自esp32s3项目)
#ifndef WEB_INTERFACE_H
#define WEB_INTERFACE_H

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32-S3 展示控制台</title>
  <style>
    * {margin:0;padding:0;box-sizing:border-box;}
    :root {
      --bg-primary:#f8fafc;--bg-secondary:#ffffff;--bg-tertiary:#f1f5f9;
      --text-primary:#1e293b;--text-secondary:#64748b;--text-tertiary:#94a3b8;
      --brand:#2563eb;--brand-hover:#1d4ed8;--danger:#ef4444;--success:#10b981;
      --warning:#f59e0b;--border:#e2e8f0;
      --shadow-sm:0 1px 2px 0 rgb(0 0 0/0.05);--shadow-md:0 4px 6px -1px rgb(0 0 0/0.1);
    }
    body {font-family:-apple-system,BlinkMacSystemFont,"Segoe UI","Roboto",sans-serif;background:var(--bg-primary);color:var(--text-primary);line-height:1.5;}
    .app-container {min-height:100vh;display:flex;flex-direction:column;}
    .navbar {position:sticky;top:0;height:60px;background:var(--bg-secondary);border-bottom:1px solid var(--border);display:flex;align-items:center;padding:0 20px;z-index:1000;box-shadow:var(--shadow-sm);}
    .menu-icon {font-size:24px;cursor:pointer;padding:8px;margin-right:16px;color:var(--text-primary);border-radius:8px;transition:background 0.2s;}
    .menu-icon:hover {background:var(--bg-tertiary);}
    .navbar-title {font-size:18px;font-weight:600;color:var(--text-primary);}
    .sidebar-overlay {position:fixed;top:0;left:0;right:0;bottom:0;background:rgba(0,0,0,0.4);z-index:1100;display:none;opacity:0;transition:opacity 0.3s ease;}
    .sidebar-overlay.active {display:block;opacity:1;}
    .sidebar {position:fixed;top:0;left:-280px;width:280px;height:100%;background:var(--bg-secondary);z-index:1200;transition:left 0.3s ease;box-shadow:var(--shadow-md);display:flex;flex-direction:column;}
    .sidebar.active {left:0;}
    .sidebar-header {padding:24px 20px;border-bottom:1px solid var(--border);}
    .sidebar-title {font-size:16px;font-weight:600;color:var(--text-primary);}
    .sidebar-menu {flex:1;padding:12px 0;overflow-y:auto;}
    .menu-item {display:flex;align-items:center;padding:12px 20px;color:var(--text-secondary);text-decoration:none;font-size:15px;transition:all 0.2s;cursor:pointer;}
    .menu-item:hover {background:var(--bg-tertiary);color:var(--text-primary);}
    .menu-item.active {background:#eff6ff;color:var(--brand);font-weight:500;border-left:3px solid var(--brand);}
    .main-content {flex:1;padding:20px;}
    .view {display:none;}
    .view.active {display:block;}
    .status-badge {display:inline-flex;align-items:center;padding:8px 16px;border-radius:20px;font-size:14px;font-weight:500;margin-bottom:20px;}
    .status-badge.connected {background:#d1fae5;color:#065f46;}
    .status-badge.disconnected {background:#fee2e2;color:#991b1b;}
    .status-badge::before {content:'';width:8px;height:8px;border-radius:50%;margin-right:8px;background:currentColor;}
    .card {background:var(--bg-secondary);border-radius:12px;padding:20px;margin-bottom:16px;box-shadow:var(--shadow-md);border:1px solid var(--border);}
    .card-title {font-size:14px;font-weight:600;color:var(--text-secondary);margin-bottom:16px;text-transform:uppercase;letter-spacing:0.05em;}
    .servo-card {background:var(--bg-secondary);border-radius:12px;padding:20px;margin-bottom:16px;box-shadow:var(--shadow-md);border:1px solid var(--border);}
    .servo-header {font-size:15px;font-weight:600;color:var(--text-primary);margin-bottom:16px;}
    .servo-controls {display:flex;align-items:center;justify-content:center;gap:20px;}
    .angle-display {font-size:32px;font-weight:700;color:var(--brand);min-width:100px;text-align:center;font-variant-numeric:tabular-nums;}
    .btn {display:inline-flex;align-items:center;justify-content:center;padding:10px 20px;border:none;border-radius:8px;font-size:15px;font-weight:500;cursor:pointer;transition:all 0.2s;text-decoration:none;min-height:44px;}
    .btn:active {transform:scale(0.98);}
    .btn-primary {background:var(--brand);color:white;}
    .btn-primary:hover {background:var(--brand-hover);}
    .btn-danger {background:var(--danger);color:white;}
    .btn-danger:hover {background:#dc2626;}
    .btn-success {background:var(--success);color:white;}
    .btn-success:hover {background:#059669;}
    .btn-secondary {background:var(--bg-tertiary);color:var(--text-primary);}
    .btn-secondary:hover {background:#cbd5e1;}
    .btn-circle {width:64px;height:64px;border-radius:50%;padding:0;font-size:28px;font-weight:300;box-shadow:var(--shadow-md);}
    .btn-compact {width:40px;height:40px;padding:0;font-size:20px;min-height:auto;border-radius:6px;}
    .input-group {display:flex;flex-direction:column;margin-bottom:12px;}
    .input-label {font-size:13px;font-weight:600;color:var(--text-secondary);margin-bottom:8px;text-transform:uppercase;letter-spacing:0.05em;}
    .input-field {width:100%;padding:12px 16px;border:2px solid var(--border);border-radius:8px;font-size:16px;text-align:center;font-weight:600;color:var(--text-primary);transition:all 0.2s;}
    .input-field:focus {outline:none;border-color:var(--brand);box-shadow:0 0 0 3px rgba(37,99,235,0.1);}
    .log-panel {background:var(--bg-secondary);border-radius:12px;padding:16px;margin-top:20px;box-shadow:var(--shadow-md);border:1px solid var(--border);max-height:400px;display:flex;flex-direction:column;}
    .log-header {font-size:13px;font-weight:600;color:var(--text-secondary);margin-bottom:12px;text-transform:uppercase;letter-spacing:0.05em;flex-shrink:0;}
    .log-console {background:#0f172a;color:#10b981;padding:16px;flex:1;min-height:0;overflow-y:auto;font-family:'SF Mono',Monaco,'Cascadia Code',monospace;font-size:13px;border-radius:8px;line-height:1.6;}
    .control-grid {display:grid;grid-template-columns:repeat(2,1fr);gap:12px;}
    @media (min-width: 768px) {.control-grid {grid-template-columns:repeat(4,1fr);}}
  </style>
</head>
<body>
  <div class="app-container">
    <nav class="navbar">
      <div class="menu-icon" onclick="toggleSidebar()">☰</div>
      <div class="navbar-title">ESP32-S3 展示控制台</div>
    </nav>

    <div class="sidebar-overlay" id="sidebarOverlay" onclick="closeSidebar()"></div>

    <aside class="sidebar" id="sidebar">
      <div class="sidebar-header"><div class="sidebar-title">功能导航</div></div>
      <nav class="sidebar-menu">
        <a class="menu-item active" onclick="switchView('actions')">动作脚本</a>
        <a class="menu-item" onclick="switchView('servo360')">360度小力舵机 (0-3)</a>
        <a class="menu-item" onclick="switchView('servo180')">180度大力舵机 (4-7)</a>
        <a class="menu-item" onclick="switchView('stepper')">步进电机控制</a>
      </nav>
    </aside>

    <main class="main-content">
      <div id="status" class="status-badge disconnected">断开连接</div>

      <!-- 动作脚本页面 -->
      <div id="view-actions" class="view active">
        <div class="card" style="background:#dbeafe;border-color:#3b82f6;">
          <div style="display:flex;align-items:center;gap:12px;margin-bottom:12px;">
            <div style="font-size:24px;">🎬</div>
            <div>
              <div style="font-weight:600;color:#1e40af;margin-bottom:4px;">动作脚本控制</div>
              <div style="font-size:13px;color:#1e3a8a;line-height:1.6;">
                点击按钮执行预设的动作序列。执行过程中请勿手动操作其他控制。
              </div>
            </div>
          </div>
        </div>

        <div class="card">
          <div class="card-title">预设动作列表</div>
          <div style="display:grid;gap:16px;">
            <div class="card" style="background:var(--bg-tertiary);">
              <h3 style="font-size:16px;font-weight:600;margin-bottom:8px;color:var(--text-primary);">动作1: 垂直丝杆上下运动</h3>
              <p style="font-size:14px;color:var(--text-secondary);margin-bottom:12px;">垂直丝杆上升10000步，然后下降10000步</p>
              <button class="btn btn-primary" style="width:100%" onclick="runAction(1)">执行动作1</button>
            </div>

            <div class="card" style="background:var(--bg-tertiary);">
              <h3 style="font-size:16px;font-weight:600;margin-bottom:8px;color:var(--text-primary);">动作2: 大力6摆动</h3>
              <p style="font-size:14px;color:var(--text-secondary);margin-bottom:12px;">大力6: 125° → 170° → 80° → 125°</p>
              <button class="btn btn-success" style="width:100%" onclick="runAction(2)">执行动作2</button>
            </div>

            <div class="card" style="background:var(--bg-tertiary);">
              <h3 style="font-size:16px;font-weight:600;margin-bottom:8px;color:var(--text-primary);">动作3: 大力4+5联动</h3>
              <p style="font-size:14px;color:var(--text-secondary);margin-bottom:8px;">
                • 大力4(60→75) + 大力5(145→120)<br>
                • 大力4(75→35) + 大力5(120→170)<br>
                • 大力4(35→60) + 大力5(170→145)
              </p>
              <button class="btn btn-primary" style="width:100%" onclick="runAction(3)">执行动作3</button>
            </div>

            <div class="card" style="background:var(--bg-tertiary);">
              <h3 style="font-size:16px;font-weight:600;margin-bottom:8px;color:var(--text-primary);">动作4: 小力舵机左右摆动</h3>
              <p style="font-size:14px;color:var(--text-secondary);margin-bottom:12px;">小力舵机：向左1秒 → 向右2秒 → 向左1秒 → 停止</p>
              <button class="btn btn-success" style="width:100%" onclick="runAction(4)">执行动作4</button>
            </div>
          </div>
        </div>
      </div>

      <!-- 360度舵机控制 -->
      <div id="view-servo360" class="view">
        <div class="card" style="background:#fef3c7;border-color:#f59e0b;">
          <div style="display:flex;align-items:center;gap:12px;margin-bottom:12px;">
            <div style="font-size:24px;">⚠️</div>
            <div>
              <div style="font-weight:600;color:#92400e;margin-bottom:4px;">首次使用必读</div>
              <div style="font-size:13px;color:#78350f;line-height:1.6;">
                360度舵机上电时<strong>已禁用PWM信号</strong>，不会转动。<br>
                请按以下步骤操作：<br>
                1️⃣ 使用"调试模式"测试每个舵机，找到停止旋转的角度<br>
                2️⃣ 在"中点设置"中保存该角度<br>
                3️⃣ 之后才能安全使用"点动控制"
              </div>
            </div>
          </div>
        </div>

        <div class="card">
          <div class="card-title">步骤1: 调试模式 - 找到中点角度</div>
          <p style="color:var(--text-secondary);margin-bottom:16px;font-size:14px;">
            输入角度并点击"测试"，观察舵机是否停止旋转。通常中点在85-95度之间。
          </p>
          <div class="control-grid">
            <div class="input-group">
              <label class="input-label">舵机 0 测试角度</label>
              <input type="number" class="input-field" id="test0" min="0" max="180" value="90">
              <button class="btn btn-secondary" style="margin-top:8px;width:100%" onclick="testRotate(0)">测试</button>
            </div>
            <div class="input-group">
              <label class="input-label">舵机 1 测试角度</label>
              <input type="number" class="input-field" id="test1" min="0" max="180" value="90">
              <button class="btn btn-secondary" style="margin-top:8px;width:100%" onclick="testRotate(1)">测试</button>
            </div>
            <div class="input-group">
              <label class="input-label">舵机 2 测试角度</label>
              <input type="number" class="input-field" id="test2" min="0" max="180" value="90">
              <button class="btn btn-secondary" style="margin-top:8px;width:100%" onclick="testRotate(2)">测试</button>
            </div>
            <div class="input-group">
              <label class="input-label">舵机 3 测试角度</label>
              <input type="number" class="input-field" id="test3" min="0" max="180" value="90">
              <button class="btn btn-secondary" style="margin-top:8px;width:100%" onclick="testRotate(3)">测试</button>
            </div>
          </div>
          <button class="btn btn-danger" style="width:100%;margin-top:16px" onclick="stopAll360()">紧急停止所有360度舵机</button>
        </div>

        <div class="card">
          <div class="card-title">步骤2: 保存中点角度</div>
          <p style="color:var(--text-secondary);margin-bottom:16px;font-size:14px;">
            找到停止旋转的角度后，在此保存。之后点动控制会自动使用该中点。
          </p>
          <div class="control-grid">
            <div class="input-group">
              <label class="input-label">舵机 0 中点</label>
              <input type="number" class="input-field" id="center0" min="0" max="180" value="90">
              <button class="btn btn-primary" style="margin-top:8px;width:100%" onclick="setCenter(0)">设置</button>
            </div>
            <div class="input-group">
              <label class="input-label">舵机 1 中点</label>
              <input type="number" class="input-field" id="center1" min="0" max="180" value="90">
              <button class="btn btn-primary" style="margin-top:8px;width:100%" onclick="setCenter(1)">设置</button>
            </div>
            <div class="input-group">
              <label class="input-label">舵机 2 中点</label>
              <input type="number" class="input-field" id="center2" min="0" max="180" value="90">
              <button class="btn btn-primary" style="margin-top:8px;width:100%" onclick="setCenter(2)">设置</button>
            </div>
            <div class="input-group">
              <label class="input-label">舵机 3 中点</label>
              <input type="number" class="input-field" id="center3" min="0" max="180" value="90">
              <button class="btn btn-primary" style="margin-top:8px;width:100%" onclick="setCenter(3)">设置</button>
            </div>
          </div>
        </div>

        <div class="card">
          <div class="card-title">步骤3: 点动控制（需先完成步骤1和2）</div>
          <p style="color:var(--text-secondary);margin-bottom:16px;font-size:14px;">
            点击左右箭头，舵机会转动一下后自动返回中点停止。
          </p>
          <div class="control-grid">
            <div class="servo-card">
              <div class="servo-header">舵机 0</div>
              <div class="servo-controls">
                <button class="btn btn-danger btn-circle" onclick="pulse360(0,-1)">←</button>
                <button class="btn btn-success btn-circle" onclick="pulse360(0,1)">→</button>
              </div>
            </div>
            <div class="servo-card">
              <div class="servo-header">舵机 1</div>
              <div class="servo-controls">
                <button class="btn btn-danger btn-circle" onclick="pulse360(1,-1)">←</button>
                <button class="btn btn-success btn-circle" onclick="pulse360(1,1)">→</button>
              </div>
            </div>
            <div class="servo-card">
              <div class="servo-header">舵机 2</div>
              <div class="servo-controls">
                <button class="btn btn-danger btn-circle" onclick="pulse360(2,-1)">←</button>
                <button class="btn btn-success btn-circle" onclick="pulse360(2,1)">→</button>
              </div>
            </div>
            <div class="servo-card">
              <div class="servo-header">舵机 3</div>
              <div class="servo-controls">
                <button class="btn btn-danger btn-circle" onclick="pulse360(3,-1)">←</button>
                <button class="btn btn-success btn-circle" onclick="pulse360(3,1)">→</button>
              </div>
            </div>
          </div>
        </div>
      </div>

      <!-- 180度舵机控制 -->
      <div id="view-servo180" class="view">
        <h3 style="color:var(--text-secondary);margin-bottom:16px;font-size:14px;">180度大力舵机 (通道4-7)</h3>
        <div class="control-grid">
          <div class="servo-card">
            <div class="servo-header">舵机 4 (大力)</div>
            <div class="servo-controls">
              <button class="btn btn-danger btn-circle" onclick="adjustServo(4,-1)">−</button>
              <div class="angle-display" id="angle4">90°</div>
              <button class="btn btn-success btn-circle" onclick="adjustServo(4,1)">+</button>
            </div>
          </div>
          <div class="servo-card">
            <div class="servo-header">舵机 5 (大力)</div>
            <div class="servo-controls">
              <button class="btn btn-danger btn-circle" onclick="adjustServo(5,-1)">−</button>
              <div class="angle-display" id="angle5">90°</div>
              <button class="btn btn-success btn-circle" onclick="adjustServo(5,1)">+</button>
            </div>
          </div>
          <div class="servo-card">
            <div class="servo-header">舵机 6 (大力)</div>
            <div class="servo-controls">
              <button class="btn btn-danger btn-circle" onclick="adjustServo(6,-1)">−</button>
              <div class="angle-display" id="angle6">90°</div>
              <button class="btn btn-success btn-circle" onclick="adjustServo(6,1)">+</button>
            </div>
          </div>
          <div class="servo-card">
            <div class="servo-header">舵机 7 (大力)</div>
            <div class="servo-controls">
              <button class="btn btn-danger btn-circle" onclick="adjustServo(7,-1)">−</button>
              <div class="angle-display" id="angle7">90°</div>
              <button class="btn btn-success btn-circle" onclick="adjustServo(7,1)">+</button>
            </div>
          </div>
        </div>
      </div>

      <!-- 步进电机控制 -->
      <div id="view-stepper" class="view">
        <div class="card">
          <div class="card-title">垂直丝杆 (4个并联上下)</div>
          <div class="input-group">
            <label class="input-label">移动步数</label>
            <input type="number" class="input-field" id="vertSteps" value="200">
          </div>
          <div style="display:grid;grid-template-columns:1fr 1fr;gap:12px;">
            <button class="btn btn-success" onclick="moveVertical(1)">上升 ↑</button>
            <button class="btn btn-danger" onclick="moveVertical(-1)">下降 ↓</button>
          </div>
          <button class="btn btn-secondary" style="width:100%;margin-top:12px" onclick="homeVertical()">垂直归零</button>
          <div style="margin-top:16px;padding:12px;background:var(--bg-tertiary);border-radius:8px;">
            <div style="font-size:13px;color:var(--text-secondary);margin-bottom:4px;">当前位置</div>
            <div style="font-size:24px;font-weight:700;color:var(--brand);" id="vertPos">0 步</div>
          </div>
        </div>

        <div class="card">
          <div class="card-title">水平丝杆 (左右)</div>
          <div class="input-group">
            <label class="input-label">移动步数</label>
            <input type="number" class="input-field" id="horzSteps" value="200">
          </div>
          <div style="display:grid;grid-template-columns:1fr 1fr;gap:12px;">
            <button class="btn btn-danger" onclick="moveHorizontal(-1)">左移 ←</button>
            <button class="btn btn-success" onclick="moveHorizontal(1)">右移 →</button>
          </div>
          <button class="btn btn-secondary" style="width:100%;margin-top:12px" onclick="homeHorizontal()">水平归零</button>
          <div style="margin-top:16px;padding:12px;background:var(--bg-tertiary);border-radius:8px;">
            <div style="font-size:13px;color:var(--text-secondary);margin-bottom:4px;">当前位置</div>
            <div style="font-size:24px;font-weight:700;color:var(--brand);" id="horzPos">0 步</div>
          </div>
        </div>

        <div class="card">
          <button class="btn btn-danger" style="width:100%" onclick="stopSteppers()">紧急停止所有步进电机</button>
        </div>
      </div>

      <div class="log-panel">
        <div class="log-header">运行日志</div>
        <div class="log-console" id="log"></div>
      </div>
    </main>
  </div>

  <script>
    var ws;

    function initWebSocket() {
      ws = new WebSocket('ws://' + window.location.hostname + ':81');
      ws.onopen = function() {
        document.getElementById('status').textContent = '已连接';
        document.getElementById('status').className = 'status-badge connected';
        addLog('WebSocket连接成功');
        updateStatus();
      };
      ws.onclose = function() {
        document.getElementById('status').textContent = '断开连接';
        document.getElementById('status').className = 'status-badge disconnected';
        addLog('断开连接，3秒后重连...');
        setTimeout(initWebSocket, 3000);
      };
      ws.onerror = function() {addLog('WebSocket错误');};
      ws.onmessage = function(event) {addLog(event.data);};
    }

    function toggleSidebar() {
      document.getElementById('sidebar').classList.toggle('active');
      document.getElementById('sidebarOverlay').classList.toggle('active');
    }

    function closeSidebar() {
      document.getElementById('sidebar').classList.remove('active');
      document.getElementById('sidebarOverlay').classList.remove('active');
    }

    function switchView(viewId) {
      document.querySelectorAll('.view').forEach(v => v.classList.remove('active'));
      document.getElementById('view-' + viewId).classList.add('active');
      document.querySelectorAll('.menu-item').forEach(a => a.classList.remove('active'));
      event.target.classList.add('active');
      closeSidebar();
    }

    async function runAction(actionNum) {
      try {
        addLog('开始执行动作' + actionNum + '...');
        const response = await fetch('/action/action' + actionNum);
        const data = await response.json();
        if (data.status === 'ok') {
          addLog('动作' + actionNum + '已触发');
        }
      } catch (error) {
        console.error(error);
        addLog('动作' + actionNum + '执行失败');
      }
    }

    async function setCenter(servo) {
      const center = parseInt(document.getElementById('center' + servo).value);
      try {
        await fetch('/servo360/setcenter?servo=' + servo + '&center=' + center);
        addLog('360度舵机' + servo + '中点已设为' + center + '°');
      } catch (error) {console.error(error);}
    }

    async function pulse360(servo, dir) {
      try {
        await fetch('/servo360/pulse?servo=' + servo + '&dir=' + dir);
      } catch (error) {console.error(error);}
    }

    async function testRotate(servo) {
      const angle = parseInt(document.getElementById('test' + servo).value);
      try {
        await fetch('/servo360/test?servo=' + servo + '&angle=' + angle);
      } catch (error) {console.error(error);}
    }

    async function stopAll360() {
      try {
        await fetch('/servo360/stopall');
        addLog('所有360度舵机已停止');
      } catch (error) {console.error(error);}
    }

    async function adjustServo(servo, dir) {
      try {
        const response = await fetch('/servo180/move?servo=' + servo + '&dir=' + dir);
        const data = await response.json();
        document.getElementById('angle' + servo).textContent = data.angle + '°';
      } catch (error) {console.error(error);}
    }

    async function moveVertical(dir) {
      const steps = parseInt(document.getElementById('vertSteps').value) * dir;
      try {
        await fetch('/stepper/vertical?steps=' + steps);
        setTimeout(updateStatus, 100);
      } catch (error) {console.error(error);}
    }

    async function moveHorizontal(dir) {
      const steps = parseInt(document.getElementById('horzSteps').value) * dir;
      try {
        await fetch('/stepper/horizontal?steps=' + steps);
        setTimeout(updateStatus, 100);
      } catch (error) {console.error(error);}
    }

    async function homeVertical() {
      try {
        await fetch('/stepper/home?motor=0');
        setTimeout(updateStatus, 100);
      } catch (error) {console.error(error);}
    }

    async function homeHorizontal() {
      try {
        await fetch('/stepper/home?motor=1');
        setTimeout(updateStatus, 100);
      } catch (error) {console.error(error);}
    }

    async function stopSteppers() {
      try {
        await fetch('/stepper/stop');
        addLog('所有步进电机已停止');
      } catch (error) {console.error(error);}
    }

    async function updateStatus() {
      try {
        const response = await fetch('/status');
        const data = await response.json();
        document.getElementById('vertPos').textContent = data.vertical + ' 步';
        document.getElementById('horzPos').textContent = data.horizontal + ' 步';
        for (let i = 4; i < 8; i++) {
          const el = document.getElementById('angle' + i);
          if (el && data.servo180[i-4] !== undefined) {
            el.textContent = data.servo180[i-4] + '°';
          }
        }
        for (let i = 0; i < 4; i++) {
          const el = document.getElementById('center' + i);
          if (el && data.servo360Centers[i] !== undefined) {
            el.value = data.servo360Centers[i];
          }
        }
      } catch (error) {console.error(error);}
    }

    function addLog(message) {
      const log = document.getElementById('log');
      const timestamp = new Date().toLocaleTimeString();
      log.innerHTML += '[' + timestamp + '] ' + message + '\n';
      log.scrollTop = log.scrollHeight;
    }

    window.onload = function() {
      initWebSocket();
      updateStatus();
      setInterval(updateStatus, 2000);
    };
  </script>
</body>
</html>
)rawliteral";

#endif
