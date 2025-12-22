// web_interface.h - Web界面HTML (12舵机控制系统)
#ifndef WEB_INTERFACE_H
#define WEB_INTERFACE_H

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>ESP32-S3 展示控制系统</title>
  <style>
    :root {
      --primary: #2196F3;
      --success: #4CAF50;
      --danger: #f44336;
      --warning: #FF9800;
      --bg-primary: #ffffff;
      --bg-secondary: #f5f5f5;
      --text-primary: #212121;
      --text-secondary: #757575;
      --text-tertiary: #9E9E9E;
      --border: #e0e0e0;
      --shadow-sm: 0 2px 4px rgba(0,0,0,0.1);
      --shadow-md: 0 4px 8px rgba(0,0,0,0.15);
    }

    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
    }

    body {
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
      background: var(--bg-secondary);
      color: var(--text-primary);
      overflow-x: hidden;
    }

    /* 顶栏 */
    .header {
      background: var(--primary);
      color: white;
      padding: 16px 20px;
      box-shadow: var(--shadow-md);
      position: sticky;
      top: 0;
      z-index: 100;
      display: flex;
      justify-content: space-between;
      align-items: center;
    }

    .header h1 {
      font-size: 20px;
      font-weight: 600;
    }

    .ws-status {
      display: flex;
      align-items: center;
      gap: 8px;
      font-size: 14px;
      padding: 6px 12px;
      background: rgba(255,255,255,0.15);
      border-radius: 20px;
    }

    .ws-indicator {
      width: 8px;
      height: 8px;
      border-radius: 50%;
      background: #f44336;
      animation: pulse 2s infinite;
    }

    .ws-indicator.connected {
      background: #4CAF50;
      animation: none;
    }

    @keyframes pulse {
      0%, 100% { opacity: 1; }
      50% { opacity: 0.5; }
    }

    /* 导航栏 */
    .nav {
      background: var(--bg-primary);
      border-bottom: 1px solid var(--border);
      display: flex;
      overflow-x: auto;
      position: sticky;
      top: 52px;
      z-index: 99;
    }

    .nav-tab {
      flex: 1;
      min-width: 100px;
      padding: 14px 16px;
      text-align: center;
      background: none;
      border: none;
      border-bottom: 2px solid transparent;
      color: var(--text-secondary);
      font-size: 14px;
      cursor: pointer;
      transition: all 0.2s;
      white-space: nowrap;
    }

    .nav-tab.active {
      color: var(--primary);
      border-bottom-color: var(--primary);
      font-weight: 600;
    }

    /* 主容器 */
    .container {
      max-width: 1200px;
      margin: 0 auto;
      padding: 16px;
    }

    .view {
      display: none;
    }

    .view.active {
      display: block;
    }

    /* 卡片 */
    .card {
      background: var(--bg-primary);
      border-radius: 12px;
      padding: 16px;
      margin-bottom: 16px;
      box-shadow: var(--shadow-sm);
    }

    .card-title {
      font-size: 16px;
      font-weight: 600;
      margin-bottom: 12px;
      color: var(--text-primary);
    }

    /* 按钮 */
    .btn {
      padding: 10px 16px;
      border: none;
      border-radius: 8px;
      font-size: 14px;
      cursor: pointer;
      transition: all 0.2s;
      font-weight: 500;
    }

    .btn:active {
      transform: scale(0.95);
    }

    .btn-primary {
      background: var(--primary);
      color: white;
    }

    .btn-success {
      background: var(--success);
      color: white;
    }

    .btn-danger {
      background: var(--danger);
      color: white;
    }

    .btn-warning {
      background: var(--warning);
      color: white;
    }

    .btn-secondary {
      background: var(--border);
      color: var(--text-primary);
    }

    /* 舵机控制组 */
    .servo-group {
      margin-bottom: 20px;
    }

    .servo-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin-bottom: 12px;
    }

    .servo-label {
      font-size: 15px;
      font-weight: 600;
      color: var(--text-primary);
    }

    .servo-angle {
      font-size: 18px;
      font-weight: 700;
      color: var(--primary);
      font-variant-numeric: tabular-nums;
    }

    /* 小力舵机控制 */
    .small-servo-controls {
      display: grid;
      gap: 8px;
    }

    .small-servo-row {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 8px;
    }

    .small-servo-config {
      display: grid;
      grid-template-columns: auto 1fr auto;
      gap: 8px;
      align-items: center;
      margin-top: 8px;
      padding: 8px;
      background: var(--bg-secondary);
      border-radius: 8px;
    }

    .small-servo-config label {
      font-size: 13px;
      color: var(--text-secondary);
      white-space: nowrap;
    }

    .small-servo-config input {
      padding: 6px 10px;
      border: 1px solid var(--border);
      border-radius: 6px;
      font-size: 14px;
      text-align: center;
    }

    .small-servo-config button {
      padding: 6px 12px;
      font-size: 13px;
    }

    /* 大力舵机控制 */
    .big-servo-controls {
      display: grid;
      gap: 8px;
    }

    .big-servo-increment {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 8px;
    }

    .big-servo-direct {
      display: grid;
      grid-template-columns: 1fr auto;
      gap: 8px;
      margin-top: 8px;
    }

    .big-servo-direct input {
      padding: 10px;
      border: 1px solid var(--border);
      border-radius: 8px;
      font-size: 16px;
      text-align: center;
      font-weight: 600;
    }

    /* 步进电机控制 */
    .stepper-controls {
      display: grid;
      gap: 12px;
    }

    .stepper-row {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 8px;
    }

    .stepper-input-row {
      display: grid;
      grid-template-columns: 1fr auto;
      gap: 8px;
      align-items: center;
    }

    .stepper-input-row input {
      padding: 10px;
      border: 1px solid var(--border);
      border-radius: 8px;
      font-size: 14px;
      text-align: center;
    }

    /* 动作脚本 */
    .action-grid {
      display: grid;
      gap: 12px;
      grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
    }

    .action-card {
      background: var(--bg-secondary);
      border-radius: 12px;
      padding: 16px;
      text-align: center;
      cursor: pointer;
      transition: all 0.2s;
      border: 2px solid transparent;
    }

    .action-card:hover {
      border-color: var(--primary);
      box-shadow: var(--shadow-md);
    }

    .action-card:active {
      transform: scale(0.95);
    }

    .action-name {
      font-size: 15px;
      font-weight: 600;
      margin-bottom: 4px;
      color: var(--text-primary);
    }

    .action-desc {
      font-size: 12px;
      color: var(--text-secondary);
    }

    /* 脚本管理 */
    .script-list {
      display: grid;
      gap: 12px;
    }

    .script-card {
      background: var(--bg-secondary);
      border-radius: 12px;
      padding: 16px;
      border: 1px solid var(--border);
      box-shadow: var(--shadow-sm);
      transition: all 0.2s;
    }

    .script-card:hover {
      box-shadow: var(--shadow-md);
    }

    .script-name {
      font-size: 16px;
      font-weight: 600;
      color: var(--text-primary);
      margin-bottom: 8px;
    }

    .script-info {
      font-size: 12px;
      color: var(--text-secondary);
      margin-bottom: 12px;
      word-break: break-all;
    }

    .script-actions {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 8px;
    }

    /* 保存按钮 */
    .save-button-container {
      position: sticky;
      bottom: 16px;
      margin-top: 20px;
    }

    .btn-save-script {
      width: 100%;
      padding: 14px;
      font-size: 16px;
    }

    /* 对话框 */
    .dialog-overlay {
      position: fixed;
      top: 0;
      left: 0;
      right: 0;
      bottom: 0;
      background: rgba(0, 0, 0, 0.5);
      display: none;
      align-items: center;
      justify-content: center;
      backdrop-filter: blur(4px);
      z-index: 1000;
    }

    .dialog-overlay.active {
      display: flex;
    }

    .dialog {
      background: var(--bg-primary);
      border-radius: 16px;
      padding: 24px;
      max-width: 400px;
      width: 90%;
      box-shadow: 0 8px 32px rgba(0,0,0,0.3);
    }

    .dialog-title {
      font-size: 18px;
      font-weight: 600;
      margin-bottom: 16px;
      color: var(--text-primary);
    }

    .dialog-input {
      width: 100%;
      padding: 12px;
      border: 1px solid var(--border);
      border-radius: 8px;
      font-size: 15px;
      margin-bottom: 16px;
    }

    .dialog-actions {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 12px;
    }

    /* 日志控制台 */
    .log-console {
      background: #1e1e1e;
      color: #d4d4d4;
      padding: 16px;
      border-radius: 12px;
      font-family: 'Courier New', monospace;
      font-size: 12px;
      height: 300px;
      overflow-y: auto;
      white-space: pre-wrap;
      word-break: break-all;
      margin-top: 20px;
    }

    /* 移动端优化 */
    @media (max-width: 768px) {
      .header h1 {
        font-size: 18px;
      }

      .ws-status {
        font-size: 12px;
        padding: 4px 8px;
      }

      .nav-tab {
        font-size: 13px;
        padding: 12px 8px;
        min-width: 80px;
      }

      .container {
        padding: 12px;
      }

      .card {
        padding: 14px;
        margin-bottom: 12px;
      }
    }
  </style>
