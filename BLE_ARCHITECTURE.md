# Masterbeam BLE Architecture

## Overview
The Masterbeam application implements a **peripheral BLE device** on the Flipper Zero that exposes a GATT service for Arduino (central/master device) to connect to and read the connection status.

## Device Roles
- **Arduino**: Central Device (Master) - Initiates connection and reads Flipper's GATT characteristics
- **Flipper Zero**: Peripheral Device (Slave) - Advertises service and exposes characteristics for Arduino to read/write

## GATT Service Structure

### Service UUID
```
12345678-1234-5678-1234-56789ABCDEF0
```

### Characteristics

#### 1. Status Characteristic (Connection Status)
- **UUID**: `12345678-1234-5678-1234-56789ABCDEF1`
- **Properties**: READ
- **Value**: 1 byte
  - `0x01` = Connected
  - `0x00` = Disconnected
- **Description**: Arduino reads this characteristic to determine if Flipper is connected

#### 2. Command Characteristic
- **UUID**: `12345678-1234-5678-1234-56789ABCDEF2`
- **Properties**: WRITE
- **Value**: Variable length
- **Description**: Arduino can write commands to this characteristic for Flipper to process

## API Functions

### Connection Status Management
```c
// Check if Arduino is connected to Flipper
bool ble_is_connected(BleContext* ble_ctx);

// Update connection status (called when Arduino connects/disconnects)
void ble_set_connected(BleContext* ble_ctx, bool connected);
```

### Initialization
```c
// Allocate and initialize BLE context
// Automatically starts advertising the Masterbeam service
BleContext* ble_context_alloc(void);

// Free BLE context and clean up resources
void ble_context_free(BleContext* ble_ctx);
```

### Communication
```c
// Start BLE connection handler thread
void ble_start_connection(BleContext* ble_ctx);

// Send command to Arduino (for bidirectional communication)
void ble_send_command(BleContext* ble_ctx, const char* command);
```

## Connection Flow

1. **Flipper Startup**: Flipper starts advertising the Masterbeam BLE service
2. **Arduino Discovery**: Arduino scans for BLE devices and finds "Masterbeam"
3. **Arduino Connection**: Arduino connects to Flipper's GATT service
4. **Status Query**: Arduino periodically reads the Status Characteristic to check connection state
5. **Command Execution**: Arduino can write commands to Command Characteristic
6. **Disconnection**: When Arduino disconnects, status updates to 0x00

## Key Data Structure

```c
typedef struct {
    FuriThread* thread;        // BLE connection thread
    FuriMutex* mutex;          // Thread-safe access to shared state
    bool is_connected;         // Connection status (exposed via Status Characteristic)
} BleContext;
```

## Thread Safety
All BLE operations are protected by a mutex (`FuriMutex`) to ensure thread-safe access to the connection status and other shared resources.

## Device Advertisement
- **Device Name**: `Masterbeam`
- **Advertising Mode**: Always on (no authentication required)
- **Visibility**: Discoverable to all nearby Bluetooth devices
