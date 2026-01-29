export const typeDefs = `#graphql
  enum DeviceStatus {
    ACTIVE
    INACTIVE
    UPDATING
    ERROR
  }

  enum OTAUpdateStatus {
    PENDING
    IN_PROGRESS
    COMPLETED
    FAILED
    CANCELLED
  }

  type Device {
    deviceId: ID!
    name: String!
    type: String!
    firmwareVersion: String!
    registeredAt: String!
    lastSeen: String
    status: DeviceStatus!
  }

  type Telemetry {
    telemetryId: ID!
    deviceId: String!
    timestamp: String!
    data: TelemetryData!
  }

  type TelemetryData {
    temperature: Float
    humidity: Float
    signal_strength: Float
    battery_level: Float
    custom: String
  }

  type OTAUpdate {
    updateId: ID!
    deviceId: String!
    fromVersion: String!
    toVersion: String!
    status: OTAUpdateStatus!
    downloadUrl: String!
    createdAt: String!
    startedAt: String
    completedAt: String
  }

  input DeviceRegistrationInput {
    name: String!
    type: String!
    firmwareVersion: String!
  }

  input TelemetryDataInput {
    temperature: Float
    humidity: Float
    signal_strength: Float
    battery_level: Float
    custom: String
  }

  input TelemetryInput {
    deviceId: String!
    data: TelemetryDataInput!
  }

  input OTAUpdateInput {
    deviceId: String!
    toVersion: String!
    downloadUrl: String!
  }

  type Query {
    device(deviceId: ID!): Device
    devices: [Device!]!
    telemetry(telemetryId: ID!): Telemetry
    deviceTelemetry(deviceId: ID!): [Telemetry!]!
    otaUpdate(updateId: ID!): OTAUpdate
    deviceOTAUpdates(deviceId: ID!): [OTAUpdate!]!
  }

  type Mutation {
    registerDevice(input: DeviceRegistrationInput!): Device!
    deleteDevice(deviceId: ID!): Boolean!
    submitTelemetry(input: TelemetryInput!): Telemetry!
    createOTAUpdate(input: OTAUpdateInput!): OTAUpdate!
    updateOTAUpdateStatus(updateId: ID!, status: OTAUpdateStatus!): OTAUpdate!
  }
`;
