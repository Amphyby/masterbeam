# Masterbeam BLE Architecture

## Overview
The Masterbeam application implements a **BLE peripheral device** on the Flipper Zero that exposes a GATT service for Arduino (central/master device) to connect to and receive button press commands. Button presses on the Flipper are transmitted to the Arduino via BLE characteristic notifications.

## Device Roles
- **Arduino**: Central Device (Master) - Initiates connection and subscribes to characteristic notifications
- **Flipper Zero**: Peripheral Device (Slave) - Advertises service and sends button press commands via characteristic updates

## GATT Service Structure

### Service UUID
```
0x181C (User Data Service - Standard Bluetooth SIG UUID)
```

### Characteristics

#### Command Characteristic (Button Press Events)
- **UUID**: `0x2A9F` (User Control Point - Standard Bluetooth SIG UUID)
- **Properties**: READ | WRITE | WRITE_WITHOUT_RESP | NOTIFY
- **Value**: 1 byte (persistent in BleContext buffer)
  - `0x00` = No command / idle
  - `0x01` = LEFT button pressed
  - `0x02` = RIGHT button pressed
  - `0x03` = UP button pressed
  - `0x04` = DOWN button pressed
  - `0x05` = OK button pressed
- **Description**: Characteristic updated when Flipper button is pressed. Arduino receives NOTIFY events with the command byte. Data persists in `ble_ctx->command_value` buffer.

## API Functions

### Connection Status Management
```c
// Check if Arduino is connected to Flipper
bool ble_is_connected(BleContext* ble_ctx);

// Set/update connection status (called when Arduino connects/disconnects)
void ble_set_connected(BleContext* ble_ctx, bool connected);
```

### Initialization
```c
// Allocate and initialize BLE context
// Automatically starts advertising the Masterbeam service
// Returns: pointer to BleContext structure
BleContext* ble_context_alloc(void);

// Free BLE context and clean up resources
void ble_context_free(BleContext* ble_ctx);
```

### Communication
```c
// Optionally start BLE connection handler thread
// (currently BLE runs immediately on ble_context_alloc)
void ble_start_connection(BleContext* ble_ctx);

// Send button command to connected Arduino clients
// command: One of "LEFT", "RIGHT", "UP", "DOWN", "OK"
// Maps to 0x01-0x05 and updates characteristic value
// Connected clients receive NOTIFY events
void ble_send_command(BleContext* ble_ctx, const char* command);
```

## Connection Flow

1. **Flipper Startup**: `ble_context_alloc()` called in app initialization
2. **GATT Service Registration**: Service 0x181C added with Command characteristic 0x2A9F
3. **BLE Advertising**: Flipper advertises "Masterbeam" as BLE peripheral
4. **Arduino Discovery**: Arduino scans for BLE devices and finds "Masterbeam"
5. **Arduino Connection**: Arduino connects to Flipper's GATT service and subscribes to NOTIFY on Command characteristic
6. **Button Press Events**: User presses button on Flipper
7. **Characteristic Update**: Button press triggers `ble_send_command()` which:
   - Maps button name to 0x01-0x05 byte value
   - Updates `command_value` buffer in BleContext
   - Calls `ble_gatt_characteristic_update()` to notify connected clients
8. **Arduino Receives**: Arduino receives NOTIFY event with new command value
9. **Disconnection**: When Arduino disconnects, `ble_set_connected(false)` can be called to update status

## Key Data Structure

```c
typedef struct {
    FuriThread* thread;                          // BLE connection thread (optional)
    FuriMutex* mutex;                            // Thread-safe access to shared state
    bool is_connected;                           // Connection status flag
    uint16_t svc_handle;                         // GATT service handle (from ble_gatt_service_add)
    BleGattCharacteristicInstance char_command;  // Command characteristic instance
    uint8_t command_value;                       // Command value buffer (persistent, points to characteristic data)
} BleContext;
```

### Buffer Persistence
The `command_value` uint8_t buffer is persistent across function calls. It is:
- Allocated as part of BleContext
- Referenced by the GATT characteristic via `FlipperGattCharacteristicDataFixed`
- Updated when `ble_send_command()` is called
- Automatically accessible to connected BLE clients

## Thread Safety
All BLE operations are protected by a mutex (`FuriMutex`) to ensure thread-safe access to shared state, including the `is_connected` flag and characteristic handles.

## Input Handling (Button Press to BLE)
Button presses on Flipper are captured via input event callbacks in the UI scene:
1. Physical button press detected by Flipper hardware
2. Input event callback in `scene_main_panel.c` captures InputKey event
3. Button event is mapped to command string ("LEFT", "RIGHT", etc.)
4. `ble_send_command()` is called with command string
5. Command is mapped to byte value (0x01-0x05)
6. BLE characteristic is updated, triggering NOTIFY to connected clients

The input event callback is registered on the text box view using:
```c
view_set_input_callback(view, scene_main_panel_input_callback);
view_set_context(view, app);
```

## Device Advertisement
- **Device Name**: `Masterbeam`
- **Advertising Mode**: Enabled at application startup (via `ble_context_alloc()`)
- **Visibility**: Discoverable to all nearby Bluetooth devices
- **Service UUID**: 0x181C (included in advertisement)
- **No Authentication**: Open connection, no passkey required

## Debugging
Debug logging is enabled throughout the BLE stack using FURI_LOG_I() with TAG "Masterbeam_BLE":
- Service and characteristic registration
- Button press detection and command mapping
- Characteristic update results
- Connection status changes

Monitor logs via:
```bash
./fbt cli
```

## Architecture Diagram

```
┌─────────────────────────────────────────────┐
│          Flipper Zero (Peripheral)          │
├─────────────────────────────────────────────┤
│                                             │
│  User presses button (LEFT/RIGHT/UP/DOWN/OK)│
│           ↓                                  │
│  Input Event Callback (view-level)          │
│           ↓                                  │
│  ble_send_command(app->ble_ctx, "LEFT")    │
│           ↓                                  │
│  Map to 0x01, update command_value buffer   │
│           ↓                                  │
│  ble_gatt_characteristic_update()           │
│           ↓                                  │
│  BLE Stack sends NOTIFY to subscribers      │
│                                             │
│  GATT Service: 0x181C                       │
│  ├─ Command Char: 0x2A9F (NOTIFY)          │
│     value: 0x01-0x05 (button commands)      │
│                                             │
└────────────────────┬────────────────────────┘
                     │ BLE Connection
                     │ (Service UUID 0x181C)
                     │
┌────────────────────▼────────────────────────┐
│       Arduino / Central Device              │
├─────────────────────────────────────────────┤
│                                             │
│  Scan for BLE devices                       │
│  Find "Masterbeam" (0x181C)                 │
│  Connect and subscribe to NOTIFY            │
│  on characteristic 0x2A9F                   │
│           ↓                                  │
│  Receive NOTIFY with button value (0x01-05)│
│  Execute corresponding action               │
│                                             │
└─────────────────────────────────────────────┘
```
