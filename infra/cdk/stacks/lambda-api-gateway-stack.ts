import * as cdk from 'aws-cdk-lib';
import { Construct } from 'constructs';
import * as lambda from 'aws-cdk-lib/aws-lambda';
import * as apigateway from 'aws-cdk-lib/aws-apigateway';
import * as dynamodb from 'aws-cdk-lib/aws-dynamodb';
import * as iam from 'aws-cdk-lib/aws-iam';
import * as logs from 'aws-cdk-lib/aws-logs';

export interface LambdaApiGatewayStackProps extends cdk.StackProps {
  projectName: string;
  stage: string;
  deviceTable: dynamodb.Table;
  telemetryTable: dynamodb.Table;
  iotEndpoint: string;
}

export class LambdaApiGatewayStack extends cdk.Stack {
  public readonly api: apigateway.RestApi;

  constructor(scope: Construct, id: string, props: LambdaApiGatewayStackProps) {
    super(scope, id, props);

    // Lambda execution role with necessary permissions
    const lambdaRole = new iam.Role(this, 'LambdaExecutionRole', {
      assumedBy: new iam.ServicePrincipal('lambda.amazonaws.com'),
      managedPolicies: [
        iam.ManagedPolicy.fromAwsManagedPolicyName('service-role/AWSLambdaBasicExecutionRole'),
      ],
    });

    // Grant Lambda access to DynamoDB tables
    props.deviceTable.grantReadWriteData(lambdaRole);
    props.telemetryTable.grantReadWriteData(lambdaRole);

    // Grant Lambda access to IoT
    lambdaRole.addToPolicy(new iam.PolicyStatement({
      actions: [
        'iot:DescribeThing',
        'iot:ListThings',
        'iot:UpdateThing',
        'iot:CreateThing',
        'iot:DeleteThing',
      ],
      resources: ['*'],
    }));

    // Device Management Lambda Function
    const deviceLambda = new lambda.Function(this, 'DeviceFunction', {
      functionName: `${props.projectName}-DeviceFunction-${props.stage}`,
      runtime: lambda.Runtime.NODEJS_20_X,
      handler: 'index.handler',
      code: lambda.Code.fromInline(`
        const { DynamoDBClient, PutItemCommand, GetItemCommand, QueryCommand, DeleteItemCommand } = require("@aws-sdk/client-dynamodb");
        const { marshall, unmarshall } = require("@aws-sdk/util-dynamodb");
        
        const dynamodb = new DynamoDBClient({});
        const DEVICE_TABLE = process.env.DEVICE_TABLE;
        
        exports.handler = async (event) => {
          console.log('Event:', JSON.stringify(event, null, 2));
          
          const httpMethod = event.httpMethod;
          const path = event.path;
          
          try {
            if (httpMethod === 'GET' && path === '/devices') {
              // List all devices
              const params = {
                TableName: DEVICE_TABLE,
              };
              const command = new QueryCommand(params);
              const result = await dynamodb.send(command);
              
              return {
                statusCode: 200,
                headers: {
                  'Content-Type': 'application/json',
                  'Access-Control-Allow-Origin': '*',
                },
                body: JSON.stringify({
                  devices: result.Items ? result.Items.map(item => unmarshall(item)) : [],
                }),
              };
            } else if (httpMethod === 'GET' && path.startsWith('/devices/')) {
              // Get specific device
              const deviceId = path.split('/')[2];
              
              const params = {
                TableName: DEVICE_TABLE,
                Key: marshall({ deviceId }),
              };
              const command = new GetItemCommand(params);
              const result = await dynamodb.send(command);
              
              if (!result.Item) {
                return {
                  statusCode: 404,
                  headers: {
                    'Content-Type': 'application/json',
                    'Access-Control-Allow-Origin': '*',
                  },
                  body: JSON.stringify({ error: 'Device not found' }),
                };
              }
              
              return {
                statusCode: 200,
                headers: {
                  'Content-Type': 'application/json',
                  'Access-Control-Allow-Origin': '*',
                },
                body: JSON.stringify(unmarshall(result.Item)),
              };
            } else if (httpMethod === 'POST' && path === '/devices') {
              // Create new device
              const body = JSON.parse(event.body || '{}');
              const device = {
                deviceId: body.deviceId || \`device-\${Date.now()}\`,
                timestamp: Date.now(),
                status: body.status || 'active',
                ...body,
              };
              
              const params = {
                TableName: DEVICE_TABLE,
                Item: marshall(device),
              };
              const command = new PutItemCommand(params);
              await dynamodb.send(command);
              
              return {
                statusCode: 201,
                headers: {
                  'Content-Type': 'application/json',
                  'Access-Control-Allow-Origin': '*',
                },
                body: JSON.stringify(device),
              };
            } else if (httpMethod === 'DELETE' && path.startsWith('/devices/')) {
              // Delete device
              const deviceId = path.split('/')[2];
              
              const params = {
                TableName: DEVICE_TABLE,
                Key: marshall({ deviceId }),
              };
              const command = new DeleteItemCommand(params);
              await dynamodb.send(command);
              
              return {
                statusCode: 204,
                headers: {
                  'Access-Control-Allow-Origin': '*',
                },
                body: '',
              };
            }
            
            return {
              statusCode: 400,
              headers: {
                'Content-Type': 'application/json',
                'Access-Control-Allow-Origin': '*',
              },
              body: JSON.stringify({ error: 'Unsupported operation' }),
            };
          } catch (error) {
            console.error('Error:', error);
            return {
              statusCode: 500,
              headers: {
                'Content-Type': 'application/json',
                'Access-Control-Allow-Origin': '*',
              },
              body: JSON.stringify({ error: 'Internal server error', message: error.message }),
            };
          }
        };
      `),
      environment: {
        DEVICE_TABLE: props.deviceTable.tableName,
        IOT_ENDPOINT: props.iotEndpoint,
      },
      role: lambdaRole,
      timeout: cdk.Duration.seconds(30),
      memorySize: 256,
      logRetention: logs.RetentionDays.ONE_WEEK,
    });

    // Telemetry Lambda Function
    const telemetryLambda = new lambda.Function(this, 'TelemetryFunction', {
      functionName: `${props.projectName}-TelemetryFunction-${props.stage}`,
      runtime: lambda.Runtime.NODEJS_20_X,
      handler: 'index.handler',
      code: lambda.Code.fromInline(`
        const { DynamoDBClient, PutItemCommand, QueryCommand } = require("@aws-sdk/client-dynamodb");
        const { marshall, unmarshall } = require("@aws-sdk/util-dynamodb");
        
        const dynamodb = new DynamoDBClient({});
        const TELEMETRY_TABLE = process.env.TELEMETRY_TABLE;
        
        exports.handler = async (event) => {
          console.log('Event:', JSON.stringify(event, null, 2));
          
          const httpMethod = event.httpMethod;
          const path = event.path;
          
          try {
            if (httpMethod === 'GET' && path.startsWith('/telemetry/')) {
              // Get telemetry for specific device
              const deviceId = path.split('/')[2];
              
              const params = {
                TableName: TELEMETRY_TABLE,
                KeyConditionExpression: 'deviceId = :deviceId',
                ExpressionAttributeValues: marshall({
                  ':deviceId': deviceId,
                }),
                ScanIndexForward: false,
                Limit: 100,
              };
              const command = new QueryCommand(params);
              const result = await dynamodb.send(command);
              
              return {
                statusCode: 200,
                headers: {
                  'Content-Type': 'application/json',
                  'Access-Control-Allow-Origin': '*',
                },
                body: JSON.stringify({
                  telemetry: result.Items ? result.Items.map(item => unmarshall(item)) : [],
                }),
              };
            } else if (httpMethod === 'POST' && path === '/telemetry') {
              // Add telemetry data
              const body = JSON.parse(event.body || '{}');
              const telemetry = {
                deviceId: body.deviceId,
                timestamp: Date.now(),
                metricType: body.metricType || 'default',
                ttl: Math.floor(Date.now() / 1000) + (30 * 24 * 60 * 60), // 30 days
                ...body,
              };
              
              const params = {
                TableName: TELEMETRY_TABLE,
                Item: marshall(telemetry),
              };
              const command = new PutItemCommand(params);
              await dynamodb.send(command);
              
              return {
                statusCode: 201,
                headers: {
                  'Content-Type': 'application/json',
                  'Access-Control-Allow-Origin': '*',
                },
                body: JSON.stringify(telemetry),
              };
            }
            
            return {
              statusCode: 400,
              headers: {
                'Content-Type': 'application/json',
                'Access-Control-Allow-Origin': '*',
              },
              body: JSON.stringify({ error: 'Unsupported operation' }),
            };
          } catch (error) {
            console.error('Error:', error);
            return {
              statusCode: 500,
              headers: {
                'Content-Type': 'application/json',
                'Access-Control-Allow-Origin': '*',
              },
              body: JSON.stringify({ error: 'Internal server error', message: error.message }),
            };
          }
        };
      `),
      environment: {
        TELEMETRY_TABLE: props.telemetryTable.tableName,
      },
      role: lambdaRole,
      timeout: cdk.Duration.seconds(30),
      memorySize: 256,
      logRetention: logs.RetentionDays.ONE_WEEK,
    });

    // Add cost allocation tags
    cdk.Tags.of(deviceLambda).add('CostCenter', 'BRAVO-API');
    cdk.Tags.of(deviceLambda).add('Service', 'Lambda-Devices');
    cdk.Tags.of(telemetryLambda).add('CostCenter', 'BRAVO-API');
    cdk.Tags.of(telemetryLambda).add('Service', 'Lambda-Telemetry');

    // API Gateway REST API
    this.api = new apigateway.RestApi(this, 'BravoApi', {
      restApiName: `${props.projectName}-API-${props.stage}`,
      description: 'B.R.A.V.O. REST API',
      deployOptions: {
        stageName: 'prod',
        throttlingRateLimit: 1000,
        throttlingBurstLimit: 2000,
        loggingLevel: apigateway.MethodLoggingLevel.INFO,
        dataTraceEnabled: true,
        metricsEnabled: true,
      },
      defaultCorsPreflightOptions: {
        allowOrigins: apigateway.Cors.ALL_ORIGINS,
        allowMethods: apigateway.Cors.ALL_METHODS,
        allowHeaders: ['Content-Type', 'X-Amz-Date', 'Authorization', 'X-Api-Key'],
      },
    });

    // Device endpoints
    const devices = this.api.root.addResource('devices');
    const device = devices.addResource('{deviceId}');
    
    devices.addMethod('GET', new apigateway.LambdaIntegration(deviceLambda));
    devices.addMethod('POST', new apigateway.LambdaIntegration(deviceLambda));
    device.addMethod('GET', new apigateway.LambdaIntegration(deviceLambda));
    device.addMethod('DELETE', new apigateway.LambdaIntegration(deviceLambda));

    // Telemetry endpoints
    const telemetry = this.api.root.addResource('telemetry');
    const telemetryDevice = telemetry.addResource('{deviceId}');
    
    telemetry.addMethod('POST', new apigateway.LambdaIntegration(telemetryLambda));
    telemetryDevice.addMethod('GET', new apigateway.LambdaIntegration(telemetryLambda));

    // Add cost allocation tags
    cdk.Tags.of(this.api).add('CostCenter', 'BRAVO-API');
    cdk.Tags.of(this.api).add('Service', 'API-Gateway');

    // Output values
    new cdk.CfnOutput(this, 'ApiEndpoint', {
      value: this.api.url,
      description: 'API Gateway Endpoint',
      exportName: `${props.projectName}-ApiEndpoint-${props.stage}`,
    });

    new cdk.CfnOutput(this, 'DeviceFunctionName', {
      value: deviceLambda.functionName,
      description: 'Device Lambda Function Name',
      exportName: `${props.projectName}-DeviceFunctionName-${props.stage}`,
    });

    new cdk.CfnOutput(this, 'TelemetryFunctionName', {
      value: telemetryLambda.functionName,
      description: 'Telemetry Lambda Function Name',
      exportName: `${props.projectName}-TelemetryFunctionName-${props.stage}`,
    });
  }
}
