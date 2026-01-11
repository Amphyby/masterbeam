#include <furi.h>
#include <notification/notification_messages.h>
#include <ble/ble.h>
#include <furi_ble/gatt.h>
#include <string.h>

#include "../include/masterbeam.h"

#define TAG "Masterbeam_BLE"

// Flipper Zero BLE device name
#define FLIPPER_BLE_NAME "Masterbeam"

// GATT Service and Characteristic UUIDs
// Service UUID: 0x181C (User Data Service - standard BLE UUID)
// Status Characteristic UUID: 0x2A9F (User Control Point - standard BLE UUID, readable)
// Command Characteristic UUID: 0x2A9F (User Control Point - standard BLE UUID, writable)

#define MASTERBEAM_SERVICE_UUID 0x181C
#define MASTERBEAM_CHAR_STATUS  0x2A9F
#define MASTERBEAM_CHAR_COMMAND 0x2A9F

// BLE advertising start - Flipper advertises itself as BLE peripheral
static bool ble_start_advertising(BleContext* ble_ctx) {
    if(!ble_ctx) return false;

    furi_mutex_acquire(ble_ctx->mutex, FuriWaitForever);

    // Setup 16-bit UUIDs for standard service and custom characteristics
    Service_UUID_t service_uuid;
    Char_UUID_t status_uuid, command_uuid;

    service_uuid.Service_UUID_16 = MASTERBEAM_SERVICE_UUID; // 0x181C (User Data Service)
    status_uuid.Char_UUID_16 = MASTERBEAM_CHAR_STATUS; // 0xA001
    command_uuid.Char_UUID_16 = MASTERBEAM_CHAR_COMMAND; // 0xA002

    // Add GATT service (User Data Service 0x181C) with characteristics
    // Max attributes: 1 (service) + 4 (status char: decl+value+uuid+data) + 4 (command char: decl+value+uuid+data)
    if(!ble_gatt_service_add(
           UUID_TYPE_16, &service_uuid, PRIMARY_SERVICE, 9, &ble_ctx->svc_handle)) {
        FURI_LOG_E(TAG, "Failed to add GATT service");
        furi_mutex_release(ble_ctx->mutex);
        return false;
    }

    FURI_LOG_I(TAG, "GATT service added - Service Handle: 0x%04x", ble_ctx->svc_handle);

    // Define Status characteristic (readable by Arduino)
    static const uint8_t status_value = 0x00; // Initially disconnected
    const BleGattCharacteristicParams status_char_desc = {
        .name = "Status",
        .uuid = status_uuid,
        .uuid_type = UUID_TYPE_16,
        .char_properties = CHAR_PROP_READ,
        .security_permissions = ATTR_PERMISSION_NONE,
        .gatt_evt_mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
        .is_variable = 0,
        .data_prop_type = FlipperGattCharacteristicDataFixed,
        .data = {.fixed = {.ptr = &status_value, .length = 1}},
        .descriptor_params = NULL,
    };

    ble_gatt_characteristic_init(ble_ctx->svc_handle, &status_char_desc, &ble_ctx->char_status);
    FURI_LOG_I(TAG, "Status characteristic added");

    // Define Command characteristic (writable from Arduino)
    static uint8_t command_value[256] = {0};
    const BleGattCharacteristicParams command_char_desc = {
        .name = "Command",
        .uuid = command_uuid,
        .uuid_type = UUID_TYPE_16,
        .char_properties = CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RESP,
        .security_permissions = ATTR_PERMISSION_NONE,
        .gatt_evt_mask = GATT_NOTIFY_ATTRIBUTE_WRITE,
        .is_variable = 1,
        .data_prop_type = FlipperGattCharacteristicDataFixed,
        .data = {.fixed = {.ptr = command_value, .length = sizeof(command_value)}},
        .descriptor_params = NULL,
    };

    ble_gatt_characteristic_init(ble_ctx->svc_handle, &command_char_desc, &ble_ctx->char_command);
    FURI_LOG_I(TAG, "Command characteristic added");

    FURI_LOG_I(
        TAG,
        "BLE advertising started - Service UUID: 0x%04X (User Data Service), Device: %s",
        MASTERBEAM_SERVICE_UUID,
        FLIPPER_BLE_NAME);

    furi_mutex_release(ble_ctx->mutex);
    return true;
}

