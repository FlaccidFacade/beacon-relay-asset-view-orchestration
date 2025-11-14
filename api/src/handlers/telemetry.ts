import { APIGatewayProxyEvent, APIGatewayProxyResult } from 'aws-lambda';
import { v4 as uuidv4 } from 'uuid';
import { Telemetry, TelemetryInput } from '../types';
import { successResponse, errorResponse } from '../utils/response';
import { validateTelemetry } from '../utils/validator';
import { database } from '../utils/database';

/**
 * Submit telemetry data
 */
export async function submitTelemetry(event: APIGatewayProxyEvent): Promise<APIGatewayProxyResult> {
  try {
    if (!event.body) {
      return errorResponse('Request body is required', 400);
    }

    const input = JSON.parse(event.body);

    if (!validateTelemetry(input)) {
      return errorResponse('Invalid telemetry data', 400);
    }

    const telemetryInput = input as TelemetryInput;

    // Check if device exists
    const device = await database.getDevice(telemetryInput.deviceId);
    if (!device) {
      return errorResponse('Device not found', 404);
    }

    const telemetry: Telemetry = {
      telemetryId: uuidv4(),
      deviceId: telemetryInput.deviceId,
      timestamp: new Date().toISOString(),
      data: telemetryInput.data
    };

    const savedTelemetry = await database.saveTelemetry(telemetry);
    return successResponse(savedTelemetry, 201);
  } catch (error) {
    console.error('Error submitting telemetry:', error);
    return errorResponse('Internal server error', 500);
  }
}

/**
 * Get telemetry by ID
 */
export async function getTelemetry(event: APIGatewayProxyEvent): Promise<APIGatewayProxyResult> {
  try {
    const telemetryId = event.pathParameters?.telemetryId;

    if (!telemetryId) {
      return errorResponse('Telemetry ID is required', 400);
    }

    const telemetry = await database.getTelemetry(telemetryId);

    if (!telemetry) {
      return errorResponse('Telemetry not found', 404);
    }

    return successResponse(telemetry);
  } catch (error) {
    console.error('Error getting telemetry:', error);
    return errorResponse('Internal server error', 500);
  }
}

/**
 * Get telemetry for a device
 */
export async function getDeviceTelemetry(event: APIGatewayProxyEvent): Promise<APIGatewayProxyResult> {
  try {
    const deviceId = event.pathParameters?.deviceId;

    if (!deviceId) {
      return errorResponse('Device ID is required', 400);
    }

    const device = await database.getDevice(deviceId);
    if (!device) {
      return errorResponse('Device not found', 404);
    }

    const telemetryRecords = await database.getTelemetryByDevice(deviceId);
    return successResponse(telemetryRecords);
  } catch (error) {
    console.error('Error getting device telemetry:', error);
    return errorResponse('Internal server error', 500);
  }
}
