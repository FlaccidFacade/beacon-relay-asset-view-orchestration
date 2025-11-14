# beacon-relay-for-asset-view-&-orchestration — Code Name: B.R.A.V.O.

**B.R.A.V.O.** is the GitHub organization powering an **open-source smart collar**.

The idea comes from my own paranoia and fear of losing my free-spirited shepsky, **Bravo**. This project aims to combine the latest in **LoRa, GPS, BLE, IMU, and LTE/5G technology** to create a scalable, modular system for tracking and monitoring devices — starting with collars for pets like Bravo.


## 🚀 Mission

* Build an **open-source ecosystem** for asset tracking.
* Create a **modular, future-proof architecture** to support additional sensors and features.
* Provide a **secure and scalable platform** for telemetry, mapping, notifications.


## 📦 Current Repositories

| Project            | Description                                                                  | Repo                                                                           |
| ------------------ | ---------------------------------------------------------------------------- | ------------------------------------------------------------------------------ |
| **Firmware**       | ESP32 collar & dongle firmware (LoRa, GPS, BLE, IMU, OTA updates)            | [firmware](https://github.com/BT-Radio-Advanced-Visual-Orchestration/firmware) |
| **Mobile**         | Galaxy Android app for LoRa gateway, telemetry, and map visualization        | [mobile](https://github.com/BT-Radio-Advanced-Visual-Orchestration/mobile)     |
| **Web Dashboard**  | React/Next.js dashboard at domain tbd                                        | [web](https://github.com/BT-Radio-Advanced-Visual-Orchestration/web)           |
| **API**            | AWS Lambda / API Gateway endpoints for telemetry, OTA, and device management | [api](https://github.com/BT-Radio-Advanced-Visual-Orchestration/api)           |
| **Infrastructure**(private) | IaC stacks for AWS (IoT Core, DynamoDB, S3/CloudFront)              | [infra](https://github.com/BT-Radio-Advanced-Visual-Orchestration/infra)       |
| **.gitub**         | organization wide workflows, templates, and more                             | [.github](https://github.com/BT-Radio-Advanced-Visual-Orchestration/.github)   |
| **Documentation**  | to hold project wide documention like diagrams and procedures                | [docs](https://github.com/BT-Radio-Advanced-Visual-Orchestration/docs)         |


## 💖 Why Open Source

Sharing this project allows other pet owners and IoT enthusiasts to:

* Learn from the design and implementation.
* Contribute improvements and new features.
* Adapt the system for different use cases, like personal tracking, gear monitoring, or IoT experiments.


## 🛠️ Development Setup

### VSCode Extensions

This project includes recommended VSCode extensions for optimal development experience. When you open this workspace in VSCode:

1. You'll see a notification to install recommended extensions - click **Install All**
2. Or manually: Open Extensions (`Ctrl+Shift+X`), filter by `@recommended`, and install

The extensions support all technologies in this project:
- TypeScript/JavaScript (API, web, infra)
- PlatformIO/C++/Arduino (firmware)
- Kotlin/Java (mobile Android)
- AWS CDK (infrastructure)

For detailed installation instructions, see [.vscode/README.md](.vscode/README.md).


## 🛠 Contribution

We welcome contributions!

1. Fork the relevant repository.
2. Open an issue describing the feature or bug.
3. Submit a pull request with clear descriptions and tests.


## 🌐 Contact

* **Email:** [bravo-gps@proton.me](mailto:bravo-gps@proton.me)
* **Discussions / Issues:** Use each repo’s Issues tab for project-specific questions.

