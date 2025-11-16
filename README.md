# beacon-relay-for-asset-view-&-orchestration — Code Name: B.R.A.V.O.

**B.R.A.V.O.** is an **open-source smart collar** project housed in a monorepo.

The idea comes from my own paranoia and fear of losing my free-spirited shepsky, **Bravo**. This project aims to combine the latest in **LoRa, GPS, BLE, IMU, and LTE/5G technology** to create a scalable, modular system for tracking and monitoring devices — starting with collars for pets like Bravo.

## 🚀 Mission

- Build an **open-source ecosystem** for asset tracking.
- Create a **modular, future-proof architecture** to support additional sensors and features.
- Provide a **secure and scalable platform** for telemetry, mapping, notifications.

## 📦 Monorepo Structure

This repository contains all components of the B.R.A.V.O. system in a single monorepo:

| Directory       | Description                                                                  |
| --------------- | ---------------------------------------------------------------------------- |
| **firmware/**   | ESP32 collar & dongle firmware (LoRa, GPS, BLE, IMU, OTA updates)            |
| **mobile/**     | Galaxy Android app for LoRa gateway, telemetry, and map visualization        |
| **web/**        | React/Next.js dashboard at domain tbd                                        |
| **api/**        | AWS Lambda / API Gateway endpoints for telemetry, OTA, and device management |
| **infra/**      | IaC stacks for AWS (IoT Core, DynamoDB, S3/CloudFront)                       |
| **.github/**    | Repository workflows, templates, and automation                              |
| **docs/**       | Project-wide documentation including diagrams and procedures                 |
| **quickstart/** | Quick start guide and initial setup documentation                            |

## 💖 Why Open Source

Sharing this project allows other pet owners and IoT enthusiasts to:

- Learn from the design and implementation.
- Contribute improvements and new features.
- Adapt the system for different use cases, like personal tracking, gear monitoring, or IoT experiments.

## 🔧 Development Setup

### VS Code Setup (Recommended)

This project includes a preconfigured VS Code workspace with recommended extensions for all technologies used in the monorepo.

**Quick Start:**

1. Open this repository in VS Code
2. When prompted, click "Install All" to install recommended extensions
3. Alternatively, run the installation script:

   ```bash
   # macOS/Linux
   .vscode/install-extensions.sh

   # Windows
   .vscode\install-extensions.bat
   ```

See [`.vscode/README.md`](.vscode/README.md) for detailed information about the extensions and workspace settings.

## 🛠 Contribution

We welcome contributions!

1. Fork this repository.
2. Open an issue describing the feature or bug.
3. Submit a pull request with clear descriptions and tests.
4. See component-specific CONTRIBUTING.md files for detailed guidelines.

## 🌐 Contact

- **Email:** [bravo-gps@proton.me](mailto:bravo-gps@proton.me)
- **Issues:** Use the [Issues tab](https://github.com/FlaccidFacade/beacon-relay-asset-view-orchestration/issues) for project questions and bug reports.
