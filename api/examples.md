# B.R.A.V.O API Usage Examples

This document provides practical examples for using the B.R.A.V.O API.

## REST API Examples

### 1. Complete Device Lifecycle

#### Step 1: Register a Device
```bash
curl -X POST https://your-api-gateway-url/devices \
  -H "Content-Type: application/json" \
  -d '{
    "name": "Temperature Sensor - Building A",
    "type": "temperature_sensor",
    "firmwareVersion": "1.0.0"
  }'
```

**Response:**
```json
{
  "success": true,
  "data": {
    "deviceId": "550e8400-e29b-41d4-a716-446655440000",
    "name": "Temperature Sensor - Building A",
    "type": "temperature_sensor",
    "firmwareVersion": "1.0.0",
    "registeredAt": "2025-10-21T12:00:00.000Z",
    "status": "ACTIVE"
  }
}
```

#### Step 2: Submit Telemetry Data
```bash
curl -X POST https://your-api-gateway-url/telemetry \
  -H "Content-Type: application/json" \
  -d '{
    "deviceId": "550e8400-e29b-41d4-a716-446655440000",
    "data": {
      "temperature": 23.5,
      "humidity": 65.2,
      "signal_strength": -45,
      "battery_level": 87
    }
  }'
```

#### Step 3: Retrieve Device Telemetry History
```bash
curl https://your-api-gateway-url/devices/550e8400-e29b-41d4-a716-446655440000/telemetry
```

#### Step 4: Create an OTA Update
```bash
curl -X POST https://your-api-gateway-url/ota-updates \
  -H "Content-Type: application/json" \
  -d '{
    "deviceId": "550e8400-e29b-41d4-a716-446655440000",
    "toVersion": "1.1.0",
    "downloadUrl": "https://firmware-bucket.s3.amazonaws.com/sensor-v1.1.0.bin"
  }'
```

**Response:**
```json
{
  "success": true,
  "data": {
    "updateId": "660e8400-e29b-41d4-a716-446655440001",
    "deviceId": "550e8400-e29b-41d4-a716-446655440000",
    "fromVersion": "1.0.0",
    "toVersion": "1.1.0",
    "status": "PENDING",
    "downloadUrl": "https://firmware-bucket.s3.amazonaws.com/sensor-v1.1.0.bin",
    "createdAt": "2025-10-21T12:05:00.000Z"
  }
}
```

#### Step 5: Update OTA Status (simulating device progress)
```bash
# Mark as in progress
curl -X PATCH https://your-api-gateway-url/ota-updates/660e8400-e29b-41d4-a716-446655440001/status \
  -H "Content-Type: application/json" \
  -d '{"status": "IN_PROGRESS"}'

# Mark as completed
curl -X PATCH https://your-api-gateway-url/ota-updates/660e8400-e29b-41d4-a716-446655440001/status \
  -H "Content-Type: application/json" \
  -d '{"status": "COMPLETED"}'
```

### 2. Multi-Device Monitoring

#### Get All Devices
```bash
curl https://your-api-gateway-url/devices
```

**Response:**
```json
{
  "success": true,
  "data": [
    {
      "deviceId": "550e8400-e29b-41d4-a716-446655440000",
      "name": "Temperature Sensor - Building A",
      "type": "temperature_sensor",
      "firmwareVersion": "1.1.0",
      "registeredAt": "2025-10-21T12:00:00.000Z",
      "lastSeen": "2025-10-21T12:30:00.000Z",
      "status": "ACTIVE"
    },
    {
      "deviceId": "550e8400-e29b-41d4-a716-446655440002",
      "name": "Humidity Sensor - Building B",
      "type": "humidity_sensor",
      "firmwareVersion": "1.0.0",
      "registeredAt": "2025-10-21T11:00:00.000Z",
      "lastSeen": "2025-10-21T12:29:00.000Z",
      "status": "ACTIVE"
    }
  ]
}
```

## GraphQL Examples

### 1. Using GraphQL Playground

The GraphQL endpoint includes an interactive playground when accessed via browser:
```
https://your-api-gateway-url/graphql
```

### 2. Complete Device Lifecycle with GraphQL

