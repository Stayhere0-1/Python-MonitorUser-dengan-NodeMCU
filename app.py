from flask import Flask, render_template, request, jsonify

app = Flask(__name__)

# Global variables to store sensor data
sensor_data = {
    "temperature": 0,
    "humidity": 0,
    "distance": 0,
    "light": 0
}

@app.route('/')
def index():
    print("WOI")
    return render_template('index.html')

#Ambil data dari parameter 
@app.route('/data', methods=['POST'])
def receive_data():
    global sensor_data
    sensor_data["temperature"] = float(request.form.get('temperature'))
    sensor_data["humidity"] = float(request.form.get('humidity'))
    sensor_data["distance"] = float(request.form.get('distance'))
    sensor_data["light"] = float(request.form.get('light'))
    
    return "Data received", 200

@app.route('/get_data', methods=['GET'])
def get_data():
    return jsonify(sensor_data)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