// BLE connection handler
static bool ble_setup_connection(BleContext* ble_ctx) {
    if(!ble_ctx) return false;

    furi_mutex_acquire(ble_ctx->mutex, FuriWaitForever);

    // Initialize connection status as disconnected
    ble_ctx->is_connected = false;

    FURI_LOG_I(TAG, "BLE peripheral setup complete - waiting for Arduino connection");

    furi_mutex_release(ble_ctx->mutex);
    return true;
}

static int32_t ble_connection_thread(void* ctx) {
    BleContext* ble_ctx = (BleContext*)ctx;
    ble_setup_connection(ble_ctx);
    return 0;
}

BleContext* ble_context_alloc(void) {
    BleContext* ble_ctx = malloc(sizeof(BleContext));
    ble_ctx->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    ble_ctx->thread = NULL;
    ble_ctx->is_connected = false; // Initially disconnected
    ble_ctx->svc_handle = 0; // GATT service handle (will be set by ble_start_advertising)
    memset(&ble_ctx->char_status, 0, sizeof(ble_ctx->char_status));
    memset(&ble_ctx->char_command, 0, sizeof(ble_ctx->char_command));

    // Start BLE advertising immediately on app startup
    ble_start_advertising(ble_ctx);

    return ble_ctx;
}

void ble_context_free(BleContext* ble_ctx) {
    if(!ble_ctx) return;

    if(ble_ctx->thread) {
        furi_thread_join(ble_ctx->thread);
        furi_thread_free(ble_ctx->thread);
    }

    // Delete characteristics
    if(ble_ctx->svc_handle) {
        ble_gatt_characteristic_delete(ble_ctx->svc_handle, &ble_ctx->char_status);
        ble_gatt_characteristic_delete(ble_ctx->svc_handle, &ble_ctx->char_command);
        // Delete service
        ble_gatt_service_delete(ble_ctx->svc_handle);
    }

    furi_mutex_free(ble_ctx->mutex);
    free(ble_ctx);
}

void ble_start_connection(BleContext* ble_ctx) {
    if(!ble_ctx) return;

    if(ble_ctx->thread) {
        furi_thread_join(ble_ctx->thread);
        furi_thread_free(ble_ctx->thread);
    }

    ble_ctx->thread = furi_thread_alloc_ex("BLE_Connection", 1024, ble_connection_thread, ble_ctx);
    furi_thread_start(ble_ctx->thread);
}

void ble_send_command(BleContext* ble_ctx, const char* command) {
    if(!ble_ctx) return;

    FURI_LOG_I(TAG, "Sending command: %s", command);
    // Command sending implementation would go here
}

bool ble_is_connected(BleContext* ble_ctx) {
    if(!ble_ctx) return false;

    furi_mutex_acquire(ble_ctx->mutex, FuriWaitForever);
    bool connected = ble_ctx->is_connected;
    furi_mutex_release(ble_ctx->mutex);

    return connected;
}

void ble_set_connected(BleContext* ble_ctx, bool connected) {
    if(!ble_ctx) return;

    furi_mutex_acquire(ble_ctx->mutex, FuriWaitForever);
    ble_ctx->is_connected = connected;
    furi_mutex_release(ble_ctx->mutex);

    // Update status characteristic value
    uint8_t status_value = connected ? 0x01 : 0x00;
    ble_gatt_characteristic_update(ble_ctx->svc_handle, &ble_ctx->char_status, &status_value);

    if(connected) {
        FURI_LOG_I(TAG, "Arduino connected to Flipper - Status characteristic updated to: 0x01");
    } else {
        FURI_LOG_I(
            TAG, "Arduino disconnected from Flipper - Status characteristic updated to: 0x00");
    }
}