#### Register Multiple Devices
```graphql
mutation RegisterMultipleDevices {
  sensor1: registerDevice(input: {
    name: "Temperature Sensor - Floor 1"
    type: "temperature_sensor"
    firmwareVersion: "1.0.0"
  }) {
    deviceId
    name
    status
  }
  
  sensor2: registerDevice(input: {
    name: "Humidity Sensor - Floor 2"
    type: "humidity_sensor"
    firmwareVersion: "1.0.0"
  }) {
    deviceId
    name
    status
  }
}
```

#### Query Devices with Telemetry
```graphql
query DevicesWithRecentTelemetry {
  devices {
    deviceId
    name
    type
    firmwareVersion
    status
    lastSeen
  }
}
```

#### Submit Telemetry
```graphql
mutation SubmitSensorData {
  submitTelemetry(input: {
    deviceId: "550e8400-e29b-41d4-a716-446655440000"
    data: {
      temperature: 24.5
      humidity: 62.0
      signal_strength: -42
      battery_level: 85
    }
  }) {
    telemetryId
    timestamp
    data {
      temperature
      humidity
      signal_strength
      battery_level
    }
  }
}
```

#### Get Device with Telemetry History
```graphql
query DeviceWithHistory($deviceId: ID!) {
  device(deviceId: $deviceId) {
    deviceId
    name
    type
    firmwareVersion
    status
  }
  
  deviceTelemetry(deviceId: $deviceId) {
    telemetryId
    timestamp
    data {
      temperature
      humidity
      signal_strength
      battery_level
    }
  }
}
```

**Variables:**
```json
{
  "deviceId": "550e8400-e29b-41d4-a716-446655440000"
}
```

#### Create and Monitor OTA Update
```graphql
mutation CreateAndMonitorOTA {
  createOTAUpdate(input: {
    deviceId: "550e8400-e29b-41d4-a716-446655440000"
    toVersion: "2.0.0"
    downloadUrl: "https://firmware-bucket.s3.amazonaws.com/sensor-v2.0.0.bin"
  }) {
    updateId
    deviceId
    fromVersion
    toVersion
    status
    downloadUrl
    createdAt
  }
}
```

#### Update OTA Status
```graphql
mutation UpdateOTAProgress {
  updateOTAUpdateStatus(
    updateId: "660e8400-e29b-41d4-a716-446655440001"
    status: IN_PROGRESS
  ) {
    updateId
    status
    startedAt
  }
}
```

#### Query All OTA Updates for Device
```graphql
query DeviceOTAHistory {
  deviceOTAUpdates(deviceId: "550e8400-e29b-41d4-a716-446655440000") {
    updateId
    fromVersion
    toVersion
    status
    createdAt
    startedAt
    completedAt
  }
}
```

## JavaScript/Node.js Client Examples

### REST API with Fetch

```javascript
// Register a device
async function registerDevice() {
  const response = await fetch('https://your-api-gateway-url/devices', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
    },
    body: JSON.stringify({
      name: 'Smart Thermostat',
      type: 'thermostat',
      firmwareVersion: '1.0.0'
    })
  });
  
  const result = await response.json();
  return result.data;
}

// Submit telemetry
async function submitTelemetry(deviceId, data) {
  const response = await fetch('https://your-api-gateway-url/telemetry', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
    },
    body: JSON.stringify({
      deviceId,
      data
    })
  });
  
  return await response.json();
}

// Usage
const device = await registerDevice();
await submitTelemetry(device.deviceId, {
  temperature: 22.0,
  humidity: 45.0,
  battery_level: 92
});
```

### GraphQL Client

```javascript
// Using fetch
async function graphqlRequest(query, variables = {}) {
  const response = await fetch('https://your-api-gateway-url/graphql', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
    },
    body: JSON.stringify({
      query,
      variables
    })
  });
  
  const result = await response.json();
  return result.data;
}

// Register device
const registerMutation = `
  mutation RegisterDevice($input: DeviceRegistrationInput!) {
    registerDevice(input: $input) {
      deviceId
      name
      status
    }
  }
`;

const device = await graphqlRequest(registerMutation, {
  input: {
    name: 'Smart Light',
    type: 'light',
    firmwareVersion: '1.0.0'
  }
});

// Query devices
const devicesQuery = `
  query {
    devices {
      deviceId
      name
      type
      status
    }
  }
`;

const devices = await graphqlRequest(devicesQuery);
```

