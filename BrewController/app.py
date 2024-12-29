from flask import Flask, jsonify, render_template_string
import sqlite3

# Flask App
app = Flask(__name__)

# SQLite Database Path
DB_PATH = "mydb.sqlite"

# Query to fetch data from the SensorState table
QUERY = """
SELECT iteration, smoothedPressure, temperature, target_pressure, target_temperature
FROM SensorState;
"""

# Fetch data from the SQLite database
def fetch_data():
    try:
        print("Connecting to database...")
        connection = sqlite3.connect(DB_PATH)
        cursor = connection.cursor()
        print("Executing query...")
        cursor.execute(QUERY)
        rows = cursor.fetchall()
        cursor.close()
        connection.close()
        print(f"Fetched rows: {rows}")

        if not rows:
            print("No data found in the SensorState table.")

        # Parse data into a usable format for the frontend
        data = {
            'x': [],  # IterationNumber
            'pressure': [],
            'temperature': [],
            'target_pressure': [],
            'target_temperature': []
        }
        for row in rows:
            data['x'].append(row[0])
            data['pressure'].append(row[1])
            data['temperature'].append(row[2])
            data['target_pressure'].append(row[3])
            data['target_temperature'].append(row[4])

        return data
    except sqlite3.Error as e:
        print(f"Database error: {e}")
        return None
    except Exception as e:
        print(f"Unexpected error: {e}")
        return None

# API route to serve the data
@app.route('/data')
def get_data():
    print("Fetching data from API route...")
    data = fetch_data()
    if data is None:
        return jsonify({"error": "Failed to fetch data from the database."}), 500
    return jsonify(data)

# Root route to serve the graph
@app.route('/')
def index():
    # Inline HTML template
    html_template = """
    <!DOCTYPE html>
    <html>
    <head>
        <title>BrewView</title>
        <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    </head>
    <body>
        <h2>SensorState Data</h2>
        <canvas id="myChart" width="800" height="400"></canvas>
        <script>
            // Fetch data from the server
            fetch('/data')
                .then(response => {
                    if (!response.ok) {
                        throw new Error("Failed to fetch data from the server.");
                    }
                    return response.json();
                })
                .then(data => {
                    const ctx = document.getElementById('myChart').getContext('2d');
                    new Chart(ctx, {
                        type: 'line',  // Line graph
                        data: {
                            labels: data.x, // IterationNumber as X-axis
                            datasets: [
                                {
                                    label: 'Pressure',
                                    data: data.pressure,
                                    borderColor: 'rgba(255, 99, 132, 1)',
                                    borderWidth: 1,
                                    pointRadius: 0
                                },
                                {
                                    label: 'Temperature',
                                    data: data.temperature,
                                    borderColor: 'rgba(54, 162, 235, 1)',
                                    borderWidth: 1,
                                    pointRadius: 0
                                },
                                {
                                    label: 'Target Pressure',
                                    data: data.target_pressure,
                                    borderColor: 'rgba(75, 192, 192, 1)',
                                    borderWidth: 1,
                                    pointRadius: 0
                                },
                                {
                                    label: 'Target Temperature',
                                    data: data.target_temperature,
                                    borderColor: 'rgba(153, 102, 255, 1)',
                                    borderWidth: 1,
                                    pointRadius: 0
                                }
                            ]
                        },
                        options: {
                            responsive: true,
                            plugins: {
                                tooltip: { enabled: true },
                                legend: { display: true },
                            },
                            scales: {
                                x: {
                                    type: 'linear',
                                    position: 'bottom',
                                    ticks: { autoSkip: true, maxTicksLimit: 10 }
                                }
                            }
                        }
                    });
                })
                .catch(error => {
                    console.error("Error loading data:", error);
                    alert("Failed to load data. Check the console for details.");
                });
        </script>
    </body>
    </html>
    """
    return render_template_string(html_template)

# Run the Flask app
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
