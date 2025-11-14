# beacon-relay-for-asset-view-&-orchestration — Code Name: B.R.A.V.O.

**B.R.A.V.O.** is the GitHub organization powering an **open-source smart collar**.

The idea comes from my own paranoia and fear of losing my free-spirited shepsky, **Bravo**. This project aims to combine the latest in **LoRa, GPS, BLE, IMU, and LTE/5G technology** to create a scalable, modular system for tracking and monitoring devices — starting with collars for pets like Bravo.


## 🚀 Mission

* Build an **open-source ecosystem** for asset tracking.
* Create a **modular, future-proof architecture** to support additional sensors and features.
* Provide a **secure and scalable platform** for telemetry, mapping, notifications.


## 📦 Monorepo Structure

This project uses a monorepo architecture with all components in a single repository:

| Directory          | Description                                                                  |
| ------------------ | ---------------------------------------------------------------------------- |
| **firmware/**      | ESP32 collar & dongle firmware (LoRa, GPS, BLE, IMU, OTA updates)            |
| **mobile/**        | Galaxy Android app for LoRa gateway, telemetry, and map visualization        |
| **web/**           | React/Next.js dashboard at domain tbd                                        |
| **api/**           | AWS Lambda / API Gateway endpoints for telemetry, OTA, and device management |
| **infra/**         | IaC stacks for AWS (IoT Core, DynamoDB, S3/CloudFront)                       |
| **docs/**          | Project-wide documentation including diagrams and procedures                 |
| **quickstart/**    | Quick start guides and setup instructions                                    |


## 💖 Why Open Source

Sharing this project allows other pet owners and IoT enthusiasts to:

* Learn from the design and implementation.
* Contribute improvements and new features.
* Adapt the system for different use cases, like personal tracking, gear monitoring, or IoT experiments.


## 🛠 Contribution

We welcome contributions!

1. Fork this repository.
2. Navigate to the relevant directory (`api/`, `firmware/`, `mobile/`, `web/`, etc.).
3. Open an issue describing the feature or bug.
4. Submit a pull request with clear descriptions and tests.


## 🌐 Contact

* **Email:** [bravo-gps@proton.me](mailto:bravo-gps@proton.me)
* **Discussions / Issues:** Use each repo’s Issues tab for project-specific questions.

