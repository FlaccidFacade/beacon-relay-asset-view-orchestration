import { APIGatewayProxyEvent, APIGatewayProxyResult } from 'aws-lambda';
import { v4 as uuidv4 } from 'uuid';
import { OTAUpdate, OTAUpdateStatus, OTAUpdateInput } from '../types';
import { successResponse, errorResponse } from '../utils/response';
import { validateOTAUpdate } from '../utils/validator';
import { database } from '../utils/database';

/**
 * Create an OTA update
 */
export async function createOTAUpdate(event: APIGatewayProxyEvent): Promise<APIGatewayProxyResult> {
  try {
    if (!event.body) {
      return errorResponse('Request body is required', 400);
    }

    const input = JSON.parse(event.body);

    if (!validateOTAUpdate(input)) {
      return errorResponse('Invalid OTA update data', 400);
    }

    const otaInput = input as OTAUpdateInput;

    // Check if device exists
    const device = await database.getDevice(otaInput.deviceId);
    if (!device) {
      return errorResponse('Device not found', 404);
    }

    const otaUpdate: OTAUpdate = {
      updateId: uuidv4(),
      deviceId: otaInput.deviceId,
      fromVersion: device.firmwareVersion,
      toVersion: otaInput.toVersion,
      status: OTAUpdateStatus.PENDING,
      downloadUrl: otaInput.downloadUrl,
      createdAt: new Date().toISOString()
    };

    const savedUpdate = await database.saveOTAUpdate(otaUpdate);
    return successResponse(savedUpdate, 201);
  } catch (error) {
    console.error('Error creating OTA update:', error);
    return errorResponse('Internal server error', 500);
  }
}

/**
 * Get an OTA update by ID
 */
export async function getOTAUpdate(event: APIGatewayProxyEvent): Promise<APIGatewayProxyResult> {
  try {
    const updateId = event.pathParameters?.updateId;

    if (!updateId) {
      return errorResponse('Update ID is required', 400);
    }

    const update = await database.getOTAUpdate(updateId);

    if (!update) {
      return errorResponse('OTA update not found', 404);
    }

    return successResponse(update);
  } catch (error) {
    console.error('Error getting OTA update:', error);
    return errorResponse('Internal server error', 500);
  }
}

/**
 * Get OTA updates for a device
 */
export async function getDeviceOTAUpdates(event: APIGatewayProxyEvent): Promise<APIGatewayProxyResult> {
  try {
    const deviceId = event.pathParameters?.deviceId;

    if (!deviceId) {
      return errorResponse('Device ID is required', 400);
    }

    const device = await database.getDevice(deviceId);
    if (!device) {
      return errorResponse('Device not found', 404);
    }

    const updates = await database.getOTAUpdatesByDevice(deviceId);
    return successResponse(updates);
  } catch (error) {
    console.error('Error getting device OTA updates:', error);
    return errorResponse('Internal server error', 500);
  }
}

/**
 * Update OTA update status
 */
export async function updateOTAUpdateStatus(event: APIGatewayProxyEvent): Promise<APIGatewayProxyResult> {
  try {
    const updateId = event.pathParameters?.updateId;

    if (!updateId) {
      return errorResponse('Update ID is required', 400);
    }

    if (!event.body) {
      return errorResponse('Request body is required', 400);
    }

    const { status } = JSON.parse(event.body);

    if (!Object.values(OTAUpdateStatus).includes(status)) {
      return errorResponse('Invalid status value', 400);
    }

    const updates: Partial<OTAUpdate> = { status };

    if (status === OTAUpdateStatus.IN_PROGRESS) {
      updates.startedAt = new Date().toISOString();
    } else if (status === OTAUpdateStatus.COMPLETED || status === OTAUpdateStatus.FAILED || status === OTAUpdateStatus.CANCELLED) {
      updates.completedAt = new Date().toISOString();
    }

    const updatedUpdate = await database.updateOTAUpdate(updateId, updates);

    if (!updatedUpdate) {
      return errorResponse('OTA update not found', 404);
    }

    return successResponse(updatedUpdate);
  } catch (error) {
    console.error('Error updating OTA update status:', error);
    return errorResponse('Internal server error', 500);
  }
}
