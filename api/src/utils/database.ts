// Mock database implementation
// In production, replace with DynamoDB, RDS, or your preferred database

import { Device, Telemetry, OTAUpdate, DeviceStatus, OTAUpdateStatus } from '../types';

// In-memory storage (for demonstration purposes)
const devices = new Map<string, Device>();
const telemetryRecords = new Map<string, Telemetry>();
const otaUpdates = new Map<string, OTAUpdate>();

export const database = {
  // Device operations
  saveDevice: async (device: Device): Promise<Device> => {
    devices.set(device.deviceId, device);
    return device;
  },

  getDevice: async (deviceId: string): Promise<Device | null> => {
    return devices.get(deviceId) || null;
  },

  getAllDevices: async (): Promise<Device[]> => {
    return Array.from(devices.values());
  },

  updateDevice: async (deviceId: string, updates: Partial<Device>): Promise<Device | null> => {
    const device = devices.get(deviceId);
    if (!device) return null;
    
    const updatedDevice = { ...device, ...updates };
    devices.set(deviceId, updatedDevice);
    return updatedDevice;
  },

  deleteDevice: async (deviceId: string): Promise<boolean> => {
    return devices.delete(deviceId);
  },

  // Telemetry operations
  saveTelemetry: async (telemetry: Telemetry): Promise<Telemetry> => {
    telemetryRecords.set(telemetry.telemetryId, telemetry);
    
    // Update device last seen
    const device = devices.get(telemetry.deviceId);
    if (device) {
      device.lastSeen = telemetry.timestamp;
      device.status = DeviceStatus.ACTIVE;
      devices.set(device.deviceId, device);
    }
    
    return telemetry;
  },

  getTelemetry: async (telemetryId: string): Promise<Telemetry | null> => {
    return telemetryRecords.get(telemetryId) || null;
  },

  getTelemetryByDevice: async (deviceId: string): Promise<Telemetry[]> => {
    return Array.from(telemetryRecords.values())
      .filter(t => t.deviceId === deviceId)
      .sort((a, b) => new Date(b.timestamp).getTime() - new Date(a.timestamp).getTime());
  },

  // OTA Update operations
  saveOTAUpdate: async (update: OTAUpdate): Promise<OTAUpdate> => {
    otaUpdates.set(update.updateId, update);
    
    // Update device status
    const device = devices.get(update.deviceId);
    if (device) {
      device.status = DeviceStatus.UPDATING;
      devices.set(device.deviceId, device);
    }
    
    return update;
  },

  getOTAUpdate: async (updateId: string): Promise<OTAUpdate | null> => {
    return otaUpdates.get(updateId) || null;
  },

  getOTAUpdatesByDevice: async (deviceId: string): Promise<OTAUpdate[]> => {
    return Array.from(otaUpdates.values())
      .filter(u => u.deviceId === deviceId)
      .sort((a, b) => new Date(b.createdAt).getTime() - new Date(a.createdAt).getTime());
  },

  updateOTAUpdate: async (updateId: string, updates: Partial<OTAUpdate>): Promise<OTAUpdate | null> => {
    const update = otaUpdates.get(updateId);
    if (!update) return null;
    
    const updatedUpdate = { ...update, ...updates };
    otaUpdates.set(updateId, updatedUpdate);
    
    // Update device status and firmware version on completion
    if (updatedUpdate.status === OTAUpdateStatus.COMPLETED) {
      const device = devices.get(updatedUpdate.deviceId);
      if (device) {
        device.status = DeviceStatus.ACTIVE;
        device.firmwareVersion = updatedUpdate.toVersion;
        devices.set(device.deviceId, device);
      }
    } else if (updatedUpdate.status === OTAUpdateStatus.FAILED) {
      const device = devices.get(updatedUpdate.deviceId);
      if (device) {
        device.status = DeviceStatus.ERROR;
        devices.set(device.deviceId, device);
      }
    }
    
    return updatedUpdate;
  }
};
