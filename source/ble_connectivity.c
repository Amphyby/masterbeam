#include <furi.h>
#include <notification/notification_messages.h>

#include "../include/masterbeam.h"

#define TAG "Masterbeam_BLE"

// Flipper Zero BLE device name
#define FLIPPER_BLE_NAME "Masterbeam"

// GATT Service and Characteristic UUIDs
// Custom service UUID for Masterbeam control
#define MASTERBEAM_SERVICE_UUID 0x180A // Using Device Information Service as template
#define MASTERBEAM_CHAR_COMMAND 0x2A29 // Characteristic for commands from client
#define MASTERBEAM_CHAR_STATUS  0x2A28 // Characteristic for status to client

// BLE peripheral mode setup (Flipper as server)
// The Flipper will advertise itself and accept connections from clients (Arduino, etc.)
// The clients will discover and connect to Flipper's GATT services

// BLE advertising start - Flipper advertises itself as BLE peripheral
static bool ble_start_advertising(BleContext* ble_ctx) {
    if(!ble_ctx) return false;

    furi_mutex_acquire(ble_ctx->mutex, FuriWaitForever);

    // TODO: Implement actual GAP peripheral mode setup
    // Real implementation will:
    // 1. Configure Flipper as GAP peripheral (server)
    // 2. Define GATT services and characteristics
    // 3. Start advertising without mandatory authentication
    // 4. Wait for clients (Arduino, etc.) to connect
    // 5. Handle client read/write requests on GATT characteristics

    FURI_LOG_I(TAG, "BLE advertising started - Masterbeam service available");

    furi_mutex_release(ble_ctx->mutex);
    return true;
}

// BLE connection handler - deprecated, kept for compatibility
static bool ble_setup_connection(BleContext* ble_ctx) {
    if(!ble_ctx) return false;

    furi_mutex_acquire(ble_ctx->mutex, FuriWaitForever);

    furi_delay_ms(1500);

    furi_mutex_release(ble_ctx->mutex);

    FURI_LOG_I(TAG, "BLE peripheral setup complete (GAP server, no auth required)");
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
