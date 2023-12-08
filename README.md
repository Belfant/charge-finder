# ChargeFinder 

### Overview
ChargeFinder is a hobby project that utilizes an ESP32 feather board with WiFi capabilities together with RCWL-1601 sensors and a Flask web server to monitor the availability of electric vehicle charging spots in a parking garage. The project provides real-time updates on the occupancy of each parking spot via a web interface. Can be powered either through micro-usb or LiPo battery. 

### Features
- Real-Time Parking Spot Monitoring: Uses ESP32 with RCWL-1601 sensors to detect vehicle presence in parking spots.
- Web-Based Interface: Displays the current status of each parking spot (occupied/empty) with visual indicators.
- Battery and Signal Strength Monitoring: Tracks and displays the battery level and Wi-Fi signal strength of the ESP32 board.
- Responsive Design: The web interface is designed to be user-friendly and accessible from various devices.

### 3D Design Blueprints
The repository includes 3D design blueprints for the ESP32 board and sensors, which can be used to print custom cases for the hardware components.
Available here:
https://www.tinkercad.com/things/fNzbke9PvNR-parking-sensor-feather-pcb-container

### Web interface
![ChargeFinder](https://github.com/Belfant/charge-finder/assets/134707588/1dc9aab9-023c-4276-8c3f-952a338ac86f)

### Hardware Components
- Adafruit Huzzah ESP32 Feather Board
- RCWL-1601 Ultrasonic Sensors (2x) (HC-SR04 Compatible)
- Miscellaneous: Cables, Power Supply, etc.

### Software Components
- Arduino program for the ESP32 and sensors
- Flask web server for handling sensor data and serving the webpage.
- JavaScript for dynamically updating the web page based on sensor data.
- HTML/CSS for the webpage layout and styling.

# Pictures

### Design
Available here
https://www.tinkercad.com/things/fNzbke9PvNR-parking-sensor-feather-pcb-container

![image](https://github.com/Belfant/charge-finder/assets/134707588/f83f05f7-ad3e-486b-9108-e1067412c4b8)

### Assembled
![Device small cropeed](https://github.com/Belfant/charge-finder/assets/134707588/c18a715c-203a-4cf4-ae49-ec03d15b715b)
