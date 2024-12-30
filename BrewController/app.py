from flask import Flask, jsonify, render_template, request
import sqlite3

app = Flask(__name__)

# Default database path
DB_PATH = "mydb.sqlite"

DEFAULT_QUERY = """
SELECT iteration, smoothedPressure, temperature, target_pressure, target_temperature
FROM SensorState;
"""

# Default axis values
time_interval_ms = 10
pres_y_min = 0
pres_y_max = 15
temp_y_min = 50
temp_y_max = 120

# Function to fetch data from SQLite with dynamic columns
def fetch_data(columns):
    if not columns:
        return {"error": "No columns specified"}, 400

    column_list = ', '.join(columns)
    query = f"SELECT iteration, {column_list} FROM SensorState;"
    
    connection = sqlite3.connect(DB_PATH)
    cursor = connection.cursor()
    cursor.execute(query)
    rows = cursor.fetchall()
    cursor.close()
    connection.close()

    data = {'x': []}
    for col in columns:
        data[col] = []

    for row in rows:
        data['x'].append(row[0])  # iteration column
        for idx, col in enumerate(columns):
            data[col].append(row[idx + 1])

    return data

# Function to fetch column names from the database
def get_columns():
    connection = sqlite3.connect(DB_PATH)
    cursor = connection.cursor()
    cursor.execute("PRAGMA table_info(SensorState);")
    columns = [row[1] for row in cursor.fetchall()]
    cursor.close()
    connection.close()
    return columns

@app.route('/')
def index():
    return render_template(
        'index.html',
        time_interval_ms=time_interval_ms,
        pres_y_min=pres_y_min,
        pres_y_max=pres_y_max,
        temp_y_min=temp_y_min,
        temp_y_max=temp_y_max
    )

@app.route('/default-data')
def default_data():
    columns = request.args.get('columns', '').split(',')
    if not columns or columns == ['']:
        # Default columns if none are specified
        columns = ["smoothedPressure", "temperature", "target_pressure", "target_temperature"]
    data = fetch_data(columns)
    data['table_name'] = "SensorState"  # Add table name to response
    return jsonify(data)

@app.route('/columns')
def columns():
    columns = get_columns()
    return jsonify(columns)

if __name__ == '__main__':
    app.run(debug=True)
