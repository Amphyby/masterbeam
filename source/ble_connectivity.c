#include <furi.h>
#include <notification/notification_messages.h>

#include "../include/masterbeam.h"

#define TAG "Masterbeam_BLE"

// Simulated BLE connection handler
static bool ble_connect_to_arduino(BleContext* ble_ctx) {
    if(!ble_ctx) return false;

    furi_mutex_acquire(ble_ctx->mutex, FuriWaitForever);

    // Simulate connection delay
    furi_delay_ms(1500);

    ble_ctx->is_connected = true;

    furi_mutex_release(ble_ctx->mutex);

    FURI_LOG_I(TAG, "BLE connection established");
    return true;
}

static int32_t ble_connection_thread(void* ctx) {
    BleContext* ble_ctx = (BleContext*)ctx;
    ble_connect_to_arduino(ble_ctx);
    return 0;
}

BleContext* ble_context_alloc(void) {
    BleContext* ble_ctx = malloc(sizeof(BleContext));
    ble_ctx->is_connected = false;
    ble_ctx->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    ble_ctx->thread = NULL;
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

bool ble_is_connected(BleContext* ble_ctx) {
    if(!ble_ctx) return false;

    furi_mutex_acquire(ble_ctx->mutex, FuriWaitForever);
    bool connected = ble_ctx->is_connected;
    furi_mutex_release(ble_ctx->mutex);

    return connected;
}

void ble_send_command(BleContext* ble_ctx, const char* command) {
    if(!ble_ctx || !ble_is_connected(ble_ctx)) return;

    FURI_LOG_I(TAG, "Sending command: %s", command);
    // Command sending implementation would go here
}
