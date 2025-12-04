#ifndef WEB_INTERFACE_H
#define WEB_INTERFACE_H

// HTML网页界面 - 专业三栏布局
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32S3 舵机控制</title>
  <style>
    /* ========== 重置和基础 ========== */
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
    }

    :root {
      --bg-primary: #f8fafc;
      --bg-secondary: #ffffff;
      --bg-tertiary: #f1f5f9;
      --text-primary: #1e293b;
      --text-secondary: #64748b;
      --text-tertiary: #94a3b8;
      --brand: #2563eb;
      --brand-hover: #1d4ed8;
      --danger: #ef4444;
      --success: #10b981;
      --warning: #f59e0b;
      --border: #e2e8f0;
      --shadow-sm: 0 1px 2px 0 rgb(0 0 0 / 0.05);
      --shadow-md: 0 4px 6px -1px rgb(0 0 0 / 0.1);
      --shadow-lg: 0 10px 15px -3px rgb(0 0 0 / 0.1);
    }

    body {
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", "Roboto", sans-serif;
      background: var(--bg-primary);
      color: var(--text-primary);
      line-height: 1.5;
    }

    /* ========== 移动端布局（默认） ========== */
    .app-container {
      min-height: 100vh;
      display: flex;
      flex-direction: column;
    }

    /* 顶部导航栏 */
    .navbar {
      position: sticky;
      top: 0;
      height: 60px;
      background: var(--bg-secondary);
      border-bottom: 1px solid var(--border);
      display: flex;
      align-items: center;
      padding: 0 20px;
      z-index: 1000;
      box-shadow: var(--shadow-sm);
    }

    .menu-icon {
      font-size: 24px;
      cursor: pointer;
      padding: 8px;
      margin-right: 16px;
      color: var(--text-primary);
      border-radius: 8px;
      transition: background 0.2s;
    }

    .menu-icon:hover {
      background: var(--bg-tertiary);
    }

    .navbar-title {
      font-size: 18px;
      font-weight: 600;
      color: var(--text-primary);
    }

    /* 侧边栏（移动端抽屉） */
    .sidebar-overlay {
      position: fixed;
      top: 0;
      left: 0;
      right: 0;
      bottom: 0;
      background: rgba(0, 0, 0, 0.4);
      z-index: 1100;
      display: none;
      opacity: 0;
      transition: opacity 0.3s ease;
    }

    .sidebar-overlay.active {
      display: block;
      opacity: 1;
    }

    .sidebar {
      position: fixed;
      top: 0;
      left: -280px;
      width: 280px;
      height: 100%;
      background: var(--bg-secondary);
      z-index: 1200;
      transition: left 0.3s ease;
      box-shadow: var(--shadow-lg);
      display: flex;
      flex-direction: column;
    }

    .sidebar.active {
      left: 0;
    }

    .sidebar-header {
      padding: 24px 20px;
      border-bottom: 1px solid var(--border);
    }

    .sidebar-title {
      font-size: 16px;
      font-weight: 600;
      color: var(--text-primary);
    }

    .sidebar-menu {
      flex: 1;
      padding: 12px 0;
      overflow-y: auto;
    }

    .menu-item {
      display: flex;
      align-items: center;
      padding: 12px 20px;
      color: var(--text-secondary);
      text-decoration: none;
      font-size: 15px;
      transition: all 0.2s;
      cursor: pointer;
    }

    .menu-item:hover {
      background: var(--bg-tertiary);
      color: var(--text-primary);
    }

    .menu-item.active {
      background: #eff6ff;
      color: var(--brand);
      font-weight: 500;
      border-left: 3px solid var(--brand);
    }

    .menu-item-icon {
      margin-right: 12px;
      width: 18px;
      height: 18px;
      flex-shrink: 0;
    }

    .menu-item-icon svg {
      width: 100%;
      height: 100%;
      fill: currentColor;
    }

    /* 主内容区 */
    .main-content {
      flex: 1;
      padding: 20px;
    }

    .view {
      display: none;
    }

    .view.active {
      display: block;
    }

    /* 状态指示器 */
    .status-badge {
      display: inline-flex;
      align-items: center;
      padding: 8px 16px;
      border-radius: 20px;
      font-size: 14px;
      font-weight: 500;
      margin-bottom: 20px;
    }

    .status-badge.connected {
      background: #d1fae5;
      color: #065f46;
    }

    .status-badge.disconnected {
      background: #fee2e2;
      color: #991b1b;
    }

    .status-badge::before {
      content: '';
      width: 8px;
      height: 8px;
      border-radius: 50%;
      margin-right: 8px;
      background: currentColor;
    }

    /* 卡片 */
    .card {
      background: var(--bg-secondary);
      border-radius: 12px;
      padding: 20px;
      margin-bottom: 16px;
      box-shadow: var(--shadow-md);
      border: 1px solid var(--border);
    }

    .card-title {
      font-size: 14px;
      font-weight: 600;
      color: var(--text-secondary);
      margin-bottom: 16px;
      text-transform: uppercase;
      letter-spacing: 0.05em;
    }

    /* 舵机卡片 */
    .servo-card {
      background: var(--bg-secondary);
      border-radius: 12px;
      padding: 20px;
      margin-bottom: 16px;
      box-shadow: var(--shadow-md);
      border: 1px solid var(--border);
    }

    .servo-header {
      font-size: 15px;
      font-weight: 600;
      color: var(--text-primary);
      margin-bottom: 16px;
    }

    .servo-controls {
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 20px;
    }

    .angle-display {
      font-size: 32px;
      font-weight: 700;
      color: var(--brand);
      min-width: 100px;
      text-align: center;
      font-variant-numeric: tabular-nums;
    }

    /* 按钮 */
    .btn {
      display: inline-flex;
      align-items: center;
      justify-content: center;
      padding: 10px 20px;
      border: none;
      border-radius: 8px;
      font-size: 15px;
      font-weight: 500;
      cursor: pointer;
      transition: all 0.2s;
      text-decoration: none;
      min-height: 44px;
    }

    .btn:active {
      transform: scale(0.98);
    }

    .btn-primary {
      background: var(--brand);
      color: white;
    }

    .btn-primary:hover {
      background: var(--brand-hover);
    }

    .btn-danger {
      background: var(--danger);
      color: white;
    }

    .btn-danger:hover {
      background: #dc2626;
    }

    .btn-success {
      background: var(--success);
      color: white;
    }

    .btn-success:hover {
      background: #059669;
    }

    .btn-secondary {
      background: var(--bg-tertiary);
      color: var(--text-primary);
    }

    .btn-secondary:hover {
      background: #cbd5e1;
    }

    .btn-circle {
      width: 64px;
      height: 64px;
      border-radius: 50%;
      padding: 0;
      font-size: 28px;
      font-weight: 300;
      box-shadow: var(--shadow-md);
    }

    /* 步进选择器 */
    .step-selector {
      background: var(--bg-secondary);
      border-radius: 12px;
      padding: 20px;
      margin-bottom: 20px;
      box-shadow: var(--shadow-md);
      border: 1px solid var(--border);
    }

    .step-label {
      font-size: 13px;
      font-weight: 600;
      color: var(--text-secondary);
      margin-bottom: 12px;
      text-transform: uppercase;
      letter-spacing: 0.05em;
    }

    .step-buttons {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      gap: 8px;
    }

    .step-btn {
      padding: 12px;
      border: 2px solid var(--border);
      background: var(--bg-secondary);
      border-radius: 8px;
      font-size: 15px;
      font-weight: 600;
      cursor: pointer;
      transition: all 0.2s;
      color: var(--text-secondary);
    }

    .step-btn:hover {
      border-color: var(--brand);
      color: var(--brand);
    }

    .step-btn.active {
      border-color: var(--brand);
      background: var(--brand);
      color: white;
    }

    /* 批量输入网格 */
    .batch-grid {
      display: grid;
      grid-template-columns: repeat(2, 1fr);
      gap: 16px;
      margin-bottom: 20px;
    }

    .input-group {
      display: flex;
      flex-direction: column;
    }

    .input-label {
      font-size: 13px;
      font-weight: 600;
      color: var(--text-secondary);
      margin-bottom: 8px;
      text-transform: uppercase;
      letter-spacing: 0.05em;
    }

    .input-field {
      width: 100%;
      padding: 12px 16px;
      border: 2px solid var(--border);
      border-radius: 8px;
      font-size: 16px;
      text-align: center;
      font-weight: 600;
      color: var(--text-primary);
      transition: all 0.2s;
    }

    .input-field:focus {
      outline: none;
      border-color: var(--brand);
      box-shadow: 0 0 0 3px rgba(37, 99, 235, 0.1);
    }

    /* 动作按钮组 */
    .action-buttons {
      display: grid;
      grid-template-columns: repeat(2, 1fr);
      gap: 12px;
      margin-bottom: 16px;
    }

    /* 脚本列表 */
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
      font-size: 13px;
      color: var(--text-secondary);
      margin-bottom: 12px;
      font-family: 'SF Mono', Monaco, monospace;
    }

    .script-actions {
      display: flex;
      gap: 8px;
    }

    /* 日志面板 */
    .log-panel {
      background: var(--bg-secondary);
      border-radius: 12px;
      padding: 16px;
      margin-top: 20px;
      box-shadow: var(--shadow-md);
      border: 1px solid var(--border);
      max-height: 400px;
      display: flex;
      flex-direction: column;
    }

    .log-header {
      font-size: 13px;
      font-weight: 600;
      color: var(--text-secondary);
      margin-bottom: 12px;
      text-transform: uppercase;
      letter-spacing: 0.05em;
      flex-shrink: 0;
    }

    .log-console {
      background: #0f172a;
      color: #10b981;
      padding: 16px;
      flex: 1;
      min-height: 0;
      overflow-y: auto;
      font-family: 'SF Mono', Monaco, 'Cascadia Code', monospace;
      font-size: 13px;
      border-radius: 8px;
      line-height: 1.6;
    }

    /* 对话框 */
    .dialog-overlay {
      position: fixed;
      top: 0;
      left: 0;
      right: 0;
      bottom: 0;
      background: rgba(15, 23, 42, 0.6);
      z-index: 2000;
      display: none;
      align-items: center;
      justify-content: center;
      backdrop-filter: blur(4px);
    }

    .dialog-overlay.active {
      display: flex;
    }

    .dialog {
      background: var(--bg-secondary);
      border-radius: 12px;
      padding: 24px;
      max-width: 90%;
      width: 400px;
      box-shadow: var(--shadow-lg);
    }

    .dialog-title {
      font-size: 18px;
      font-weight: 600;
      margin-bottom: 16px;
      color: var(--text-primary);
    }

    .dialog-input {
      width: 100%;
      padding: 12px 16px;
      border: 2px solid var(--border);
      border-radius: 8px;
      font-size: 15px;
      margin-bottom: 20px;
    }

    .dialog-input:focus {
      outline: none;
      border-color: var(--brand);
      box-shadow: 0 0 0 3px rgba(37, 99, 235, 0.1);
    }

    .dialog-actions {
      display: flex;
      gap: 12px;
      justify-content: flex-end;
    }

    /* 高级控制 - 16通道紧凑网格 */
    .advanced-servo-grid {
      display: grid;
      grid-template-columns: repeat(2, 1fr);
      gap: 12px;
    }

    .advanced-servo-item {
      background: var(--bg-tertiary);
      border-radius: 8px;
      padding: 12px;
      display: flex;
      align-items: center;
      justify-content: space-between;
      border: 1px solid var(--border);
    }

    .advanced-servo-info {
      display: flex;
      flex-direction: column;
      gap: 2px;
    }

    .advanced-servo-label {
      font-size: 12px;
      font-weight: 600;
      color: var(--text-secondary);
      text-transform: uppercase;
    }

    .advanced-servo-angle {
      font-size: 20px;
      font-weight: 700;
      color: var(--brand);
      font-variant-numeric: tabular-nums;
    }

    .advanced-servo-controls {
      display: flex;
      gap: 8px;
    }

    .btn-compact {
      width: 40px;
      height: 40px;
      padding: 0;
      font-size: 20px;
      min-height: auto;
      border-radius: 6px;
    }

    /* ========== 桌面端三栏布局 (1200px+) ========== */
    @media (min-width: 1200px) {
      .app-container {
        display: grid;
        grid-template-columns: 260px 1fr 500px;
        grid-template-rows: auto 1fr;
        height: 100vh;
      }

      .navbar {
        grid-column: 1 / -1;
        position: static;
      }

      .menu-icon {
        display: none;
      }

      .sidebar-overlay {
        display: none !important;
      }

      .sidebar {
        position: static;
        left: 0;
        width: auto;
        height: auto;
        border-right: 1px solid var(--border);
        box-shadow: none;
      }

      .main-content {
        padding: 32px;
        overflow-y: auto;
        background: var(--bg-primary);
      }

      .log-panel {
        margin: 0;
        padding: 20px;
        border-radius: 0;
        border-left: 1px solid var(--border);
        max-height: none;
        height: 100%;
        display: flex;
        flex-direction: column;
        background: var(--bg-secondary);
        overflow: hidden;
      }

      .log-console {
        flex: 1;
        min-height: 0;
        overflow-y: auto;
      }

      /* 舵机调试视图2列布局 */
      #view-tuning .servo-grid {
        display: grid;
        grid-template-columns: repeat(2, 1fr);
        gap: 16px;
      }

      #view-tuning .step-selector {
        grid-column: 1 / -1;
      }

      #view-tuning .save-card {
        grid-column: 1 / -1;
      }

      /* 批量控制4列布局 */
      #view-batch .batch-grid {
        grid-template-columns: repeat(4, 1fr);
      }

      /* 高级控制4列布局 */
      .advanced-servo-grid {
        grid-template-columns: repeat(4, 1fr);
      }
    }

    /* ========== 小屏手机优化 ========== */
    @media (max-width: 640px) {
      .batch-grid {
        grid-template-columns: 1fr;
      }

      .action-buttons {
        grid-template-columns: 1fr;
      }

      .btn-circle {
        width: 56px;
        height: 56px;
        font-size: 24px;
      }

      .angle-display {
        font-size: 28px;
        min-width: 90px;
      }
    }
  </style>