</head>
<body>
  <!-- 顶栏 -->
  <div class="header">
    <h1>ESP32-S3 展示控制</h1>
    <div class="ws-status">
      <div class="ws-indicator" id="wsIndicator"></div>
      <span id="wsStatus">连接中...</span>
    </div>
  </div>

  <!-- 导航栏 -->
  <div class="nav">
    <button class="nav-tab active" onclick="switchView('servos')">舵机控制</button>
    <button class="nav-tab" onclick="switchView('stepper')">步进电机</button>
    <button class="nav-tab" onclick="switchView('actions')">动作脚本</button>
    <button class="nav-tab" onclick="switchView('scripts')">脚本管理</button>
  </div>

  <!-- 主容器 -->
  <div class="container">
    <!-- 视图1: 舵机控制 -->
    <div id="view-servos" class="view active">
      <!-- 小力舵机组A (通道0-3) -->
      <div class="card">
        <div class="card-title">小力舵机 0-3 (松紧控制)</div>
        <div id="smallServosA"></div>
      </div>

      <!-- 大力舵机 (通道4-7) -->
      <div class="card">
        <div class="card-title">大力舵机 4-7 (角度控制)</div>
        <div id="bigServos"></div>
      </div>

      <!-- 小力舵机组B (通道8-11) -->
      <div class="card">
        <div class="card-title">小力舵机 8-11 (松紧控制)</div>
        <div id="smallServosB"></div>
      </div>

      <!-- 保存当前姿态 -->
      <div class="save-button-container">
        <button class="btn btn-primary btn-save-script" onclick="showSaveDialog()">保存当前姿态</button>
      </div>
    </div>

    <!-- 视图2: 步进电机控制 -->
    <div id="view-stepper" class="view">
      <div class="card">
        <div class="card-title">垂直丝杆 (4个并联)</div>
        <div class="stepper-controls">
          <div class="stepper-row">
            <button class="btn btn-primary" onclick="moveStepper('vertical', 1000)">上升 1000步</button>
            <button class="btn btn-primary" onclick="moveStepper('vertical', -1000)">下降 1000步</button>
          </div>
          <div class="stepper-input-row">
            <input type="number" id="verticalSteps" placeholder="输入步数..." value="1000">
            <button class="btn btn-success" onclick="moveStepperCustom('vertical')">执行</button>
          </div>
          <button class="btn btn-warning" onclick="homeMotor(0)">垂直归零</button>
        </div>
      </div>

      <div class="card">
        <div class="card-title">水平丝杆</div>
        <div class="stepper-controls">
          <div class="stepper-row">
            <button class="btn btn-primary" onclick="moveStepper('horizontal', 1000)">左移 1000步</button>
            <button class="btn btn-primary" onclick="moveStepper('horizontal', -1000)">右移 1000步</button>
          </div>
          <div class="stepper-input-row">
            <input type="number" id="horizontalSteps" placeholder="输入步数..." value="1000">
            <button class="btn btn-success" onclick="moveStepperCustom('horizontal')">执行</button>
          </div>
          <button class="btn btn-warning" onclick="homeMotor(1)">水平归零</button>
        </div>
      </div>

      <div class="card">
        <button class="btn btn-danger" style="width:100%" onclick="stopAllSteppers()">停止所有电机</button>
      </div>
    </div>

    <!-- 视图3: 动作脚本 -->
    <div id="view-actions" class="view">
      <div class="card">
        <div class="card-title">预设动作</div>
        <div class="action-grid">
          <div class="action-card" onclick="executeAction(1)">
            <div class="action-name">动作 1</div>
            <div class="action-desc">垂直上下运动</div>
          </div>
          <div class="action-card" onclick="executeAction(2)">
            <div class="action-name">动作 2</div>
            <div class="action-desc">大力6摆动</div>
          </div>
          <div class="action-card" onclick="executeAction(3)">
            <div class="action-name">动作 3</div>
            <div class="action-desc">大力4+5联动</div>
          </div>
          <div class="action-card" onclick="executeAction(4)">
            <div class="action-name">动作 4</div>
            <div class="action-desc">小力摆动</div>
          </div>
          <div class="action-card" onclick="executeAction(5)">
            <div class="action-name">动作 5</div>
            <div class="action-desc">龙抓手-抓</div>
          </div>
          <div class="action-card" onclick="executeAction(6)">
            <div class="action-name">动作 6</div>
            <div class="action-desc">龙抓手-张</div>
          </div>
        </div>
      </div>
    </div>

    <!-- 视图4: 脚本管理 -->
    <div id="view-scripts" class="view">
      <div id="scriptList" class="script-list">
        <!-- 动态生成 -->
      </div>
    </div>

    <!-- 日志控制台 -->
    <div class="log-console" id="log"></div>
  </div>

  <!-- 保存对话框 -->
  <div class="dialog-overlay" id="saveDialog">
    <div class="dialog">
      <h3 class="dialog-title">保存姿态</h3>
      <input type="text" class="dialog-input" id="scriptName" placeholder="输入姿态名称...">
      <div class="dialog-actions">
        <button class="btn btn-secondary" onclick="closeSaveDialog()">取消</button>
        <button class="btn btn-primary" onclick="saveScript()">保存</button>
      </div>
    </div>
  </div>

  <script>
    // ========== 全局变量 ==========
    let ws;
    let angles = new Array(12).fill(90);  // 12个舵机当前角度
    let leftValues = new Array(8).fill(0);   // 8个小力舵机左值
    let rightValues = new Array(8).fill(180); // 8个小力舵机右值
    let scripts = [];

    // ========== WebSocket ==========
    function initWebSocket() {
      ws = new WebSocket('ws://' + window.location.hostname + ':81/');

      ws.onopen = function() {
        document.getElementById('wsStatus').textContent = '已连接';
        document.getElementById('wsIndicator').classList.add('connected');
        addLog('[系统] WebSocket已连接');
      };

      ws.onclose = function() {
        document.getElementById('wsStatus').textContent = '已断开';
        document.getElementById('wsIndicator').classList.remove('connected');
        addLog('[系统] WebSocket已断开，5秒后重连...');
        setTimeout(initWebSocket, 5000);
      };

      ws.onmessage = function(event) {
        addLog(event.data);
      };

      ws.onerror = function(error) {
        addLog('[错误] WebSocket错误');
      };
    }

    // ========== 视图切换 ==========
    function switchView(viewName) {
      // 更新导航栏
      document.querySelectorAll('.nav-tab').forEach(tab => {
        tab.classList.remove('active');
      });
      event.target.classList.add('active');

      // 更新视图
      document.querySelectorAll('.view').forEach(view => {
        view.classList.remove('active');
      });
      document.getElementById('view-' + viewName).classList.add('active');

      // 如果切换到脚本管理，渲染脚本列表
      if (viewName === 'scripts') {
        renderScripts();
      }
    }

    // ========== 舵机控制 ==========
    function createSmallServoControl(channel) {
      const idx = channel <= 3 ? channel : (channel - 4);

      return `
        <div class="servo-group">
          <div class="servo-header">
            <span class="servo-label">小力舵机 ${channel}</span>
            <span class="servo-angle" id="angle-${channel}">${angles[channel]}°</span>
          </div>
          <div class="small-servo-controls">
            <div class="small-servo-row">
              <button class="btn btn-success" onclick="moveSmallServo(${channel}, 'left')">松开</button>
              <button class="btn btn-danger" onclick="moveSmallServo(${channel}, 'right')">抓紧</button>
            </div>
            <button class="btn btn-secondary" onclick="moveSmallServo(${channel}, 'center')">居中 (90°)</button>
            <div class="small-servo-config">
              <label>左值(松):</label>
              <input type="number" id="left-${channel}" value="${leftValues[idx]}" min="0" max="90">
              <button class="btn btn-primary" onclick="setSmallServoValue(${channel}, 'left')">设定</button>
            </div>
            <div class="small-servo-config">
              <label>右值(紧):</label>
              <input type="number" id="right-${channel}" value="${rightValues[idx]}" min="90" max="180">
              <button class="btn btn-primary" onclick="setSmallServoValue(${channel}, 'right')">设定</button>
            </div>
          </div>
        </div>
      `;
    }

    function createBigServoControl(channel) {
      return `
        <div class="servo-group">
          <div class="servo-header">
            <span class="servo-label">大力舵机 ${channel}</span>
            <span class="servo-angle" id="angle-${channel}">${angles[channel]}°</span>
          </div>
          <div class="big-servo-controls">
            <div class="big-servo-increment">
              <button class="btn btn-primary" onclick="moveBigServo(${channel}, -1)">- 5°</button>
              <button class="btn btn-primary" onclick="moveBigServo(${channel}, 1)">+ 5°</button>
            </div>
            <div class="big-servo-direct">
              <input type="number" id="direct-${channel}" value="${angles[channel]}" min="0" max="180" placeholder="输入角度...">
              <button class="btn btn-success" onclick="setBigServoAngle(${channel})">设定</button>
            </div>
          </div>
        </div>
      `;
    }

    function renderServoControls() {
      // 小力舵机组A (0-3)
      let htmlA = '';
      for (let i = 0; i <= 3; i++) {
        htmlA += createSmallServoControl(i);
      }
      document.getElementById('smallServosA').innerHTML = htmlA;

      // 大力舵机 (4-7)
      let htmlBig = '';
      for (let i = 4; i <= 7; i++) {
        htmlBig += createBigServoControl(i);
      }
      document.getElementById('bigServos').innerHTML = htmlBig;

      // 小力舵机组B (8-11)
      let htmlB = '';
      for (let i = 8; i <= 11; i++) {
        htmlB += createSmallServoControl(i);
      }
      document.getElementById('smallServosB').innerHTML = htmlB;
    }

    // 小力舵机移动
    async function moveSmallServo(channel, direction) {
      const endpoint = direction === 'center' ? '/smallservo/center' :
                       direction === 'left' ? '/smallservo/moveleft' : '/smallservo/moveright';

      try {
        const response = await fetch(endpoint + '?servo=' + channel);
        const data = await response.json();
        if (data.angle !== undefined) {
          angles[channel] = data.angle;
          updateAngleDisplay(channel);
        }
      } catch (error) {
        console.error('小力舵机' + channel + '移动失败:', error);
      }
    }

    // 设置小力舵机左右值
    async function setSmallServoValue(channel, side) {
      const value = parseInt(document.getElementById(side + '-' + channel).value);
      const endpoint = side === 'left' ? '/smallservo/setleft' : '/smallservo/setright';

      try {
        const response = await fetch(endpoint + '?servo=' + channel + '&value=' + value);
        const data = await response.json();
        if (data.status === 'ok') {
          const idx = channel <= 3 ? channel : (channel - 4);
          if (side === 'left') {
            leftValues[idx] = value;
          } else {
            rightValues[idx] = value;
          }
          addLog('[设置] 小力舵机' + channel + ' ' + (side === 'left' ? '左值' : '右值') + ': ' + value + '°');
        }
      } catch (error) {
        console.error('设置失败:', error);
      }
    }

    // 大力舵机增量移动
    async function moveBigServo(channel, direction) {
      try {
        const response = await fetch('/bigservo/move?servo=' + channel + '&dir=' + direction);
        const data = await response.json();
        if (data.angle !== undefined) {
          angles[channel] = data.angle;
          updateAngleDisplay(channel);
          document.getElementById('direct-' + channel).value = data.angle;
        }
      } catch (error) {
        console.error('大力舵机' + channel + '移动失败:', error);
      }
    }

    // 大力舵机直接设定角度
    async function setBigServoAngle(channel) {
      const angle = parseInt(document.getElementById('direct-' + channel).value);

      try {
        const response = await fetch('/bigservo/set?servo=' + channel + '&angle=' + angle);
        const data = await response.json();
        if (data.angle !== undefined) {
          angles[channel] = data.angle;
          updateAngleDisplay(channel);
        }
      } catch (error) {
        console.error('大力舵机' + channel + '设定失败:', error);
      }
    }

    // 更新角度显示
    function updateAngleDisplay(channel) {
      const elem = document.getElementById('angle-' + channel);
      if (elem) {
        elem.textContent = angles[channel] + '°';
      }
    }

    // ========== 步进电机控制 ==========
    async function moveStepper(motor, steps) {
      const endpoint = motor === 'vertical' ? '/stepper/vertical' : '/stepper/horizontal';

      try {
        await fetch(endpoint + '?steps=' + steps);
        addLog('[步进] ' + (motor === 'vertical' ? '垂直' : '水平') + '移动 ' + steps + ' 步');
      } catch (error) {
        console.error('步进电机移动失败:', error);
      }
    }

    async function moveStepperCustom(motor) {
      const inputId = motor + 'Steps';
      const steps = parseInt(document.getElementById(inputId).value) || 0;

      if (steps !== 0) {
        await moveStepper(motor, steps);
      }
    }

    async function homeMotor(motor) {
      try {
        await fetch('/stepper/home?motor=' + motor);
        addLog('[归零] ' + (motor === 0 ? '垂直' : '水平') + '电机归零');
      } catch (error) {
        console.error('归零失败:', error);
      }
    }

    async function stopAllSteppers() {
      try {
        await fetch('/stepper/stop');
        addLog('[停止] 所有步进电机已停止');
      } catch (error) {
        console.error('停止失败:', error);
      }
    }

    // ========== 动作脚本 ==========
    async function executeAction(actionNum) {
      try {
        await fetch('/action/action' + actionNum);
        addLog('[动作] 执行动作 ' + actionNum);
      } catch (error) {
        console.error('动作执行失败:', error);
      }
    }

    // ========== 脚本管理 ==========
    function loadScripts() {
      const saved = localStorage.getItem('servoScripts12');
      if (saved) {
        scripts = JSON.parse(saved);
      }
    }

    function saveScriptsToStorage() {
      localStorage.setItem('servoScripts12', JSON.stringify(scripts));
    }

    function showSaveDialog() {
      document.getElementById('saveDialog').classList.add('active');
      document.getElementById('scriptName').value = '';
      document.getElementById('scriptName').focus();
    }

    function closeSaveDialog() {
      document.getElementById('saveDialog').classList.remove('active');
    }

    function saveScript() {
      const name = document.getElementById('scriptName').value.trim();
      if (!name) {
        alert('请输入姿态名称');
        return;
      }

      const script = {
        id: Date.now(),
        name: name,
        angles: [...angles],
        leftValues: [...leftValues],
        rightValues: [...rightValues],
        timestamp: new Date().toLocaleString()
      };

      scripts.push(script);
      saveScriptsToStorage();
      closeSaveDialog();
      addLog('[保存] 姿态已保存: ' + name);

      if (document.getElementById('view-scripts').classList.contains('active')) {
        renderScripts();
      }
    }

    function deleteScript(id) {
      if (confirm('确定删除这个姿态吗?')) {
        scripts = scripts.filter(s => s.id !== id);
        saveScriptsToStorage();
        renderScripts();
        addLog('[删除] 姿态已删除');
      }
    }

    async function executeScript(id) {
      const script = scripts.find(s => s.id === id);
      if (!script) return;

      addLog('[执行] 姿态: ' + script.name);

      // 恢复左右值配置
      if (script.leftValues && script.rightValues) {
        leftValues = [...script.leftValues];
        rightValues = [...script.rightValues];
      }

      // 执行所有舵机角度
      for (let i = 0; i < 12; i++) {
        try {
          let response;

          if (i <= 3 || (i >= 8 && i <= 11)) {
            // 小力舵机 - 直接设置角度
            response = await fetch('/smallservo/moveleft?servo=' + i);
          } else {
            // 大力舵机
            response = await fetch('/bigservo/set?servo=' + i + '&angle=' + script.angles[i]);
          }

          const data = await response.json();
          if (data.angle !== undefined) {
            angles[data.servo] = data.angle;
            updateAngleDisplay(data.servo);
          }
        } catch (error) {
          console.error('舵机' + i + '执行失败:', error);
        }
      }

      // 重新渲染控制界面以更新左右值
      renderServoControls();
    }

    function renderScripts() {
      const container = document.getElementById('scriptList');

      if (scripts.length === 0) {
        container.innerHTML = '<div class="card"><p style="text-align:center;color:var(--text-tertiary);padding:40px 20px;">暂无保存的姿态<br><small>在"舵机控制"中保存当前姿态</small></p></div>';
        return;
      }

      container.innerHTML = scripts.map(script => {
        return `
          <div class="script-card">
            <div class="script-name">${script.name} <small style="color:var(--text-tertiary)">[12通道]</small></div>
            <div class="script-info">${script.timestamp}</div>
            <div class="script-actions">
              <button class="btn btn-success" onclick="executeScript(${script.id})">执行</button>
              <button class="btn btn-danger" onclick="deleteScript(${script.id})">删除</button>
            </div>
          </div>
        `;
      }).join('');
    }

    // ========== 日志 ==========
    function addLog(message) {
      const log = document.getElementById('log');
      const timestamp = new Date().toLocaleTimeString();
      log.innerHTML += '[' + timestamp + '] ' + message + '\n';
      log.scrollTop = log.scrollHeight;
    }

    // ========== 初始化 ==========
    window.onload = function() {
      initWebSocket();
      loadScripts();
      renderServoControls();

      // 获取初始状态
      fetch('/status')
        .then(response => response.json())
        .then(data => {
          // 更新角度
          for (let i = 0; i < 12; i++) {
            angles[i] = data.currentAngles[i];
            updateAngleDisplay(i);

            // 更新大力舵机输入框
            if (i >= 4 && i <= 7) {
              const elem = document.getElementById('direct-' + i);
              if (elem) elem.value = data.currentAngles[i];
            }
          }

          // 更新左右值
          leftValues = [...data.leftValues];
          rightValues = [...data.rightValues];

          // 重新渲染以显示正确的左右值
          renderServoControls();
        })
        .catch(error => {
          console.error('获取初始状态失败:', error);
        });
    };
  </script>
</body>
</html>
)rawliteral";

#endif