## Python Examples

### REST API with requests

```python
import requests
import json

API_BASE = 'https://your-api-gateway-url'

# Register a device
def register_device(name, device_type, firmware_version):
    response = requests.post(
        f'{API_BASE}/devices',
        json={
            'name': name,
            'type': device_type,
            'firmwareVersion': firmware_version
        }
    )
    return response.json()['data']

# Submit telemetry
def submit_telemetry(device_id, data):
    response = requests.post(
        f'{API_BASE}/telemetry',
        json={
            'deviceId': device_id,
            'data': data
        }
    )
    return response.json()['data']

# Usage
device = register_device('Motion Sensor', 'motion_sensor', '1.0.0')
telemetry = submit_telemetry(device['deviceId'], {
    'motion_detected': True,
    'battery_level': 78,
    'signal_strength': -38
})
```

### GraphQL with requests

```python
import requests

GRAPHQL_URL = 'https://your-api-gateway-url/graphql'

def graphql_request(query, variables=None):
    response = requests.post(
        GRAPHQL_URL,
        json={
            'query': query,
            'variables': variables or {}
        }
    )
    return response.json()['data']

# Register device
register_mutation = """
mutation RegisterDevice($input: DeviceRegistrationInput!) {
  registerDevice(input: $input) {
    deviceId
    name
    status
  }
}
"""

device = graphql_request(register_mutation, {
    'input': {
        'name': 'Door Sensor',
        'type': 'door_sensor',
        'firmwareVersion': '1.0.0'
    }
})

# Get all devices
devices_query = """
query {
  devices {
    deviceId
    name
    type
    status
  }
}
"""

devices = graphql_request(devices_query)
```

## Advanced Use Cases

### Batch Telemetry Upload

```bash
# Submit multiple telemetry records in sequence
for i in {1..10}; do
  curl -X POST https://your-api-gateway-url/telemetry \
    -H "Content-Type: application/json" \
    -d "{
      \"deviceId\": \"550e8400-e29b-41d4-a716-446655440000\",
      \"data\": {
        \"temperature\": $(awk -v min=20 -v max=30 'BEGIN{srand(); print min+rand()*(max-min)}'),
        \"timestamp\": \"$(date -u +%Y-%m-%dT%H:%M:%S.000Z)\"
      }
    }"
  sleep 1
done
```

### Fleet Firmware Update

```bash
# Get all devices
DEVICES=$(curl -s https://your-api-gateway-url/devices | jq -r '.data[].deviceId')

# Create OTA update for each device
for DEVICE_ID in $DEVICES; do
  curl -X POST https://your-api-gateway-url/ota-updates \
    -H "Content-Type: application/json" \
    -d "{
      \"deviceId\": \"$DEVICE_ID\",
      \"toVersion\": \"2.0.0\",
      \"downloadUrl\": \"https://firmware-bucket.s3.amazonaws.com/firmware-v2.0.0.bin\"
    }"
done
```

## Error Handling Examples

### Handling Device Not Found

```javascript
async function getDevice(deviceId) {
  try {
    const response = await fetch(
      `https://your-api-gateway-url/devices/${deviceId}`
    );
    
    const result = await response.json();
    
    if (!result.success) {
      throw new Error(result.error);
    }
    
    return result.data;
  } catch (error) {
    console.error('Failed to get device:', error.message);
    return null;
  }
}
```

### Handling Validation Errors

```javascript
async function registerDevice(data) {
  const response = await fetch('https://your-api-gateway-url/devices', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
    },
    body: JSON.stringify(data)
  });
  
  const result = await response.json();
  
  if (response.status === 400) {
    console.error('Validation error:', result.error);
    return null;
  }
  
  return result.data;
}
```

## Monitoring and Debugging

### Check Device Status
```bash
# Get device details
curl https://your-api-gateway-url/devices/550e8400-e29b-41d4-a716-446655440000

# Check recent telemetry
curl https://your-api-gateway-url/devices/550e8400-e29b-41d4-a716-446655440000/telemetry

# Check OTA updates
curl https://your-api-gateway-url/devices/550e8400-e29b-41d4-a716-446655440000/ota-updates
```

### GraphQL Introspection
```graphql
query IntrospectionQuery {
  __schema {
    types {
      name
      kind
      description
    }
  }
}
```
