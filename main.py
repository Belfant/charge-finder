from flask import Flask, render_template, request, jsonify

app = Flask(__name__)

# In-memory storage of occupancy status for two sensors
occupied_status = {
    'sensor1': False,
    'sensor2': False
}

# In-memory storage of battery and WiFi signal strength
utils = {
    'battery': 0,
    'signalStrength': 0
}

@app.route('/')
def index():
    # Pass the current occupancy status to the template
    return render_template('index.html', occupied1=occupied_status['sensor1'], occupied2=occupied_status['sensor2'])

@app.route('/update_status', methods=['POST'])
def update_status():
    data = request.json
    sensor_id = data.get('sensor') # Get the sensor ID from the request
    occupied = data.get('occupied', False)

    # Ensure that the sensor_id is valid
    if sensor_id not in occupied_status:
        return jsonify(success=False, error="Invalid sensor ID"), 400

    # Update the in-memory storage for the specified sensor
    occupied_status[sensor_id] = occupied
    return jsonify(success=True), 200

@app.route('/update_utils', methods=['POST'])
def update_utils():
    data = request.json
    battery = data.get('battery')
    signal_strength = data.get('signalStrength')

    # Validate the received data
    if battery is not None:
        utils['battery'] = battery
    if signal_strength is not None:
        utils['signalStrength'] = signal_strength

    return jsonify(success=True), 200

@app.route('/get_status', methods=['GET'])
def get_status():
    # Return the current occupancy status for both sensors
    return jsonify(occupied_status), 200

@app.route('/get_utils', methods=['GET'])
def get_utils():
    return jsonify(utils), 200

if __name__ == '__main__':
    app.run(debug=True)
