package com.bravo.mobile.services;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;
import android.util.Log;

/**
 * Broadcast receiver for USB device attachment and detachment events
 * Handles automatic connection when ESP32 collar/dongle is plugged in
 */
public class UsbBroadcastReceiver extends BroadcastReceiver {
    private static final String TAG = "UsbBroadcastReceiver";
    
    public interface UsbEventListener {
        void onUsbDeviceAttached(UsbDevice device);
        void onUsbDeviceDetached(UsbDevice device);
    }
    
    private UsbEventListener listener;

    public UsbBroadcastReceiver() {
    }

    public UsbBroadcastReceiver(UsbEventListener listener) {
        this.listener = listener;
    }

    public void setListener(UsbEventListener listener) {
        this.listener = listener;
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        String action = intent.getAction();
        
        if (UsbManager.ACTION_USB_DEVICE_ATTACHED.equals(action)) {
            UsbDevice device = intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
            if (device != null) {
                Log.d(TAG, "USB Device attached: " + device.getDeviceName());
                if (listener != null) {
                    listener.onUsbDeviceAttached(device);
                }
            }
        } else if (UsbManager.ACTION_USB_DEVICE_DETACHED.equals(action)) {
            UsbDevice device = intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
            if (device != null) {
                Log.d(TAG, "USB Device detached: " + device.getDeviceName());
                if (listener != null) {
                    listener.onUsbDeviceDetached(device);
                }
            }
        }
    }
}
