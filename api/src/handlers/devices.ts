import { APIGatewayProxyEvent, APIGatewayProxyResult } from 'aws-lambda';
import { v4 as uuidv4 } from 'uuid';
import { Device, DeviceStatus, DeviceRegistrationInput } from '../types';
import { successResponse, errorResponse } from '../utils/response';
import { validateDeviceRegistration } from '../utils/validator';
import { database } from '../utils/database';

/**
 * Register a new device
 */
export async function registerDevice(event: APIGatewayProxyEvent): Promise<APIGatewayProxyResult> {
  try {
    if (!event.body) {
      return errorResponse('Request body is required', 400);
    }

    const input = JSON.parse(event.body);

    if (!validateDeviceRegistration(input)) {
      return errorResponse('Invalid device registration data', 400);
    }

    const deviceInput = input as DeviceRegistrationInput;
    const device: Device = {
      deviceId: uuidv4(),
      name: deviceInput.name,
      type: deviceInput.type,
      firmwareVersion: deviceInput.firmwareVersion,
      registeredAt: new Date().toISOString(),
      status: DeviceStatus.ACTIVE
    };

    const savedDevice = await database.saveDevice(device);
    return successResponse(savedDevice, 201);
  } catch (error) {
    console.error('Error registering device:', error);
    return errorResponse('Internal server error', 500);
  }
}

/**
 * Get a device by ID
 */
export async function getDevice(event: APIGatewayProxyEvent): Promise<APIGatewayProxyResult> {
  try {
    const deviceId = event.pathParameters?.deviceId;

    if (!deviceId) {
      return errorResponse('Device ID is required', 400);
    }

    const device = await database.getDevice(deviceId);

    if (!device) {
      return errorResponse('Device not found', 404);
    }

    return successResponse(device);
  } catch (error) {
    console.error('Error getting device:', error);
    return errorResponse('Internal server error', 500);
  }
}

/**
 * Get all devices
 */
export async function listDevices(): Promise<APIGatewayProxyResult> {
  try {
    const devices = await database.getAllDevices();
    return successResponse(devices);
  } catch (error) {
    console.error('Error listing devices:', error);
    return errorResponse('Internal server error', 500);
  }
}

/**
 * Delete a device
 */
export async function deleteDevice(event: APIGatewayProxyEvent): Promise<APIGatewayProxyResult> {
  try {
    const deviceId = event.pathParameters?.deviceId;

    if (!deviceId) {
      return errorResponse('Device ID is required', 400);
    }

    const deleted = await database.deleteDevice(deviceId);

    if (!deleted) {
      return errorResponse('Device not found', 404);
    }

    return successResponse({ message: 'Device deleted successfully' });
  } catch (error) {
    console.error('Error deleting device:', error);
    return errorResponse('Internal server error', 500);
  }
}
