"""
Enhanced HTML template for web interface with parameter controls.
"""

HTML_TEMPLATE = """
<!DOCTYPE html>
<html>
<head>
    <title>SIMILAR2Logo Simulation</title>
    <style>
        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
        }
        
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
        }
        
        .container {
            max-width: 1600px;
            margin: 0 auto;
            background: white;
            border-radius: 20px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            overflow: hidden;
        }
        
        .header {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 30px;
            text-align: center;
        }
        
        .header h1 {
            margin: 0;
            font-size: 2.5em;
            font-weight: 300;
        }
        
        .content {
            display: grid;
            grid-template-columns: 1fr 350px;
            gap: 20px;
            padding: 20px;
        }
        
        .canvas-container {
            background: #f8f9fa;
            border-radius: 10px;
            padding: 20px;
            box-shadow: inset 0 2px 10px rgba(0,0,0,0.1);
        }
        
        canvas {
            border: 2px solid #dee2e6;
            border-radius: 8px;
            background: white;
            display: block;
            width: 100%;
            height: auto;
        }
        
        .sidebar {
            display: flex;
            flex-direction: column;
            gap: 15px;
        }
        
        .panel {
            background: #f8f9fa;
            border-radius: 10px;
            padding: 20px;
        }
        
        .panel h3 {
            margin: 0 0 15px 0;
            color: #495057;
            font-size: 1.1em;
            border-bottom: 2px solid #667eea;
            padding-bottom: 10px;
        }
        
        button {
            width: 100%;
            padding: 12px;
            margin: 5px 0;
            border: none;
            border-radius: 8px;
            font-size: 1em;
            cursor: pointer;
            transition: all 0.3s;
            font-weight: 500;
        }
        
        button:hover {
            transform: translateY(-2px);
            box-shadow: 0 4px 12px rgba(0,0,0,0.15);
        }
        
        .btn-start {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
        }
        
        .btn-pause {
            background: #ffc107;
            color: #000;
        }
        
        .btn-stop {
            background: #dc3545;
            color: white;
        }
        
        .btn-reset {
            background: #6c757d;
            color: white;
        }
        
        .stats {
            background: white;
            border-radius: 8px;
            padding: 15px;
            margin-top: 10px;
        }
        
        .stat-item {
            display: flex;
            justify-content: space-between;
            padding: 8px 0;
            border-bottom: 1px solid #e9ecef;
        }
        
        .stat-item:last-child {
            border-bottom: none;
        }
        
        .stat-label {
            color: #6c757d;
            font-weight: 500;
        }
        
        .stat-value {
            color: #495057;
            font-weight: 600;
        }
        
        .control-group {
            margin: 15px 0;
        }
        
        .control-group label {
            display: block;
            margin-bottom: 8px;
            color: #495057;
            font-weight: 500;
            font-size: 0.9em;
        }
        
        input[type="range"] {
            width: 100%;
            height: 6px;
            border-radius: 3px;
            background: #dee2e6;
            outline: none;
            -webkit-appearance: none;
        }
        
        input[type="range"]::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 18px;
            height: 18px;
            border-radius: 50%;
            background: #667eea;
            cursor: pointer;
            transition: all 0.2s;
        }
        
        input[type="range"]::-webkit-slider-thumb:hover {
            background: #764ba2;
            transform: scale(1.2);
        }
        
        input[type="range"]::-moz-range-thumb {
            width: 18px;
            height: 18px;
            border-radius: 50%;
            background: #667eea;
            cursor: pointer;
            border: none;
        }
        
        input[type="number"] {
            width: 100%;
            padding: 8px 12px;
            border: 2px solid #dee2e6;
            border-radius: 6px;
            font-size: 0.95em;
            transition: border-color 0.2s;
        }
        
        input[type="number"]:focus {
            outline: none;
            border-color: #667eea;
        }
        
        .value-display {
            display: inline-block;
            float: right;
            color: #667eea;
            font-weight: 600;
        }
        
        .collapsible {
            max-height: 400px;
            overflow-y: auto;
        }
        
        .collapsible::-webkit-scrollbar {
            width: 8px;
        }
        
        .collapsible::-webkit-scrollbar-track {
            background: #f1f1f1;
            border-radius: 4px;
        }
        
        .collapsible::-webkit-scrollbar-thumb {
            background: #667eea;
            border-radius: 4px;
        }
        
        .collapsible::-webkit-scrollbar-thumb:hover {
            background: #764ba2;
        }
        .tabs {
            display: flex;
            margin-bottom: 20px;
            border-bottom: 2px solid #e9ecef;
        }
        
        .tab {
            padding: 10px 20px;
            cursor: pointer;
            font-weight: 500;
            color: #6c757d;
            border-bottom: 2px solid transparent;
            margin-bottom: -2px;
            transition: all 0.3s;
        }
        
        .tab:hover {
            color: #667eea;
        }
        
        .tab.active {
            color: #667eea;
            border-bottom-color: #667eea;
        }
        
        .editor-container {
            display: none;
            height: 800px;
            background: #1e1e1e;
            border-radius: 10px;
            padding: 20px;
            color: #d4d4d4;
            font-family: 'Consolas', 'Monaco', monospace;
        }
        
        .editor-container.active {
            display: flex;
            flex-direction: column;
        }
        
        textarea#code-editor {
            width: 100%;
            height: 100%;
            background: #1e1e1e;
            color: #d4d4d4;
            border: none;
            resize: none;
            font-family: inherit;
            font-size: 14px;
            line-height: 1.5;
            outline: none;
        }
        
        .editor-controls {
            margin-top: 10px;
            display: flex;
            gap: 10px;
            justify-content: flex-end;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🐢 SIMILAR2Logo Simulation</h1>
        </div>
        
        <div style="padding: 0 20px;">
            <div class="tabs">
                <div class="tab active" onclick="switchTab('simulation')">Simulation</div>
                <div class="tab" onclick="switchTab('editor')">Code Editor</div>
            </div>
        </div>

        <div class="content" id="simulation-view">
            <div class="canvas-container">
                <canvas id="canvas" width="800" height="800"></canvas>
            </div>
            <div class="sidebar">
                <!-- Control Panel -->
                <div class="panel">
                    <h3>Simulation Control</h3>
                    <button class="btn-start" onclick="startSim()">▶ Start</button>
                    <button class="btn-pause" onclick="pauseSim()">⏸ Pause</button>
                    <button class="btn-stop" onclick="stopSim()">⏹ Stop</button>
                    <button class="btn-reset" onclick="resetSim()">🔄 Reset</button>
                </div>
                
                <!-- Speed Control -->
                <div class="panel">
                    <h3>Speed Control</h3>
                    <div class="control-group">
                        <label>
                            Speed Factor 
                            <span class="value-display" id="speed-display">10.0x</span>
                        </label>
                        <input type="range" id="speed-slider" 
                               min="0.1" max="100" step="0.1" value="10.0"
                               oninput="updateSpeed(this.value)">
                        <div style="display: flex; justify-content: space-between; font-size: 0.8em; color: #6c757d; margin-top: 5px;">
                            <span>0.1x (Slow)</span>
                            <span>100x (Fast)</span>
                        </div>
                    </div>
                </div>
                <!-- Parameters Control -->
                <div class="panel">
                    <h3>Model Parameters</h3>
                    <div id="parameters-container" class="collapsible"></div>
                </div>
                
                <!-- Agent Count Control -->
                <div class="panel">
                    <h3>Agent Count</h3>
                    <div class="control-group">
                        <label>Number of Agents</label>
                        <div style="display: flex; gap: 10px; align-items: center; margin-bottom: 10px;">
                            <input type="number" id="agent-count-input" 
                                   min="0" max="10000" value="50" 
                                   onchange="syncAgentCount(this.value, 'input')"
                                   style="width: 100px;">
                            <input type="range" id="agent-count-slider" 
                                   min="0" max="5000" step="1" value="50"
                                   oninput="syncAgentCount(this.value, 'slider')"
                                   style="flex-grow: 1;">
                        </div>
                        <div style="display: flex; justify-content: space-between; font-size: 0.8em; color: #6c757d;">
                            <span>0</span>
                            <span>5000</span>
                        </div>
                        <button class="btn-start" style="margin-top: 10px;" onclick="applyAgentCount()">
                            Apply Agent Count
                        </button>
                    </div>
                </div>
                
                <!-- Model Selection Panel -->
                <div class="panel">
                    <h3>Model Selection</h3>
                    <div class="control-group">
                        <label for="model-select">Choose Model:</label>
                        <select id="model-select" style="width:100%; padding:8px;">
                            <option value="" disabled selected>Loading models...</option>
                        </select>
                    </div>
                    <button class="btn-start" style="margin-top:10px;" onclick="loadSelectedModel()">Load Model</button>
                </div>

                
                <!-- Statistics Panel -->
                <div class="panel">
                    <h3>Statistics</h3>
                    <div class="stats">
                        <div class="stat-item">
                            <span class="stat-label">Step:</span>
                            <span class="stat-value" id="step">0</span>
                        </div>
                        <div class="stat-item">
                            <span class="stat-label">Agents:</span>
                            <span class="stat-value" id="turtles">0</span>
                        </div>
                        <div class="stat-item">
                            <span class="stat-label">FPS:</span>
                            <span class="stat-value" id="fps">0</span>
                        </div>
                        <div class="stat-item">
                            <span class="stat-label">Speed:</span>
                            <span class="stat-value" id="current-speed">10.0x</span>
                        </div>
                    </div>
                </div>
            </div>
        </div>
        
        <div class="editor-container" id="editor-view">
            <div style="margin-bottom: 10px; display: flex; justify-content: space-between; align-items: center;">
                <span style="color: #888;" id="filename-display">Loading...</span>
                <span style="color: #ff6b6b; font-size: 0.9em;">⚠️ Restart required after saving</span>
            </div>
            <textarea id="code-editor" spellcheck="false"></textarea>
            <div class="editor-controls">
                <button class="btn-start" onclick="saveAndRestart()" style="width: auto; padding: 10px 30px;">
                    💾 Save & Restart Simulation
                </button>
            </div>
        </div>
    </div>

    <script>
        const canvas = document.getElementById('canvas');
        const ctx = canvas.getContext('2d');
        let ws = new WebSocket('ws://' + window.location.host + '/ws');
        
        let lastFrameTime = Date.now();
        let frameCount = 0;
        let fps = 0;
        let parameters = {};
        
        function connectWebSocket() {
            ws = new WebSocket('ws://' + window.location.host + '/ws');
            ws.onmessage = function(event) {
                try {
                    const state = JSON.parse(event.data);
                    console.log('Received state with', state.marks ? state.marks.length : 0, 'marks');
                    render(state);
                    updateStats(state);
                } catch (e) {
                    console.error('Error processing WebSocket message:', e);
                }
            };
            ws.onclose = function() {
                console.log('WebSocket closed, reconnecting...');
                // Try to reconnect after a delay
                setTimeout(connectWebSocket, 1000);
            };
            ws.onerror = function(error) {
                console.error('WebSocket error:', error);
            };
        }
        
        connectWebSocket();
        
        // Load parameters on startup
        fetch('/api/parameters')
            .then(r => r.json())
            .then(data => {
                parameters = data.parameters;
                renderParameters();
                // Load list of available models for selection
                loadModels();
            });
        
        // Load current speed
        fetch('/api/speed')
            .then(r => r.json())
            .then(data => {
                document.getElementById('speed-slider').value = data.speed;
                document.getElementById('speed-display').textContent = data.speed.toFixed(1) + 'x';
            });
            
        // Load list of available models for selection
        function loadModels() {
            fetch('/api/models')
                .then(r => r.json())
                .then(data => {
                    const select = document.getElementById('model-select');
                    // Clear existing options
                    select.innerHTML = '';
                    const defaultOption = document.createElement('option');
                    defaultOption.value = '';
                    defaultOption.disabled = true;
                    defaultOption.selected = true;
                    defaultOption.textContent = 'Select a model';
                    select.appendChild(defaultOption);
                    data.models.forEach(m => {
                        const opt = document.createElement('option');
                        opt.value = m.filename;
                        opt.textContent = `${m.name} – ${m.category}`;
                        select.appendChild(opt);
                    });
                })
                .catch(err => console.error('Failed to load models', err));
        }

        async function loadSelectedModel() {
            const select = document.getElementById('model-select');
            const filename = select.value;
            if (!filename) {
                alert('Please select a model');
                return;
            }
            const btn = select.parentElement.parentElement.querySelector('button');
            const original = btn.textContent;
            btn.textContent = 'Loading...';
            btn.disabled = true;
            try {
                await fetch('/api/load_model', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify({ filename })
                });
                // The server will restart; give user feedback and reload page
                alert('Model loaded. The simulation will restart.');
                // Reload after short delay to allow server restart
                setTimeout(() => {
                    window.location.reload();
                }, 3000);
            } catch (e) {
                alert('Failed to load model: ' + e);
            } finally {
                btn.textContent = original;
                btn.disabled = false;
            }
        }

        // Initial load of models when page loads
        document.addEventListener('DOMContentLoaded', loadModels);
            
        // Load code
        fetch('/api/code')
            .then(r => r.json())
            .then(data => {
                if (data.code) {
                    document.getElementById('code-editor').value = data.code;
                    document.getElementById('filename-display').textContent = data.filename;
                }
            });
            
        function switchTab(tabName) {
            // Update tab styling
            document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
            event.target.classList.add('active');
            
            // Show/hide content
            if (tabName === 'simulation') {
                document.getElementById('simulation-view').style.display = 'grid';
                document.getElementById('editor-view').classList.remove('active');
            } else {
                document.getElementById('simulation-view').style.display = 'none';
                document.getElementById('editor-view').classList.add('active');
            }
        }
        
        async function saveAndRestart() {
            const code = document.getElementById('code-editor').value;
            const btn = document.querySelector('.editor-controls button');
            const originalText = btn.textContent;
            
            btn.textContent = 'Saving...';
            btn.disabled = true;
            
            try {
                // Save code
                await fetch('/api/code', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify({ code: code })
                });
                
                btn.textContent = 'Restarting...';
                
                // Trigger restart
                await fetch('/api/restart', { method: 'POST' });
                
                // Wait and reload page
                setTimeout(() => {
                    window.location.reload();
                }, 2000);
                
            } catch (e) {
                alert('Error saving/restarting: ' + e);
                btn.textContent = originalText;
                btn.disabled = false;
            }
        }
        
        function render(state) {
            // Clear canvas
            ctx.fillStyle = '#ffffff';
            ctx.fillRect(0, 0, canvas.width, canvas.height);

            const scaleX = canvas.width / state.environment.width;
            const scaleY = canvas.height / state.environment.height;

            console.log('Rendering state - turtles:', state.turtles.length, 'marks:', state.marks ? state.marks.length : 0);
            
            // Draw pheromones
            if (state.pheromones) {
                for (const [pid, grid] of Object.entries(state.pheromones)) {
                    // Choose color based on pheromone ID
                    let r=0, g=0, b=0;
                    if (pid.includes('food')) { r=0; g=255; b=0; }
                    else if (pid.includes('home') || pid.includes('nest')) { r=255; g=0; b=0; }
                    else { r=0; g=0; b=255; }
                    
                    // Draw each cell
                    for (let x = 0; x < grid.length; x++) {
                        for (let y = 0; y < grid[x].length; y++) {
                            const val = grid[x][y];
                            if (val > 0.1) {
                                // Logarithmic or linear scaling for visibility
                                const alpha = Math.min(0.8, val / 50.0); 
                                ctx.fillStyle = `rgba(${r}, ${g}, ${b}, ${alpha})`;
                                ctx.fillRect(x * scaleX, y * scaleY, scaleX, scaleY);
                            }
                        }
                    }
                }
            }
            
            // Draw grid
            ctx.strokeStyle = '#e9ecef';
            ctx.lineWidth = 1;
            const gridSize = 50;
            for (let x = 0; x < canvas.width; x += gridSize) {
                ctx.beginPath();
                ctx.moveTo(x, 0);
                ctx.lineTo(x, canvas.height);
                ctx.stroke();
            }
            for (let y = 0; y < canvas.width; y += gridSize) {
                ctx.beginPath();
                ctx.moveTo(0, y);
                ctx.lineTo(canvas.width, y);
                ctx.stroke();
            }

            // Draw marks (before turtles so they don't get covered)
            if (state.marks && state.marks.length > 0) {
                console.log('Drawing', state.marks.length, 'marks');
                state.marks.forEach((mark, index) => {
                    const x = mark.position[0] * scaleX;
                    const y = mark.position[1] * scaleY;
                    console.log('Mark', index, 'at canvas position:', x, y);

                    // Draw mark as a highly visible cross with background
                    const colors = ['red', 'blue', 'green', 'orange', 'purple', 'cyan'];
                    const color = colors[index % colors.length];

                    // Draw white background circle for visibility
                    ctx.fillStyle = 'white';
                    ctx.beginPath();
                    ctx.arc(x, y, 12, 0, 2 * Math.PI);
                    ctx.fill();

                    // Draw colored border
                    ctx.strokeStyle = color;
                    ctx.lineWidth = 4;
                    ctx.beginPath();
                    ctx.arc(x, y, 12, 0, 2 * Math.PI);
                    ctx.stroke();

                    // Draw cross
                    ctx.strokeStyle = color;
                    ctx.lineWidth = 3;
                    const size = 8;

                    ctx.beginPath();
                    ctx.moveTo(x - size, y);
                    ctx.lineTo(x + size, y);
                    ctx.moveTo(x, y - size);
                    ctx.lineTo(x, y + size);
                    ctx.stroke();

                    // Draw mark number for debugging
                    ctx.fillStyle = color;
                    ctx.font = '12px Arial';
                    ctx.textAlign = 'center';
                    ctx.fillText(index.toString(), x, y + 4);
                });
            }

            // Draw turtles
            state.turtles.forEach(turtle => {
                const x = turtle.position[0] * scaleX;
                const y = turtle.position[1] * scaleY;

                ctx.fillStyle = turtle.color;
                ctx.beginPath();
                ctx.arc(x, y, 5, 0, 2 * Math.PI);
                ctx.fill();

                // Draw heading indicator
                const headingLength = 10;
                const dx = Math.sin(turtle.heading) * headingLength;
                const dy = -Math.cos(turtle.heading) * headingLength;

                ctx.strokeStyle = turtle.color;
                ctx.lineWidth = 2;
                ctx.beginPath();
                ctx.moveTo(x, y);
                ctx.lineTo(x + dx, y + dy);
                ctx.stroke();
            });
            
            // Update FPS
            frameCount++;
            const now = Date.now();
            if (now - lastFrameTime >= 1000) {
                fps = frameCount;
                frameCount = 0;
                lastFrameTime = now;
            }
        }
        
        function updateStats(state) {
            document.getElementById('step').textContent = state.step;
            document.getElementById('turtles').textContent = state.num_turtles;
            document.getElementById('fps').textContent = fps;
        }
        
        function renderParameters() {
            const container = document.getElementById('parameters-container');
            container.innerHTML = '';
            
            for (const [key, value] of Object.entries(parameters)) {
                const group = document.createElement('div');
                group.className = 'control-group';
                
                const label = document.createElement('label');
                label.textContent = key.replace(/_/g, ' ').replace(/\b\w/g, l => l.toUpperCase());
                
                const valueDisplay = document.createElement('span');
                valueDisplay.className = 'value-display';
                valueDisplay.id = `param-${key}-display`;
                valueDisplay.textContent = value.toFixed(2);
                label.appendChild(valueDisplay);
                
                const input = document.createElement('input');
                input.type = 'number';
                input.id = `param-${key}`;
                input.value = value;
                input.step = '0.1';
                input.min = '0';
                input.onchange = () => updateParameter(key, parseFloat(input.value));
                
                group.appendChild(label);
                group.appendChild(input);
                container.appendChild(group);
            }
        }
        
        async function startSim() {
            await fetch('/api/start', { method: 'POST' });
            // After starting, fetch parameters to enable UI controls
            fetch('/api/parameters')
                .then(r => r.json())
                .then(data => {
                    parameters = data.parameters;
                    renderParameters();
                });
        }
        
        async function pauseSim() {
            await fetch('/api/pause', { method: 'POST' });
        }
        
        async function stopSim() {
            await fetch('/api/stop', { method: 'POST' });
        }
        
        async function resetSim() {
            await fetch('/api/reset', { method: 'POST' });
        }
        
        async function updateSpeed(value) {
            const speed = parseFloat(value);
            document.getElementById('speed-display').textContent = speed.toFixed(1) + 'x';
            document.getElementById('current-speed').textContent = speed.toFixed(1) + 'x';
            
            await fetch('/api/speed', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ speed: speed })
            });
        }
        
        async function updateParameter(key, value) {
            const display = document.getElementById(`param-${key}-display`);
            if (display) {
                display.textContent = value.toFixed(2);
            }
            
            parameters[key] = value;
            
            await fetch('/api/parameters', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ parameters: { [key]: value } })
            });
        }
        
        // Load current agent count on startup
        fetch('/api/agent_count')
            .then(r => r.json())
            .then(data => {
                document.getElementById('agent-count-slider').value = data.count;
                document.getElementById('agent-count-input').value = data.count;
            });
        
        function syncAgentCount(value, source) {
            const val = parseInt(value);
            if (source === 'slider') {
                document.getElementById('agent-count-input').value = val;
            } else {
                document.getElementById('agent-count-slider').value = val;
            }
        }
        
        async function applyAgentCount() {
            const input = document.getElementById('agent-count-input');
            const count = parseInt(input.value);
            
            const response = await fetch('/api/agent_count', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ count: count })
            });
            
            const result = await response.json();
            if (result.status === 'agent_count_updated') {
                // Update both inputs to reflect actual count (in case server clamped it)
                document.getElementById('agent-count-slider').value = result.count;
                document.getElementById('agent-count-input').value = result.count;
                console.log(`Agent count updated: ${result.previous} → ${result.count}`);
            }
        }
    </script>
</body>
</html>
"""
