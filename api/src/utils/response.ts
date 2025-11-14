import { APIGatewayProxyResult } from 'aws-lambda';
import { ApiResponse } from '../types';

export function successResponse<T>(data: T, statusCode = 200): APIGatewayProxyResult {
  const response: ApiResponse<T> = {
    success: true,
    data
  };

  return {
    statusCode,
    headers: {
      'Content-Type': 'application/json',
      'Access-Control-Allow-Origin': '*',
      'Access-Control-Allow-Credentials': true,
    },
    body: JSON.stringify(response),
  };
}

export function errorResponse(error: string, statusCode = 500): APIGatewayProxyResult {
  const response: ApiResponse = {
    success: false,
    error
  };

  return {
    statusCode,
    headers: {
      'Content-Type': 'application/json',
      'Access-Control-Allow-Origin': '*',
      'Access-Control-Allow-Credentials': true,
    },
    body: JSON.stringify(response),
  };
}