</head>
<body>
  <div class="app-container">
    <!-- 顶部导航栏 -->
    <nav class="navbar">
      <div class="menu-icon" onclick="toggleSidebar()">☰</div>
      <div class="navbar-title">ESP32-S3 舵机控制台</div>
    </nav>

    <!-- 侧边栏背景遮罩 -->
    <div class="sidebar-overlay" id="sidebarOverlay" onclick="closeSidebar()"></div>

    <!-- 侧边栏 -->
    <aside class="sidebar" id="sidebar">
      <div class="sidebar-header">
        <div class="sidebar-title">功能导航</div>
      </div>
      <nav class="sidebar-menu">
        <a class="menu-item active" onclick="switchView('tuning')">
          <span class="menu-item-icon">
            <svg viewBox="0 0 24 24"><path d="M22.7 19l-9.1-9.1c.9-2.3.4-5-1.5-6.9-2-2-5-2.4-7.4-1.3L9 6 6 9 1.6 4.7C.4 7.1.9 10.1 2.9 12.1c1.9 1.9 4.6 2.4 6.9 1.5l9.1 9.1c.4.4 1 .4 1.4 0l2.3-2.3c.5-.4.5-1.1.1-1.4z"/></svg>
          </span>
          <span>舵机调试</span>
        </a>
        <a class="menu-item" onclick="switchView('batch')">
          <span class="menu-item-icon">
            <svg viewBox="0 0 24 24"><path d="M7 2v11h3v9l7-12h-4l4-8z"/></svg>
          </span>
          <span>批量控制</span>
        </a>
        <a class="menu-item" onclick="switchView('scripts')">
          <span class="menu-item-icon">
            <svg viewBox="0 0 24 24"><path d="M8 5v14l11-7z"/></svg>
          </span>
          <span>动作脚本</span>
        </a>
        <a class="menu-item" onclick="switchView('advanced')">
          <span class="menu-item-icon">
            <svg viewBox="0 0 24 24"><path d="M12 15.5A3.5 3.5 0 0 1 8.5 12 3.5 3.5 0 0 1 12 8.5a3.5 3.5 0 0 1 3.5 3.5 3.5 3.5 0 0 1-3.5 3.5m7.43-2.53c.04-.32.07-.64.07-.97 0-.33-.03-.66-.07-1l2.11-1.63c.19-.15.24-.42.12-.64l-2-3.46c-.12-.22-.39-.31-.61-.22l-2.49 1c-.52-.39-1.06-.73-1.69-.98l-.37-2.65A.506.506 0 0 0 14 2h-4c-.25 0-.46.18-.5.42l-.37 2.65c-.63.25-1.17.59-1.69.98l-2.49-1c-.22-.09-.49 0-.61.22l-2 3.46c-.13.22-.07.49.12.64L4.57 11c-.04.34-.07.67-.07 1 0 .33.03.65.07.97l-2.11 1.66c-.19.15-.25.42-.12.64l2 3.46c.12.22.39.3.61.22l2.49-1.01c.52.4 1.06.74 1.69.99l.37 2.65c.04.24.25.42.5.42h4c.25 0 .46-.18.5-.42l.37-2.65c.63-.26 1.17-.59 1.69-.99l2.49 1.01c.22.08.49 0 .61-.22l2-3.46c.12-.22.07-.49-.12-.64z"/></svg>
          </span>
          <span>高级控制</span>
        </a>
      </nav>
    </aside>

    <!-- 主内容区 -->
    <main class="main-content">
      <!-- 连接状态 -->
      <div id="status" class="status-badge disconnected">断开连接</div>

      <!-- 视图A: 舵机调试 -->
      <div id="view-tuning" class="view active">
        <div class="step-selector">
          <div class="step-label">步进角度</div>
          <div class="step-buttons">
            <button class="step-btn" onclick="setStepSize(1)">1°</button>
            <button class="step-btn active" onclick="setStepSize(5)">5°</button>
            <button class="step-btn" onclick="setStepSize(10)">10°</button>
          </div>
        </div>

        <div class="servo-grid">
          <div class="servo-card">
            <div class="servo-header">舵机 0</div>
            <div class="servo-controls">
              <button class="btn btn-danger btn-circle" onclick="adjustServo(0, -1)">−</button>
              <div class="angle-display" id="angle0">135°</div>
              <button class="btn btn-success btn-circle" onclick="adjustServo(0, 1)">+</button>
            </div>
          </div>

          <div class="servo-card">
            <div class="servo-header">舵机 1</div>
            <div class="servo-controls">
              <button class="btn btn-danger btn-circle" onclick="adjustServo(1, -1)">−</button>
              <div class="angle-display" id="angle1">135°</div>
              <button class="btn btn-success btn-circle" onclick="adjustServo(1, 1)">+</button>
            </div>
          </div>

          <div class="servo-card">
            <div class="servo-header">舵机 2</div>
            <div class="servo-controls">
              <button class="btn btn-danger btn-circle" onclick="adjustServo(2, -1)">−</button>
              <div class="angle-display" id="angle2">135°</div>
              <button class="btn btn-success btn-circle" onclick="adjustServo(2, 1)">+</button>
            </div>
          </div>

          <div class="servo-card">
            <div class="servo-header">舵机 3</div>
            <div class="servo-controls">
              <button class="btn btn-danger btn-circle" onclick="adjustServo(3, -1)">−</button>
              <div class="angle-display" id="angle3">135°</div>
              <button class="btn btn-success btn-circle" onclick="adjustServo(3, 1)">+</button>
            </div>
          </div>

          <div class="save-card">
            <button class="btn btn-primary" style="width:100%" onclick="showSaveDialog()">保存当前姿态</button>
          </div>
        </div>
      </div>

      <!-- 视图B: 批量控制 -->
      <div id="view-batch" class="view">
        <div class="card">
          <div class="card-title">快捷操作</div>
          <div class="action-buttons">
            <button class="btn btn-secondary" onclick="resetAll()">归零 (0°)</button>
            <button class="btn btn-primary" onclick="centerAll()">居中 (135°)</button>
          </div>
        </div>

        <div class="card">
          <div class="card-title">批量设定角度</div>
          <div class="batch-grid">
            <div class="input-group">
              <label class="input-label">舵机 0</label>
              <input type="number" class="input-field" id="batch0" min="0" max="270" value="135">
            </div>
            <div class="input-group">
              <label class="input-label">舵机 1</label>
              <input type="number" class="input-field" id="batch1" min="0" max="270" value="135">
            </div>
            <div class="input-group">
              <label class="input-label">舵机 2</label>
              <input type="number" class="input-field" id="batch2" min="0" max="270" value="135">
            </div>
            <div class="input-group">
              <label class="input-label">舵机 3</label>
              <input type="number" class="input-field" id="batch3" min="0" max="270" value="135">
            </div>
          </div>
          <button class="btn btn-success" style="width:100%" onclick="batchExecute()">执行</button>
        </div>
      </div>

      <!-- 视图C: 动作脚本 -->
      <div id="view-scripts" class="view">
        <div id="scriptList" class="script-list">
          <!-- 动态生成 -->
        </div>
      </div>

      <!-- 视图D: 高级控制 (16通道) -->
      <div id="view-advanced" class="view">
        <div class="card">
          <div class="card-title">16通道快捷操作</div>
          <div class="action-buttons">
            <button class="btn btn-secondary" onclick="resetAll16()">全部归零 (0°)</button>
            <button class="btn btn-primary" onclick="centerAll16()">全部居中 (135°)</button>
          </div>
        </div>

        <div class="card">
          <div class="card-title">全部通道控制 (0-15)</div>
          <div id="advanced-grid" class="advanced-servo-grid">
            <!-- 动态生成16个舵机控制 -->
          </div>
          <button class="btn btn-primary" style="width:100%;margin-top:16px" onclick="showSaveDialog()">保存当前16通道姿态</button>
        </div>
      </div>
    </main>

    <!-- 日志面板 -->
    <aside class="log-panel">
      <div class="log-header">运行日志</div>
      <div class="log-console" id="log"></div>
    </aside>
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
    var ws;
    var angles = [135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135];
    var stepSize = 5;
    var scripts = [];

    // ========== WebSocket连接 ==========
    function initWebSocket() {
      ws = new WebSocket('ws://' + window.location.hostname + ':81');

      ws.onopen = function() {
        document.getElementById('status').textContent = '已连接';
        document.getElementById('status').className = 'status-badge connected';
        addLog('WebSocket连接成功');
      };

      ws.onclose = function() {
        document.getElementById('status').textContent = '断开连接';
        document.getElementById('status').className = 'status-badge disconnected';
        addLog('断开连接，3秒后重连...');
        setTimeout(initWebSocket, 3000);
      };

      ws.onerror = function() {
        addLog('WebSocket错误');
      };

      ws.onmessage = function(event) {
        addLog(event.data);
      };
    }

    // ========== 侧边栏控制 ==========
    function toggleSidebar() {
      document.getElementById('sidebar').classList.toggle('active');
      document.getElementById('sidebarOverlay').classList.toggle('active');
    }

    function closeSidebar() {
      document.getElementById('sidebar').classList.remove('active');
      document.getElementById('sidebarOverlay').classList.remove('active');
    }

    // ========== 视图切换 ==========
    function switchView(viewId) {
      document.querySelectorAll('.view').forEach(v => v.classList.remove('active'));
      document.getElementById('view-' + viewId).classList.add('active');

      document.querySelectorAll('.menu-item').forEach(a => a.classList.remove('active'));
      event.target.closest('.menu-item').classList.add('active');

      closeSidebar();

      if (viewId === 'scripts') {
        renderScripts();
      } else if (viewId === 'advanced') {
        renderAdvancedGrid();
      }
    }

    // ========== 步进控制 ==========
    function setStepSize(size) {
      stepSize = size;
      document.querySelectorAll('.step-btn').forEach(btn => btn.classList.remove('active'));
      event.target.classList.add('active');
    }

    // ========== 舵机调整 ==========
    async function adjustServo(servo, direction) {
      const currentAngle = angles[servo];
      let newAngle = currentAngle + (direction * stepSize);
      newAngle = Math.max(0, Math.min(270, newAngle));

      try {
        const response = await fetch('/set?servo=' + servo + '&angle=' + newAngle);
        const data = await response.json();
        if (data.angle !== undefined) {
          angles[data.servo] = data.angle;
          updateAngleDisplay(data.servo, data.angle);
        }
      } catch (error) {
        console.error('舵机' + servo + '调整失败:', error);
      }
    }

    // ========== 更新角度显示 ==========
    function updateAngleDisplay(servo, angle) {
      document.getElementById('angle' + servo).textContent = angle + '°';
      const batchInput = document.getElementById('batch' + servo);
      if (batchInput) {
        batchInput.value = angle;
      }
    }

    // ========== 批量控制 ==========
    async function resetAll() {
      for (let i = 0; i < 4; i++) {
        try {
          const response = await fetch('/set?servo=' + i + '&angle=0');
          const data = await response.json();
          if (data.angle !== undefined) {
            angles[data.servo] = data.angle;
            updateAngleDisplay(data.servo, data.angle);
          }
        } catch (error) {
          console.error('舵机' + i + '归零失败:', error);
        }
      }
    }

    async function centerAll() {
      for (let i = 0; i < 4; i++) {
        try {
          const response = await fetch('/set?servo=' + i + '&angle=135');
          const data = await response.json();
          if (data.angle !== undefined) {
            angles[data.servo] = data.angle;
            updateAngleDisplay(data.servo, data.angle);
          }
        } catch (error) {
          console.error('舵机' + i + '居中失败:', error);
        }
      }
    }

    async function batchExecute() {
      const batchAngles = [];
      for (let i = 0; i < 4; i++) {
        const angle = parseInt(document.getElementById('batch' + i).value);
        if (angle < 0 || angle > 270) {
          alert('舵机' + i + '的角度必须在0-270度之间');
          return;
        }
        batchAngles.push(angle);
      }

      for (let i = 0; i < 4; i++) {
        try {
          const response = await fetch('/set?servo=' + i + '&angle=' + batchAngles[i]);
          const data = await response.json();
          if (data.angle !== undefined) {
            angles[data.servo] = data.angle;
            updateAngleDisplay(data.servo, data.angle);
          }
        } catch (error) {
          console.error('舵机' + i + '设置失败:', error);
        }
      }
    }

    // ========== 高级控制 (16通道) ==========
    function renderAdvancedGrid() {
      const container = document.getElementById('advanced-grid');
      let html = '';

      for (let i = 0; i < 16; i++) {
        html += `
          <div class="advanced-servo-item">
            <div class="advanced-servo-info">
              <div class="advanced-servo-label">S${i}</div>
              <div class="advanced-servo-angle" id="advanced-angle${i}">${angles[i]}°</div>
            </div>
            <div class="advanced-servo-controls">
              <button class="btn btn-danger btn-compact" onclick="adjustAdvanced(${i}, -1)">−</button>
              <button class="btn btn-success btn-compact" onclick="adjustAdvanced(${i}, 1)">+</button>
            </div>
          </div>
        `;
      }

      container.innerHTML = html;
    }

    async function adjustAdvanced(servo, direction) {
      const currentAngle = angles[servo];
      let newAngle = currentAngle + (direction * stepSize);
      newAngle = Math.max(0, Math.min(270, newAngle));

      try {
        const response = await fetch('/set?servo=' + servo + '&angle=' + newAngle);
        const data = await response.json();
        if (data.angle !== undefined) {
          angles[data.servo] = data.angle;
          updateAdvancedAngle(data.servo, data.angle);
          // Also update main view if visible
          updateAngleDisplay(data.servo, data.angle);
        }
      } catch (error) {
        console.error('舵机' + servo + '调整失败:', error);
      }
    }

    function updateAdvancedAngle(servo, angle) {
      const element = document.getElementById('advanced-angle' + servo);
      if (element) {
        element.textContent = angle + '°';
      }
    }

    async function resetAll16() {
      for (let i = 0; i < 16; i++) {
        try {
          const response = await fetch('/set?servo=' + i + '&angle=0');
          const data = await response.json();
          if (data.angle !== undefined) {
            angles[data.servo] = data.angle;
            updateAdvancedAngle(data.servo, data.angle);
            updateAngleDisplay(data.servo, data.angle);
          }
        } catch (error) {
          console.error('舵机' + i + '归零失败:', error);
        }
      }
    }

    async function centerAll16() {
      for (let i = 0; i < 16; i++) {
        try {
          const response = await fetch('/set?servo=' + i + '&angle=135');
          const data = await response.json();
          if (data.angle !== undefined) {
            angles[data.servo] = data.angle;
            updateAdvancedAngle(data.servo, data.angle);
            updateAngleDisplay(data.servo, data.angle);
          }
        } catch (error) {
          console.error('舵机' + i + '居中失败:', error);
        }
      }
    }

    // ========== 动作脚本管理 ==========
    function loadScripts() {
      const saved = localStorage.getItem('servoScripts');
      if (saved) {
        scripts = JSON.parse(saved);
      }
    }

    function saveScriptsToStorage() {
      localStorage.setItem('servoScripts', JSON.stringify(scripts));
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

      // Support both old 4-channel and new 16-channel scripts
      const numChannels = script.angles.length;
      for (let i = 0; i < numChannels; i++) {
        try {
          const response = await fetch('/set?servo=' + i + '&angle=' + script.angles[i]);
          const data = await response.json();
          if (data.angle !== undefined) {
            angles[data.servo] = data.angle;
            updateAngleDisplay(data.servo, data.angle);
            updateAdvancedAngle(data.servo, data.angle);
          }
        } catch (error) {
          console.error('舵机' + i + '执行失败:', error);
        }
      }
    }

    function renderScripts() {
      const container = document.getElementById('scriptList');

      if (scripts.length === 0) {
        container.innerHTML = '<div class="card"><p style="text-align:center;color:var(--text-tertiary);padding:40px 20px;">暂无保存的姿态<br><small>在"舵机调试"或"高级控制"中保存当前姿态</small></p></div>';
        return;
      }

      container.innerHTML = scripts.map(script => {
        const channelLabel = script.angles.length === 16 ? '16通道' : '4通道';
        return `
          <div class="script-card">
            <div class="script-name">${script.name} <small style="color:var(--text-tertiary)">[${channelLabel}]</small></div>
            <div class="script-info">[${script.angles.join(', ')}]° • ${script.timestamp}</div>
            <div class="script-actions">
              <button class="btn btn-success" onclick="executeScript(${script.id})" style="flex:1">执行</button>
              <button class="btn btn-danger" onclick="deleteScript(${script.id})" style="flex:1">删除</button>
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

      fetch('/angles')
        .then(response => response.json())
        .then(data => {
          // Update all 16 channels
          for (let i = 0; i < 16; i++) {
            angles[i] = data.angles[i];
            // Update displays for first 4 servos in tuning view
            if (i < 4) {
              updateAngleDisplay(i, data.angles[i]);
            }
          }
        })
        .catch(error => {
          console.error('获取初始角度失败:', error);
        });
    };
  </script>
</body>
</html>
)rawliteral";

#endif
