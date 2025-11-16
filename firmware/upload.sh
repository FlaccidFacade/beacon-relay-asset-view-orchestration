#!/bin/bash
# Helper script to upload LoRa test firmware

echo "========================================="
echo "  B.R.A.V.O. LoRa Test Upload Helper"
echo "========================================="
echo ""

# Check which port is available
if [ -e /dev/ttyUSB0 ]; then
    PORT="/dev/ttyUSB0"
    echo "✓ Found board on $PORT"
elif [ -e /dev/ttyUSB1 ]; then
    PORT="/dev/ttyUSB1"
    echo "✓ Found board on $PORT"
else
    echo "✗ No board found!"
    echo "  Please connect a Heltec board via USB"
    exit 1
fi

# Set permissions
echo "Setting permissions..."
sudo chmod 666 "$PORT"

# Upload
echo "Uploading firmware..."
~/.platformio/penv/bin/platformio run --target upload --upload-port $PORT

if [ $? -eq 0 ]; then
    echo ""
    echo "========================================="
    echo "  ✓ Upload Successful!"
    echo "========================================="
else
    echo ""
    echo "========================================="
    echo "  ✗ Upload Failed"
    echo "========================================="
    exit 1
fi
