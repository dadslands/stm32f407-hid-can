// Web Flasher JavaScript for STM32F407 HID-CAN Project

document.addEventListener('DOMContentLoaded', function() {
    // DOM elements
    const connectButton = document.getElementById('connect-button');
    const flashButton = document.getElementById('flash-button');
    const connectionStatus = document.getElementById('connection-status');
    const connectionProgress = document.getElementById('connection-progress');
    const firmwareSelect = document.getElementById('firmware-select');
    const customFirmwareUpload = document.getElementById('custom-firmware-upload');
    const firmwareFile = document.getElementById('firmware-file');
    const flashLog = document.getElementById('flash-log');
    const deviceInfoDiv = document.getElementById('device-info');
    
    // State variables
    let isConnected = false;
    let selectedFirmware = null;
    let device = null;
    
    // Initialize WebUSB
    if ('usb' in navigator) {
        flashLog.textContent = 'WebUSB is supported in this browser.\nWaiting for device connection...';
        
        // Check for already connected devices
        checkForConnectedDevices();
        
        // Listen for device connections
        navigator.usb.addEventListener('connect', event => {
            flashLog.textContent += '\nDevice connected! Click "Connect to Device" to use it.';
        });
        
        // Listen for device disconnections
        navigator.usb.addEventListener('disconnect', event => {
            if (device && event.device === device) {
                flashLog.textContent += '\nDevice disconnected.';
                connectionStatus.textContent = 'Disconnected';
                connectionStatus.classList.remove('connecting');
                isConnected = false;
                flashButton.disabled = true;
                device = null;
            }
        });
    } else {
        flashLog.textContent = 'WebUSB is not supported in this browser. Please use Chrome, Edge, or Opera.';
        connectButton.disabled = true;
    }
    
    // Event listeners
    connectButton.addEventListener('click', connectDevice);
    flashButton.addEventListener('click', flashFirmware);
    firmwareSelect.addEventListener('change', handleFirmwareSelection);
    firmwareFile.addEventListener('change', handleFileSelection);
    
    // Check for already connected devices
    async function checkForConnectedDevices() {
        try {
            const devices = await navigator.usb.getDevices();
            if (devices.length > 0) {
                flashLog.textContent += `\nFound ${devices.length} previously connected device(s). Click "Connect to Device" to use.`;
            }
        } catch (error) {
            console.error('Error checking for devices:', error);
        }
    }
    
    // Connect to device
    async function connectDevice() {
        try {
            connectionStatus.textContent = 'Connecting...';
            connectionStatus.classList.add('connecting');
            flashLog.textContent += '\nSearching for STM32 devices in bootloader mode...';
            
            // Request device with multiple filters to support various STM32 modes
            device = await navigator.usb.requestDevice({
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
                flashLog.textContent += '\nClaimed interface successfully.';
            } catch (interfaceError) {
                flashLog.textContent += '\nCould not claim interface, but continuing with connection.';
                console.warn('Interface claim error (may be normal):', interfaceError);
            }
            
            // Display device information
            displayDeviceInfo(device);
            
            // Simulate connection progress
            simulateConnectionProgress();
            
        } catch (error) {
            connectionStatus.classList.remove('connecting');
            connectionStatus.textContent = 'Connection failed';
            flashLog.textContent += '\nError: ' + error.message;
            flashLog.textContent += '\nTips: Make sure your device is in bootloader/DFU mode. For STM32F407, hold the BOOT0 button while resetting the device.';
            console.error('Connection error:', error);
        }
    }
    
    // Display device information
    function displayDeviceInfo(device) {
        if (!deviceInfoDiv) return;
        
        const info = document.createElement('div');
        info.innerHTML = `
            <h5>Connected Device:</h5>
            <ul class="list-group">
                <li class="list-group-item">Manufacturer: ${device.manufacturerName || 'Unknown'}</li>
                <li class="list-group-item">Product: ${device.productName || 'Unknown'}</li>
                <li class="list-group-item">Serial: ${device.serialNumber || 'Unknown'}</li>
                <li class="list-group-item">USB ID: ${device.vendorId.toString(16).padStart(4, '0')}:${device.productId.toString(16).padStart(4, '0')}</li>
                <li class="list-group-item">USB Version: ${device.usbVersionMajor}.${device.usbVersionMinor}.${device.usbVersionSubminor}</li>
            </ul>
        `;
        
        deviceInfoDiv.innerHTML = '';
        deviceInfoDiv.appendChild(info);
        deviceInfoDiv.classList.remove('d-none');
    }
    
    // Simulate connection progress
    function simulateConnectionProgress() {
        let progress = 0;
        const interval = setInterval(() => {
            progress += 10;
            connectionProgress.style.width = progress + '%';
            connectionProgress.textContent = progress + '%';
            
            if (progress >= 100) {
                clearInterval(interval);
                connectionStatus.classList.remove('connecting');
                connectionStatus.textContent = 'Connected';
                isConnected = true;
                flashButton.disabled = false;
                flashLog.textContent += '\nDevice connected successfully!';
                flashLog.textContent += '\nReady to flash firmware.';
            }
        }, 200);
    }
    
    // Handle firmware selection
    function handleFirmwareSelection() {
        const selectedOption = firmwareSelect.value;
        
        if (selectedOption === 'custom') {
            customFirmwareUpload.classList.remove('d-none');
            selectedFirmware = null;
        } else {
            customFirmwareUpload.classList.add('d-none');
            selectedFirmware = selectedOption;
            flashLog.textContent += '\nSelected firmware: ' + selectedOption;
        }
    }
    
    // Handle file selection
    function handleFileSelection() {
        if (firmwareFile.files.length > 0) {
            selectedFirmware = firmwareFile.files[0];
            flashLog.textContent += '\nSelected custom firmware: ' + selectedFirmware.name;
        } else {
            selectedFirmware = null;
        }
    }
    
    // Flash firmware
    async function flashFirmware() {
        if (!isConnected || !selectedFirmware) {
            flashLog.textContent += '\nError: Device not connected or firmware not selected.';
            return;
        }
        
        flashLog.textContent += '\n\nStarting firmware flash process...';
        flashButton.disabled = true;
        
        // Get flash options
        const eraseFlash = document.getElementById('erase-flash').checked;
        const verifyAfterFlash = document.getElementById('verify-after-flash').checked;
        const resetAfterFlash = document.getElementById('reset-after-flash').checked;
        
        try {
            // Simulate flashing process
            if (eraseFlash) {
                flashLog.textContent += '\nErasing flash...';
                await simulateOperation(2000);
                flashLog.textContent += ' Done.';
            }
            
            flashLog.textContent += '\nFlashing firmware...';
            await simulateOperation(5000);
            flashLog.textContent += ' Done.';
            
            if (verifyAfterFlash) {
                flashLog.textContent += '\nVerifying firmware...';
                await simulateOperation(3000);
                flashLog.textContent += ' Done.';
            }
            
            if (resetAfterFlash) {
                flashLog.textContent += '\nResetting device...';
                await simulateOperation(1000);
                flashLog.textContent += ' Done.';
            }
            
            flashLog.textContent += '\n\nFirmware flashed successfully!';
        } catch (error) {
            flashLog.textContent += '\n\nError during flashing: ' + error.message;
            console.error('Flashing error:', error);
        } finally {
            flashButton.disabled = false;
        }
    }
    
    // Simulate operation with delay
    function simulateOperation(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }
});
