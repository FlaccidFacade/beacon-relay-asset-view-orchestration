import * as cdk from "aws-cdk-lib";
import { Construct } from "constructs";
import * as dynamodb from "aws-cdk-lib/aws-dynamodb";

export interface DynamoDBStackProps extends cdk.StackProps {
  projectName: string;
  stage: string;
}

export class DynamoDBStack extends cdk.Stack {
  public readonly deviceTable: dynamodb.Table;
  public readonly telemetryTable: dynamodb.Table;

  constructor(scope: Construct, id: string, props: DynamoDBStackProps) {
    super(scope, id, props);

    // Device Table - stores device information and metadata
    this.deviceTable = new dynamodb.Table(this, "DeviceTable", {
      tableName: `${props.projectName}-Devices-${props.stage}`,
      partitionKey: {
        name: "deviceId",
        type: dynamodb.AttributeType.STRING,
      },
      sortKey: {
        name: "timestamp",
        type: dynamodb.AttributeType.NUMBER,
      },
      billingMode: dynamodb.BillingMode.PAY_PER_REQUEST,
      removalPolicy: cdk.RemovalPolicy.RETAIN,
      pointInTimeRecovery: true,
      encryption: dynamodb.TableEncryption.AWS_MANAGED,
      stream: dynamodb.StreamViewType.NEW_AND_OLD_IMAGES,
    });

    // Add GSI for querying by status
    this.deviceTable.addGlobalSecondaryIndex({
      indexName: "StatusIndex",
      partitionKey: {
        name: "status",
        type: dynamodb.AttributeType.STRING,
      },
      sortKey: {
        name: "timestamp",
        type: dynamodb.AttributeType.NUMBER,
      },
      projectionType: dynamodb.ProjectionType.ALL,
    });

    // Telemetry Table - stores device telemetry data
    this.telemetryTable = new dynamodb.Table(this, "TelemetryTable", {
      tableName: `${props.projectName}-Telemetry-${props.stage}`,
      partitionKey: {
        name: "deviceId",
        type: dynamodb.AttributeType.STRING,
      },
      sortKey: {
        name: "timestamp",
        type: dynamodb.AttributeType.NUMBER,
      },
      billingMode: dynamodb.BillingMode.PAY_PER_REQUEST,
      removalPolicy: cdk.RemovalPolicy.RETAIN,
      pointInTimeRecovery: true,
      encryption: dynamodb.TableEncryption.AWS_MANAGED,
      timeToLiveAttribute: "ttl",
    });

    // Add cost allocation tags
    cdk.Tags.of(this.deviceTable).add("CostCenter", "BRAVO-Storage");
    cdk.Tags.of(this.deviceTable).add("Service", "DynamoDB-Devices");
    cdk.Tags.of(this.telemetryTable).add("CostCenter", "BRAVO-Storage");
    cdk.Tags.of(this.telemetryTable).add("Service", "DynamoDB-Telemetry");

    // Add GSI for querying by metric type
    this.telemetryTable.addGlobalSecondaryIndex({
      indexName: "MetricTypeIndex",
      partitionKey: {
        name: "metricType",
        type: dynamodb.AttributeType.STRING,
      },
      sortKey: {
        name: "timestamp",
        type: dynamodb.AttributeType.NUMBER,
      },
      projectionType: dynamodb.ProjectionType.ALL,
    });

    // Output values
    new cdk.CfnOutput(this, "DeviceTableName", {
      value: this.deviceTable.tableName,
      description: "Device DynamoDB Table Name",
      exportName: `${props.projectName}-DeviceTableName-${props.stage}`,
    });

    new cdk.CfnOutput(this, "DeviceTableArn", {
      value: this.deviceTable.tableArn,
      description: "Device DynamoDB Table ARN",
      exportName: `${props.projectName}-DeviceTableArn-${props.stage}`,
    });

    new cdk.CfnOutput(this, "TelemetryTableName", {
      value: this.telemetryTable.tableName,
      description: "Telemetry DynamoDB Table Name",
      exportName: `${props.projectName}-TelemetryTableName-${props.stage}`,
    });

    new cdk.CfnOutput(this, "TelemetryTableArn", {
      value: this.telemetryTable.tableArn,
      description: "Telemetry DynamoDB Table ARN",
      exportName: `${props.projectName}-TelemetryTableArn-${props.stage}`,
    });
  }
}
