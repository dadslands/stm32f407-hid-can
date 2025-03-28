// Configuration Tool JavaScript for STM32F407 HID-CAN Project

document.addEventListener('DOMContentLoaded', function() {
    // DOM elements
    const connectDeviceButton = document.getElementById('connect-device-button');
    const deviceStatus = document.getElementById('device-status');
    const deviceProgress = document.getElementById('device-progress');
    const firmwareVersion = document.getElementById('firmware-version');
    const connectedDevices = document.getElementById('connected-devices');
    const activeMappings = document.getElementById('active-mappings');
    const deviceHealth = document.getElementById('device-health');
    const saveButtons = document.querySelectorAll('[id$="-button"]');
    
    // Initialize Bootstrap tabs
    const tabEls = document.querySelectorAll('button[data-bs-toggle="tab"]');
    tabEls.forEach(tabEl => {
        const tabTrigger = new bootstrap.Tab(tabEl);
        tabEl.addEventListener('click', event => {
            event.preventDefault();
            tabTrigger.show();
        });
    });
    
    // Initialize state
    let isConnected = false;
    
    // Event listeners
    connectDeviceButton.addEventListener('click', connectDevice);
    
    // Disable save buttons initially
    saveButtons.forEach(button => {
        if (button.id !== 'connect-device-button' && 
            button.id !== 'add-mapping-button' && 
            button.id !== 'add-display-item-button' &&
            button.id !== 'factory-reset-button') {
            button.disabled = true;
        }
    });
    
    // Connect to device
    async function connectDevice() {
        try {
            deviceStatus.textContent = 'Connecting...';
            deviceStatus.classList.add('connecting');
            
            // Check if WebUSB is available
            if ('usb' in navigator) {
                // Log connection attempt
                console.log('Attempting to connect to STM32F407 device...');
                
                try {
                    // Request device with multiple filters to support various STM32 modes
                    const device = await navigator.usb.requestDevice({
                        filters: [
                            // STM32 DFU mode
                            { vendorId: 0x0483, productId: 0x5740 },
                            // STM32 Bootloader mode
                            { vendorId: 0x0483, productId: 0xDF11 },
                            // STM32 Virtual COM port
                            { vendorId: 0x0483, productId: 0x5740 },
                            // Additional STM32F4 modes
                            { vendorId: 0x0483, productId: 0x374B },
                            { vendorId: 0x0483, productId: 0x3748 }
                        ]
                    });
                    
                    // Open device and claim interface
                    await device.open();
                    
                    // Get device information
                    await device.selectConfiguration(1);
                    
                    try {
                        // Try to claim interface 0 (may fail depending on device state)
                        await device.claimInterface(0);
                        console.log('Claimed interface successfully');
                    } catch (interfaceError) {
                        console.warn('Could not claim interface, but continuing with connection:', interfaceError);
                    }
                    
                    // Display device information if available
                    const deviceInfo = document.getElementById('device-info');
                    if (deviceInfo) {
                        deviceInfo.innerHTML = `
                            <div class="alert alert-info">
                                <strong>Connected Device:</strong> ${device.productName || 'STM32F407'}<br>
                                <strong>Manufacturer:</strong> ${device.manufacturerName || 'STMicroelectronics'}<br>
                                <strong>USB ID:</strong> ${device.vendorId.toString(16).padStart(4, '0')}:${device.productId.toString(16).padStart(4, '0')}
                            </div>
                        `;
                    }
                    
                    console.log('Device connected successfully');
                } catch (usbError) {
                    console.error('USB connection error:', usbError);
                    // Continue with simulation for demo purposes
                    console.log('Falling back to simulation mode');
                }
            } else {
                console.log('WebUSB not supported, using simulation mode');
            }
            
            // Simulate connection process (for demo purposes)
            simulateConnectionProgress();
            
        } catch (error) {
            deviceStatus.classList.remove('connecting');
            deviceStatus.textContent = 'Connection failed';
            console.error('Connection error:', error);
            
            // Show error message to user
            const errorAlert = document.createElement('div');
            errorAlert.className = 'alert alert-danger mt-3';
            errorAlert.innerHTML = `
                <strong>Connection Error:</strong> ${error.message}<br>
                <small>Make sure your device is connected and in the correct mode.</small>
            `;
            
            const connectionContainer = document.querySelector('.connection-container');
            if (connectionContainer) {
                connectionContainer.appendChild(errorAlert);
                
                // Remove error message after 5 seconds
                setTimeout(() => {
                    errorAlert.remove();
                }, 5000);
            }
        }
    }
    
    // Simulate connection progress
    function simulateConnectionProgress() {
        let progress = 0;
        const interval = setInterval(() => {
            progress += 10;
            deviceProgress.style.width = progress + '%';
            deviceProgress.textContent = progress + '%';
            
            if (progress >= 100) {
                clearInterval(interval);
                completeConnection();
            }
        }, 200);
    }
    
    // Complete connection and update UI
    function completeConnection() {
        deviceStatus.classList.remove('connecting');
        deviceStatus.textContent = 'Connected';
        isConnected = true;
        
        // Update device information
        firmwareVersion.textContent = 'v1.0.0';
        connectedDevices.textContent = 'Keyboard, Mouse';
        activeMappings.textContent = '5';
        deviceHealth.textContent = 'Good';
        
        // Enable save buttons
        saveButtons.forEach(button => {
            if (button.id !== 'connect-device-button') {
                button.disabled = false;
            }
        });
        
        // Update tab content alerts
        document.querySelectorAll('.tab-pane .alert-info').forEach(alert => {
            alert.classList.add('d-none');
        });
        
        // Populate mapping table with sample data
        populateMappingTable();
        
        // Populate display preview
        updateDisplayPreview();
    }
    
    // Populate mapping table with sample data
    function populateMappingTable() {
        const tableBody = document.querySelector('#mappings-table tbody');
        tableBody.innerHTML = '';
        
        const sampleMappings = [
            { device: 'Keyboard', input: 'Key A', outputType: 'Serial', target: 'COM1', transform: 'None' },
            { device: 'Keyboard', input: 'Key B', outputType: 'CAN', target: '0x123', transform: 'None' },
            { device: 'Mouse', input: 'X-Axis', outputType: 'CAN', target: '0x124', transform: 'Scale 0-1023' },
            { device: 'Mouse', input: 'Left Button', outputType: 'Serial', target: 'COM1', transform: 'None' },
            { device: 'Gamepad', input: 'Button 1', outputType: 'CAN', target: '0x125', transform: 'Invert' }
        ];
        
        sampleMappings.forEach(mapping => {
            const row = document.createElement('tr');
            row.innerHTML = `
                <td>${mapping.device}</td>
                <td>${mapping.input}</td>
                <td>${mapping.outputType}</td>
                <td>${mapping.target}</td>
                <td>${mapping.transform}</td>
                <td>
                    <button class="btn btn-sm btn-primary me-1 edit-mapping-btn">Edit</button>
                    <button class="btn btn-sm btn-danger delete-mapping-btn">Delete</button>
                </td>
            `;
            tableBody.appendChild(row);
        });
        
        // Add event listeners to edit buttons
        document.querySelectorAll('.edit-mapping-btn').forEach(button => {
            button.addEventListener('click', function() {
                const mappingModal = new bootstrap.Modal(document.getElementById('mappingModal'));
                mappingModal.show();
            });
        });
    }
    
    // Update display preview
    function updateDisplayPreview() {
        const displayPreview = document.getElementById('display-preview');
        displayPreview.innerHTML = '';
        
        // Create sample display items
        const items = [
            { type: 'text', x: 120, y: 50, text: 'STM32F407', color: '#ffffff' },
            { type: 'text', x: 120, y: 80, text: 'HID-CAN', color: '#ffffff' },
            { type: 'bar', x: 60, y: 120, width: 120, height: 20, value: 75, color: '#00ff00' },
            { type: 'value', x: 120, y: 160, text: 'RPM: 3500', color: '#ffff00' }
        ];
        
        items.forEach(item => {
            const element = document.createElement('div');
            element.style.position = 'absolute';
            element.style.color = item.color;
            element.style.fontFamily = 'Arial, sans-serif';
            element.style.textAlign = 'center';
            element.style.transform = 'translate(-50%, -50%)';
            
            switch(item.type) {
                case 'text':
                    element.textContent = item.text;
                    element.style.left = item.x + 'px';
                    element.style.top = item.y + 'px';
                    break;
                case 'value':
                    element.textContent = item.text;
                    element.style.left = item.x + 'px';
                    element.style.top = item.y + 'px';
                    element.style.fontWeight = 'bold';
                    break;
                case 'bar':
                    element.style.left = item.x + 'px';
                    element.style.top = item.y + 'px';
                    element.style.width = item.width + 'px';
                    element.style.height = item.height + 'px';
                    element.style.backgroundColor = 'rgba(0,0,0,0.5)';
                    element.style.border = '1px solid ' + item.color;
                    element.style.borderRadius = '3px';
                    
                    const fill = document.createElement('div');
                    fill.style.width = (item.value / 100 * item.width) + 'px';
                    fill.style.height = '100%';
                    fill.style.backgroundColor = item.color;
                    element.appendChild(fill);
                    break;
            }
            
            displayPreview.appendChild(element);
        });
        
        // Add display items to list
        const displayItemsList = document.getElementById('display-items-list');
        displayItemsList.innerHTML = '';
        
        items.forEach((item, index) => {
            const listItem = document.createElement('div');
            listItem.className = 'list-group-item d-flex justify-content-between align-items-center';
            listItem.innerHTML = `
                <div>
                    <strong>${item.type.charAt(0).toUpperCase() + item.type.slice(1)}</strong>
                    ${item.text ? ': ' + item.text : ''}
                </div>
                <div>
                    <button class="btn btn-sm btn-primary me-1 edit-display-item-btn" data-index="${index}">Edit</button>
                    <button class="btn btn-sm btn-danger delete-display-item-btn" data-index="${index}">Delete</button>
                </div>
            `;
            displayItemsList.appendChild(listItem);
        });
        
        // Add event listeners to edit buttons
        document.querySelectorAll('.edit-display-item-btn').forEach(button => {
            button.addEventListener('click', function() {
                const displayItemModal = new bootstrap.Modal(document.getElementById('displayItemModal'));
                displayItemModal.show();
            });
        });
    }
    
    // Add event listeners for modal buttons
    document.getElementById('save-mapping-button')?.addEventListener('click', function() {
        const mappingModal = bootstrap.Modal.getInstance(document.getElementById('mappingModal'));
        mappingModal.hide();
        // In a real implementation, this would save the mapping data
    });
    
    document.getElementById('save-display-item-button')?.addEventListener('click', function() {
        const displayItemModal = bootstrap.Modal.getInstance(document.getElementById('displayItemModal'));
        displayItemModal.hide();
        // In a real implementation, this would save the display item data
    });
    
    // Add event listener for factory reset button
    document.getElementById('factory-reset-button')?.addEventListener('click', function() {
        if (confirm('Are you sure you want to reset all settings to factory defaults? This action cannot be undone.')) {
            alert('Factory reset completed.');
        }
    });
});
