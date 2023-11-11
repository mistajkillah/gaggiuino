import json
import logging
import random
import sys
import time
import sqlite3
from datetime import datetime
from typing import Iterator

from flask import Flask, Response, render_template, request, stream_with_context

logging.basicConfig(stream=sys.stdout, level=logging.INFO, format="%(asctime)s %(levelname)s %(message)s")
logger = logging.getLogger(__name__)

application = Flask(__name__)


db_file="/tmp/mydb.sqlite"

@application.route("/")
def index() -> str:
    return render_template("index.html")

def generate_sensor_data() -> Iterator[str]:
    """
    Generates sensor data from an SQLite database.

    :return: Generator yielding JSON data with timestamp and sensor values.
    """

    try:
        logger.info("Client connected")
        conn = sqlite3.connect(db_file)
        cursor = conn.cursor()
        last_iteration = -1

        while True:
            start_time = time.time()

            # Fetch all SensorState data from the previous iteration to now
            cursor.execute("SELECT * FROM SensorState WHERE iteration > ? ORDER BY iteration", (last_iteration,))
            rows = cursor.fetchall()

            data = []

            for row in rows:
                iteration, datestamp, timeSinceBrewStart, timeSinceSystemStart, brewSwitchState, steamSwitchState, hotWaterSwitchState, isSteamForgottenON, scalesPresent, tarePending, temperature, waterTemperature, pressure, pressureChangeSpeed, pumpFlow, pumpFlowChangeSpeed, waterPumped, weightFlow, weight, shotWeight, smoothedPressure, smoothedPumpFlow, smoothedWeightFlow, consideredFlow, pumpClicks, waterLvl, tofReady = row

                # Format the data as a dictionary
                data_item = {
                    "iteration": iteration,
                    "datestamp": datestamp,
                    "timeSinceBrewStart": timeSinceBrewStart,
                    "timeSinceSystemStart": timeSinceSystemStart,
                    "brewSwitchState": brewSwitchState,
                    "steamSwitchState": steamSwitchState,
                    "hotWaterSwitchState": hotWaterSwitchState,
                    "isSteamForgottenON": isSteamForgottenON,
                    "scalesPresent": scalesPresent,
                    "tarePending": tarePending,
                    "temperature": temperature,
                    "waterTemperature": waterTemperature,
                    "pressure": pressure,
                    "pressureChangeSpeed": pressureChangeSpeed,
                    "pumpFlow": pumpFlow,
                    "pumpFlowChangeSpeed": pumpFlowChangeSpeed,
                    "waterPumped": waterPumped,
                    "weightFlow": weightFlow,
                    "weight": weight,
                    "shotWeight": shotWeight,
                    "smoothedPressure": smoothedPressure,
                    "smoothedPumpFlow": smoothedPumpFlow,
                    "smoothedWeightFlow": smoothedWeightFlow,
                    "consideredFlow": consideredFlow,
                    "pumpClicks": pumpClicks,
                    "waterLvl": waterLvl,
                    "tofReady": tofReady,
                }

                data.append(data_item)

                last_iteration = iteration

            # Convert the data to JSON
            json_data = json.dumps(data)

            yield f"data:{json_data}\n\n"

            end_time = time.time()
            elapsed_time = end_time - start_time
            sleep_time = max(0, 1 - elapsed_time)  # Ensure at least 1 second between iterations
            time.sleep(sleep_time)
    except GeneratorExit:
        logger.info("Client disconnected")
        pass
    finally:
        conn.close()
        
def generate_sensor_data_last() -> Iterator[str]:
    """
    Generates sensor data from an SQLite database.

    :param db_file: SQLite database file path.
    :return: Generator yielding JSON data with timestamp and sensor values.
    """
    
    if request.headers.getlist("X-Forwarded-For"):
        client_ip = request.headers.getlist("X-Forwarded-For")[0]
    else:
        client_ip = request.remote_addr or ""

    try:
        logger.info("Client %s connected", client_ip)
        conn = sqlite3.connect(db_file)
        cursor = conn.cursor()
        while True:
            start_time = time.time()

            # Fetch the latest SensorState data from the database
            cursor.execute("SELECT * FROM SensorState ORDER BY iteration DESC LIMIT 1")
            row = cursor.fetchone()
            if row:
                iteration, datestamp, timeSinceBrewStart, timeSinceSystemStart, brewSwitchState, steamSwitchState, hotWaterSwitchState, isSteamForgottenON, scalesPresent, tarePending, temperature, waterTemperature, pressure, pressureChangeSpeed, pumpFlow, pumpFlowChangeSpeed, waterPumped, weightFlow, weight, shotWeight, smoothedPressure, smoothedPumpFlow, smoothedWeightFlow, consideredFlow, pumpClicks, waterLvl, tofReady = row

                # Format the data as a dictionary
                data = {
                    "iteration": iteration,
                    "datestamp": datestamp,
                    "timeSinceBrewStart": timeSinceBrewStart,
                    "timeSinceSystemStart": timeSinceSystemStart,
                    "brewSwitchState": brewSwitchState,
                    "steamSwitchState": steamSwitchState,
                    "hotWaterSwitchState": hotWaterSwitchState,
                    "isSteamForgottenON": isSteamForgottenON,
                    "scalesPresent": scalesPresent,
                    "tarePending": tarePending,
                    "temperature": temperature,
                    "waterTemperature": waterTemperature,
                    "pressure": pressure,
                    "pressureChangeSpeed": pressureChangeSpeed,
                    "pumpFlow": pumpFlow,
                    "pumpFlowChangeSpeed": pumpFlowChangeSpeed,
                    "waterPumped": waterPumped,
                    "weightFlow": weightFlow,
                    "weight": weight,
                    "shotWeight": shotWeight,
                    "smoothedPressure": smoothedPressure,
                    "smoothedPumpFlow": smoothedPumpFlow,
                    "smoothedWeightFlow": smoothedWeightFlow,
                    "consideredFlow": consideredFlow,
                    "pumpClicks": pumpClicks,
                    "waterLvl": waterLvl,
                    "tofReady": tofReady,
                }

                # Convert the data to JSON
                json_data = json.dumps(data)

                yield f"data:{json_data}\n\n"
            else:
                # If there's no data in the database, yield a placeholder message
                yield f"data:{{\"message\": \"No data available\"}}\n\n"

            end_time = time.time()
            elapsed_time = end_time - start_time
            sleep_time = max(0, 1 - elapsed_time)  # Ensure at least 1 second between iterations
            time.sleep(sleep_time)
    except GeneratorExit:
        logger.info("Client %s disconnected", client_ip)
        pass
    finally:
        conn.close()

@application.route("/chart-data")
def chart_data() -> Response:
    response = Response(stream_with_context(generate_sensor_data()), mimetype="text/event-stream")
    response.headers["Cache-Control"] = "no-cache"
    response.headers["X-Accel-Buffering"] = "no"
    return response


if __name__ == "__main__":
    application.run(host="0.0.0.0", threaded=True)
