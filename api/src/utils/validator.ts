import { DeviceRegistrationInput, TelemetryInput, OTAUpdateInput } from '../types';

export function validateDeviceRegistration(input: unknown): input is DeviceRegistrationInput {
  if (!input || typeof input !== 'object') {
    return false;
  }

  const data = input as Record<string, unknown>;
  
  return (
    typeof data.name === 'string' && data.name.length > 0 &&
    typeof data.type === 'string' && data.type.length > 0 &&
    typeof data.firmwareVersion === 'string' && data.firmwareVersion.length > 0
  );
}

export function validateTelemetry(input: unknown): input is TelemetryInput {
  if (!input || typeof input !== 'object') {
    return false;
  }

  const data = input as Record<string, unknown>;
  
  return (
    typeof data.deviceId === 'string' && data.deviceId.length > 0 &&
    typeof data.data === 'object' && data.data !== null
  );
}

export function validateOTAUpdate(input: unknown): input is OTAUpdateInput {
  if (!input || typeof input !== 'object') {
    return false;
  }

  const data = input as Record<string, unknown>;
  
  return (
    typeof data.deviceId === 'string' && data.deviceId.length > 0 &&
    typeof data.toVersion === 'string' && data.toVersion.length > 0 &&
    typeof data.downloadUrl === 'string' && data.downloadUrl.length > 0
  );
}
