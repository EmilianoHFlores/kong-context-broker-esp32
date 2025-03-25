from flask import Flask, send_file, jsonify

app = Flask(__name__)

# You can store the version in a variable
FIRMWARE_VERSION = "0.1.1"

@app.route('/firmware_update')
def firmware_update():
    return send_file('firmware.bin', as_attachment=False)

@app.route('/firmware_version')
def firmware_version():
    return jsonify({"version": FIRMWARE_VERSION})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8000, debug=True)