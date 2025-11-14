// Device types
export interface Device {
  deviceId: string;
  name: string;
  type: string;
  firmwareVersion: string;
  registeredAt: string;
  lastSeen?: string;
  status: DeviceStatus;
}

export enum DeviceStatus {
  ACTIVE = 'ACTIVE',
  INACTIVE = 'INACTIVE',
  UPDATING = 'UPDATING',
  ERROR = 'ERROR'
}

export interface DeviceRegistrationInput {
  name: string;
  type: string;
  firmwareVersion: string;
}

// Telemetry types
export interface Telemetry {
  telemetryId: string;
  deviceId: string;
  timestamp: string;
  data: TelemetryData;
}

export interface TelemetryData {
  temperature?: number;
  humidity?: number;
  signal_strength?: number;
  battery_level?: number;
  [key: string]: number | string | boolean | undefined;
}

export interface TelemetryInput {
  deviceId: string;
  data: TelemetryData;
}

// OTA Update types
export interface OTAUpdate {
  updateId: string;
  deviceId: string;
  fromVersion: string;
  toVersion: string;
  status: OTAUpdateStatus;
  downloadUrl: string;
  createdAt: string;
  startedAt?: string;
  completedAt?: string;
}

export enum OTAUpdateStatus {
  PENDING = 'PENDING',
  IN_PROGRESS = 'IN_PROGRESS',
  COMPLETED = 'COMPLETED',
  FAILED = 'FAILED',
  CANCELLED = 'CANCELLED'
}

export interface OTAUpdateInput {
  deviceId: string;
  toVersion: string;
  downloadUrl: string;
}

// API Response types
export interface ApiResponse<T = unknown> {
  success: boolean;
  data?: T;
  error?: string;
  message?: string;
}
