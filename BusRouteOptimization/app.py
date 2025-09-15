from flask import Flask, request, jsonify
from flask_cors import CORS
import subprocess

app = Flask(__name__)
CORS(app)

# Endpoint: get all cities
@app.route("/cities")
def cities():
    try:
        result = subprocess.run(
            ["./bus_planner", "cities"],
            capture_output=True,
            text=True,
            check=True
        )
        cities_list = result.stdout.strip().splitlines()
        return jsonify(cities_list)
    except Exception as e:
        return jsonify({"error": str(e)}), 500

# Endpoint: shortest path
@app.route("/shortest")
def shortest():
    src = request.args.get("src")
    dst = request.args.get("dst")
    if not src or not dst:
        return jsonify({"error": "Please provide src and dst"}), 400

    try:
        result = subprocess.run(
            ["./bus_planner", src, dst],
            capture_output=True,
            text=True,
            check=True
        )

        # Parse C++ output
        output_lines = result.stdout.strip().splitlines()
        data = {"path": "", "distance": "", "transfers": ""}
        for line in output_lines:
            if line.startswith("Path:"):
                data["path"] = line.replace("Path:", "").strip()
            elif line.startswith("Total Distance:"):
                data["distance"] = line.replace("Total Distance:", "").strip()
            elif line.startswith("Min Transfers:"):
                data["transfers"] = line.replace("Min Transfers:", "").strip()

        return jsonify(data)
    except subprocess.CalledProcessError as e:
        return jsonify({"error": e.stderr, "stdout": e.stdout}), 500
    except Exception as e:
        return jsonify({"error": str(e)}), 500

if __name__ == "__main__":
    app.run(debug=True)
