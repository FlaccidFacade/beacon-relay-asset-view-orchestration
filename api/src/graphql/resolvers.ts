import { v4 as uuidv4 } from 'uuid';
import { Device, Telemetry, OTAUpdate, DeviceStatus, OTAUpdateStatus } from '../types';
import { database } from '../utils/database';

export const resolvers = {
  Query: {
    device: async (_: unknown, { deviceId }: { deviceId: string }): Promise<Device | null> => {
      return await database.getDevice(deviceId);
    },

    devices: async (): Promise<Device[]> => {
      return await database.getAllDevices();
    },

    telemetry: async (_: unknown, { telemetryId }: { telemetryId: string }): Promise<Telemetry | null> => {
      return await database.getTelemetry(telemetryId);
    },

    deviceTelemetry: async (_: unknown, { deviceId }: { deviceId: string }): Promise<Telemetry[]> => {
      return await database.getTelemetryByDevice(deviceId);
    },

    otaUpdate: async (_: unknown, { updateId }: { updateId: string }): Promise<OTAUpdate | null> => {
      return await database.getOTAUpdate(updateId);
    },

    deviceOTAUpdates: async (_: unknown, { deviceId }: { deviceId: string }): Promise<OTAUpdate[]> => {
      return await database.getOTAUpdatesByDevice(deviceId);
    },
  },

  Mutation: {
    registerDevice: async (_: unknown, { input }: { input: { name: string; type: string; firmwareVersion: string } }): Promise<Device> => {
      const device: Device = {
        deviceId: uuidv4(),
        name: input.name,
        type: input.type,
        firmwareVersion: input.firmwareVersion,
        registeredAt: new Date().toISOString(),
        status: DeviceStatus.ACTIVE,
      };

      return await database.saveDevice(device);
    },

    deleteDevice: async (_: unknown, { deviceId }: { deviceId: string }): Promise<boolean> => {
      return await database.deleteDevice(deviceId);
    },

    submitTelemetry: async (_: unknown, { input }: { input: { deviceId: string; data: Record<string, number | string | boolean | undefined> } }): Promise<Telemetry> => {
      const device = await database.getDevice(input.deviceId);
      if (!device) {
        throw new Error('Device not found');
      }

      const telemetry: Telemetry = {
        telemetryId: uuidv4(),
        deviceId: input.deviceId,
        timestamp: new Date().toISOString(),
        data: input.data,
      };

      return await database.saveTelemetry(telemetry);
    },

    createOTAUpdate: async (_: unknown, { input }: { input: { deviceId: string; toVersion: string; downloadUrl: string } }): Promise<OTAUpdate> => {
      const device = await database.getDevice(input.deviceId);
      if (!device) {
        throw new Error('Device not found');
      }

      const otaUpdate: OTAUpdate = {
        updateId: uuidv4(),
        deviceId: input.deviceId,
        fromVersion: device.firmwareVersion,
        toVersion: input.toVersion,
        status: OTAUpdateStatus.PENDING,
        downloadUrl: input.downloadUrl,
        createdAt: new Date().toISOString(),
      };

      return await database.saveOTAUpdate(otaUpdate);
    },

    updateOTAUpdateStatus: async (_: unknown, { updateId, status }: { updateId: string; status: OTAUpdateStatus }): Promise<OTAUpdate> => {
      const updates: Partial<OTAUpdate> = { status };

      if (status === OTAUpdateStatus.IN_PROGRESS) {
        updates.startedAt = new Date().toISOString();
      } else if (
        status === OTAUpdateStatus.COMPLETED ||
        status === OTAUpdateStatus.FAILED ||
        status === OTAUpdateStatus.CANCELLED
      ) {
        updates.completedAt = new Date().toISOString();
      }

      const updatedUpdate = await database.updateOTAUpdate(updateId, updates);
      if (!updatedUpdate) {
        throw new Error('OTA update not found');
      }

      return updatedUpdate;
    },
  },
};
