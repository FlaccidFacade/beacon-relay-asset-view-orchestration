# Contributing to BRAVO Mobile

Thank you for your interest in contributing to the BRAVO Mobile Android application!

## Understanding the System Architecture

Before contributing, it's important to understand that:

**The mobile phone does NOT receive LoRa radio signals directly.**

The BRAVO system uses a relay architecture:
```
[GPS Collar/Dongle] --LoRa Radio--> [ESP32 Relay] --USB/BLE--> [Mobile Phone]
    Transmitter                    LoRa Receiver              This App
```

The mobile app:
- Connects to ESP32 relay devices via USB or BLE
- Receives telemetry data that was transmitted via LoRa to the relay
- Cannot access LoRa information without the relay or an API/dashboard

When making changes, maintain this architectural separation.

## Development Setup

1. Fork the repository
2. Clone your fork locally:
   ```bash
   git clone https://github.com/YOUR_USERNAME/beacon-relay-asset-view-orchestration.git
   cd beacon-relay-asset-view-orchestration/mobile
   ```
3. Open the project in Android Studio
4. Create a new branch for your feature

## Code Style

- Follow Android coding conventions
- Use meaningful variable and method names
- Add comments for complex logic
- Keep methods focused and concise
- Follow the existing project structure

## Project Structure

- `activities/` - UI screens and user interactions
- `services/` - Background services for BLE and USB communication
- `libs/` - Core libraries and utilities (parsers, receivers, visualization)
- `models/` - Data models and entities
- `utils/` - Helper classes and constants

## Making Changes

1. Create a feature branch: `git checkout -b feature/your-feature-name`
2. Make your changes
3. Test thoroughly on both physical devices and emulators
4. Commit with clear messages: `git commit -m "Add feature: description"`
5. Push to your fork: `git push origin feature/your-feature-name`
6. Create a Pull Request

## Testing

- Test on multiple Android versions (minimum API 26)
- Test both BLE and USB connections if modifying connectivity
- Test with and without internet for map features
- Verify permissions are properly requested
- Check for memory leaks in long-running services

## Pull Request Guidelines

- Provide a clear description of changes
- Reference any related issues
- Include screenshots for UI changes
- Ensure code builds without errors
- Update documentation if needed

## Reporting Issues

When reporting bugs, include:
- Android version
- Device model
- Steps to reproduce
- Expected vs actual behavior
- Relevant logs or screenshots

## Questions?

Open an issue for questions or discussion about potential contributions.
