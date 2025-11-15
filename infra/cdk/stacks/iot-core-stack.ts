import * as cdk from "aws-cdk-lib";
import { Construct } from "constructs";
import * as iot from "aws-cdk-lib/aws-iot";
import * as iam from "aws-cdk-lib/aws-iam";

export interface IoTCoreStackProps extends cdk.StackProps {
  projectName: string;
  stage: string;
}

export class IoTCoreStack extends cdk.Stack {
  public readonly iotEndpoint: string;

  constructor(scope: Construct, id: string, props: IoTCoreStackProps) {
    super(scope, id, props);

    // Create IoT Thing Type for B.R.A.V.O devices
    const thingType = new iot.CfnThingType(this, "BRAVOThingType", {
      thingTypeName: `${props.projectName}-Device-${props.stage}`,
      thingTypeProperties: {
        thingTypeDescription: "B.R.A.V.O IoT Device Type",
      },
    });

    // Create IoT Policy for devices
    const iotPolicy = new iot.CfnPolicy(this, "DevicePolicy", {
      policyName: `${props.projectName}-DevicePolicy-${props.stage}`,
      policyDocument: {
        Version: "2012-10-17",
        Statement: [
          {
            Effect: "Allow",
            Action: ["iot:Connect"],
            Resource: [
              `arn:aws:iot:${this.region}:${this.account}:client/\${iot:Connection.Thing.ThingName}`,
            ],
          },
          {
            Effect: "Allow",
            Action: ["iot:Publish", "iot:Receive"],
            Resource: [
              `arn:aws:iot:${this.region}:${this.account}:topic/${props.projectName.toLowerCase()}/devices/\${iot:Connection.Thing.ThingName}/*`,
            ],
          },
          {
            Effect: "Allow",
            Action: ["iot:Subscribe"],
            Resource: [
              `arn:aws:iot:${this.region}:${this.account}:topicfilter/${props.projectName.toLowerCase()}/devices/\${iot:Connection.Thing.ThingName}/*`,
            ],
          },
        ],
      },
    });

    // Get IoT endpoint
    this.iotEndpoint = `${this.account}.iot.${this.region}.amazonaws.com`;

    // Add cost allocation tags
    cdk.Tags.of(thingType).add("CostCenter", "BRAVO-IoT");
    cdk.Tags.of(thingType).add("Service", "IoT-Core");
    cdk.Tags.of(iotPolicy).add("CostCenter", "BRAVO-IoT");
    cdk.Tags.of(iotPolicy).add("Service", "IoT-Core");

    // Output values
    new cdk.CfnOutput(this, "ThingTypeName", {
      value: thingType.thingTypeName || "",
      description: "IoT Thing Type Name",
      exportName: `${props.projectName}-ThingTypeName-${props.stage}`,
    });

    new cdk.CfnOutput(this, "PolicyName", {
      value: iotPolicy.policyName || "",
      description: "IoT Device Policy Name",
      exportName: `${props.projectName}-PolicyName-${props.stage}`,
    });

    new cdk.CfnOutput(this, "IoTEndpoint", {
      value: this.iotEndpoint,
      description: "IoT Core Endpoint",
      exportName: `${props.projectName}-IoTEndpoint-${props.stage}`,
    });
  }
}
