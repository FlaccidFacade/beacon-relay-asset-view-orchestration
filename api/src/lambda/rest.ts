import { APIGatewayProxyEvent, APIGatewayProxyResult } from 'aws-lambda';
import { registerDevice, getDevice, listDevices, deleteDevice } from '../handlers/devices';
import { submitTelemetry, getTelemetry, getDeviceTelemetry } from '../handlers/telemetry';
import { createOTAUpdate, getOTAUpdate, getDeviceOTAUpdates, updateOTAUpdateStatus } from '../handlers/ota';
import { errorResponse } from '../utils/response';

/**
 * Main REST API Lambda handler
 * Routes requests to appropriate handlers based on path and method
 */
export async function handler(event: APIGatewayProxyEvent): Promise<APIGatewayProxyResult> {
  console.log('Event:', JSON.stringify(event, null, 2));

  const path = event.path;
  const method = event.httpMethod;

  try {
    // Device endpoints
    if (path === '/devices' && method === 'POST') {
      return await registerDevice(event);
    }
    
    if (path === '/devices' && method === 'GET') {
      return await listDevices();
    }
    
    if (path.startsWith('/devices/') && method === 'GET') {
      return await getDevice(event);
    }
    
    if (path.startsWith('/devices/') && method === 'DELETE') {
      return await deleteDevice(event);
    }

    // Telemetry endpoints
    if (path === '/telemetry' && method === 'POST') {
      return await submitTelemetry(event);
    }
    
    if (path.startsWith('/telemetry/') && !path.includes('/devices/') && method === 'GET') {
      return await getTelemetry(event);
    }
    
    if (path.match(/\/devices\/[^/]+\/telemetry$/) && method === 'GET') {
      return await getDeviceTelemetry(event);
    }

    // OTA Update endpoints
    if (path === '/ota-updates' && method === 'POST') {
      return await createOTAUpdate(event);
    }
    
    if (path.startsWith('/ota-updates/') && !path.includes('/devices/') && method === 'GET') {
      return await getOTAUpdate(event);
    }
    
    if (path.match(/\/ota-updates\/[^/]+\/status$/) && method === 'PATCH') {
      return await updateOTAUpdateStatus(event);
    }
    
    if (path.match(/\/devices\/[^/]+\/ota-updates$/) && method === 'GET') {
      return await getDeviceOTAUpdates(event);
    }

    // No matching route found
    return errorResponse(`Route not found: ${method} ${path}`, 404);
  } catch (error) {
    console.error('Unhandled error:', error);
    return errorResponse('Internal server error', 500);
  }
}
