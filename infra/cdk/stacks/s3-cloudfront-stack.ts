import * as cdk from 'aws-cdk-lib';
import { Construct } from 'constructs';
import * as s3 from 'aws-cdk-lib/aws-s3';
import * as cloudfront from 'aws-cdk-lib/aws-cloudfront';
import * as origins from 'aws-cdk-lib/aws-cloudfront-origins';
import * as s3deploy from 'aws-cdk-lib/aws-s3-deployment';

export interface S3CloudFrontStackProps extends cdk.StackProps {
  projectName: string;
  stage: string;
}

export class S3CloudFrontStack extends cdk.Stack {
  public readonly bucket: s3.Bucket;
  public readonly distribution: cloudfront.Distribution;

  constructor(scope: Construct, id: string, props: S3CloudFrontStackProps) {
    super(scope, id, props);

    // S3 Bucket for website hosting
    this.bucket = new s3.Bucket(this, 'WebsiteBucket', {
      bucketName: `${props.projectName.toLowerCase()}-website-${props.stage}-${this.account}`,
      removalPolicy: cdk.RemovalPolicy.RETAIN,
      autoDeleteObjects: false,
      versioned: true,
      encryption: s3.BucketEncryption.S3_MANAGED,
      blockPublicAccess: s3.BlockPublicAccess.BLOCK_ALL,
      enforceSSL: true,
    });

    // Origin Access Identity for CloudFront
    const originAccessIdentity = new cloudfront.OriginAccessIdentity(this, 'OAI', {
      comment: `OAI for ${props.projectName} Website`,
    });

    // Grant read access to CloudFront
    this.bucket.grantRead(originAccessIdentity);

    // CloudFront Distribution
    this.distribution = new cloudfront.Distribution(this, 'WebsiteDistribution', {
      defaultBehavior: {
        origin: new origins.S3Origin(this.bucket, {
          originAccessIdentity: originAccessIdentity,
        }),
        viewerProtocolPolicy: cloudfront.ViewerProtocolPolicy.REDIRECT_TO_HTTPS,
        allowedMethods: cloudfront.AllowedMethods.ALLOW_GET_HEAD_OPTIONS,
        cachedMethods: cloudfront.CachedMethods.CACHE_GET_HEAD_OPTIONS,
        compress: true,
        cachePolicy: cloudfront.CachePolicy.CACHING_OPTIMIZED,
      },
      defaultRootObject: 'index.html',
      errorResponses: [
        {
          httpStatus: 403,
          responseHttpStatus: 200,
          responsePagePath: '/index.html',
          ttl: cdk.Duration.minutes(5),
        },
        {
          httpStatus: 404,
          responseHttpStatus: 200,
          responsePagePath: '/index.html',
          ttl: cdk.Duration.minutes(5),
        },
      ],
      priceClass: cloudfront.PriceClass.PRICE_CLASS_100,
      enabled: true,
      comment: `${props.projectName} Website Distribution`,
    });

    // Add cost allocation tags
    cdk.Tags.of(this.bucket).add('CostCenter', 'BRAVO-WebHosting');
    cdk.Tags.of(this.bucket).add('Service', 'S3-Website');
    cdk.Tags.of(this.distribution).add('CostCenter', 'BRAVO-WebHosting');
    cdk.Tags.of(this.distribution).add('Service', 'CloudFront-CDN');

    // Output values
    new cdk.CfnOutput(this, 'BucketName', {
      value: this.bucket.bucketName,
      description: 'S3 Bucket Name for Website',
      exportName: `${props.projectName}-WebsiteBucketName-${props.stage}`,
    });

    new cdk.CfnOutput(this, 'BucketArn', {
      value: this.bucket.bucketArn,
      description: 'S3 Bucket ARN for Website',
      exportName: `${props.projectName}-WebsiteBucketArn-${props.stage}`,
    });

    new cdk.CfnOutput(this, 'DistributionId', {
      value: this.distribution.distributionId,
      description: 'CloudFront Distribution ID',
      exportName: `${props.projectName}-DistributionId-${props.stage}`,
    });

    new cdk.CfnOutput(this, 'DistributionDomainName', {
      value: this.distribution.distributionDomainName,
      description: 'CloudFront Distribution Domain Name',
      exportName: `${props.projectName}-DistributionDomain-${props.stage}`,
    });

    new cdk.CfnOutput(this, 'WebsiteURL', {
      value: `https://${this.distribution.distributionDomainName}`,
      description: 'Website URL',
    });
  }
}
