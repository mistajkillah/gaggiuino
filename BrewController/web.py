import dash
from dash import dcc, html, Input, Output, State
import socket
import json
import plotly.graph_objs as go
import pandas as pd
from sqlalchemy import create_engine
from sqlalchemy.sql import text

# RPC Server Configuration
RPC_SERVER_HOST = "localhost"
RPC_SERVER_PORT = 9050

# Database connection (for graph controls)
DATABASE_URL = "sqlite:////tmp/mydb.sqlite"

# Create database engine
engine = create_engine(DATABASE_URL, connect_args={"check_same_thread": False})

# Enable PRAGMA read_uncommitted
def enable_read_uncommitted(connection):
    connection.execute(text("PRAGMA read_uncommitted = true;"))

# Plain TCP socket communication
def send_rpc_request(payload):
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.connect((RPC_SERVER_HOST, RPC_SERVER_PORT))
            request_data = json.dumps(payload)
            sock.sendall(request_data.encode("utf-8"))
            response_data = sock.recv(4096)
            return json.loads(response_data.decode("utf-8"))
    except Exception as e:
        return {"error": str(e)}

# API Implementation
def get_debug_dump():
    payload = {"command": "get_debug_dump", "command_id": 101}
    return send_rpc_request(payload)

def get_temperature():
    payload = {"command": "get_temperature", "command_id": 102}
    return send_rpc_request(payload)

def get_pressure():
    payload = {"command": "get_pressure", "command_id": 103}
    return send_rpc_request(payload)

def control_heater(state):
    payload = {"command": "control_heater", "command_id": 104, "parameters": {"state": state}}
    return send_rpc_request(payload)

def control_pump(duration):
    payload = {"command": "control_pump", "command_id": 105, "parameters": {"duration": duration}}
    return send_rpc_request(payload)

# Fetch table names dynamically
def fetch_table_names():
    query = "SELECT name FROM sqlite_master WHERE type='table';"
    with engine.connect() as connection:
        enable_read_uncommitted(connection)
        tables = pd.read_sql(query, connection)
    return tables["name"].tolist()

# Fetch column names dynamically for a specific table
def fetch_schema(table_name):
    query = f"PRAGMA table_info({table_name});"
    with engine.connect() as connection:
        enable_read_uncommitted(connection)
        schema = pd.read_sql(query, connection)
    return schema["name"].tolist()

# Fetch data from a specific table
def fetch_data(table_name):
    query = f"SELECT * FROM {table_name};"
    with engine.connect() as connection:
        enable_read_uncommitted(connection)
        data = pd.read_sql(query, connection)
    return data

# Initialize Dash app
app = dash.Dash(__name__)
app.title = "Dynamic Streaming Data Visualization"

# Fetch initial table names
tables = fetch_table_names()

# Default settings for the graph
DEFAULT_X_AXIS = "iteration"
DEFAULT_Y_AXIS = [
    "target_pressure",
    "target_temperature",
    "smoothedPressure",
    "temperature",
    "brewCtrl_temp_output",
    "brewCtrl_heater_state",
    "brewCtrl_pressure_output",
]

# Layout
app.layout = html.Div(
    style={
        "backgroundColor": "#1e2130",
        "color": "white",
        "font-family": "Arial, sans-serif",
        "padding": "20px",
    },
    children=[
        html.H1(
            "Dynamic Streaming Data Visualization",
            style={"text-align": "center", "color": "white", "margin-bottom": "20px"},
        ),
        html.Div(
            style={"display": "flex", "flex-direction": "row", "gap": "20px"},
            children=[
                # Left Column: Controls and Debug
                html.Div(
                    style={
                        "width": "50%",
                        "backgroundColor": "#2b2f3a",
                        "padding": "20px",
                        "borderRadius": "8px",
                    },
                    children=[
                        html.Label("Controls", style={"font-size": "20px", "color": "white", "margin-bottom": "10px"}),
                        html.Button("Heater ON", id="heater-on-button", n_clicks=0, style={"margin-bottom": "10px"}),
                        html.Button("Heater OFF", id="heater-off-button", n_clicks=0, style={"margin-bottom": "10px"}),
                        html.Button("Read Temperature", id="temperature-button", n_clicks=0, style={"margin-bottom": "10px"}),
                        html.Button("Read Pressure", id="pressure-button", n_clicks=0, style={"margin-bottom": "10px"}),
                        html.Div([
                            dcc.Input(id="pump-seconds-input", type="number", placeholder="Seconds"),
                            html.Button("Run Pump", id="pump-button", n_clicks=0, style={"margin-left": "10px"}),
                        ], style={"margin-bottom": "10px", "display": "flex", "align-items": "center"}),
                        html.Button("Dump Status", id="dump-status-button", n_clicks=0, style={"margin-bottom": "10px"}),
                        html.Label("Debug Output:", style={"margin-top": "20px", "color": "white"}),
                        dcc.Textarea(
                            id="status-dump-output",
                            style={
                                "width": "100%",
                                "height": "200px",
                                "backgroundColor": "#1e2130",
                                "color": "white",
                                "border": "1px solid #444",
                                "padding": "10px",
                            },
                            readOnly=True,
                        ),
                    ]
                ),
                # Right Column: Graph Controls
                html.Div(
                    style={
                        "width": "50%",
                        "backgroundColor": "#2b2f3a",
                        "padding": "20px",
                        "borderRadius": "8px",
                    },
                    children=[
                        html.Label("Graph Controls", style={"font-size": "20px", "color": "white", "margin-bottom": "10px"}),
                        html.Label("Select Table:", style={"color": "white"}),
                        dcc.Dropdown(
                            id="table-dropdown",
                            options=[{"label": table, "value": table} for table in tables],
                            placeholder="Select a table",
                            value=tables[0] if tables else None,  # Default to the first table
                            style={
                                "backgroundColor": "#2b2f3a",
                                "color": "white",
                                "font-size": "16px",
                                "border": "1px solid #444",
                                "borderRadius": "5px",
                            },
                        ),
                        html.Label("Select X-axis:", style={"color": "white", "margin-top": "10px"}),
                        dcc.Dropdown(
                            id="x-axis-dropdown",
                            placeholder="Select X-axis",
                            clearable=False,
                            value=DEFAULT_X_AXIS,  # Default X-axis value
                            style={
                                "backgroundColor": "#2b2f3a",
                                "color": "white",
                                "font-size": "16px",
                                "border": "1px solid #444",
                                "borderRadius": "5px",
                            },
                        ),
                        html.Label("Select Y-axis (multiple):", style={"color": "white", "margin-top": "10px"}),
                        dcc.Dropdown(
                            id="y-axis-dropdown",
                            placeholder="Select Y-axis",
                            clearable=False,
                            multi=True,
                            value=DEFAULT_Y_AXIS,  # Default Y-axis values
                            style={
                                "backgroundColor": "#2b2f3a",
                                "color": "white",
                                "font-size": "16px",
                                "border": "1px solid #444",
                                "borderRadius": "5px",
                            },
                        ),
                    ]
                ),
            ],
        ),
        html.Div(
            style={"margin-top": "20px"},
            children=[
                dcc.Graph(
                    id="live-graph",
                    style={
                        "backgroundColor": "#1e2130",
                        "border": "1px solid #444",
                        "borderRadius": "8px",
                    },
                ),
            ]
        ),
        # Interval component for streaming updates
        dcc.Interval(
            id="interval-component",
            interval=10000,  # Update every 10 seconds
            n_intervals=0,
        ),
    ]
)

# Callbacks for Graph Controls
@app.callback(
    [Output("x-axis-dropdown", "options"),
     Output("y-axis-dropdown", "options")],
    Input("table-dropdown", "value"),
)
def update_axis_options(selected_table):
    if not selected_table:
        return [], []
    columns = fetch_schema(selected_table)
    options = [{"label": col, "value": col} for col in columns]
    return options, options

@app.callback(
    Output("live-graph", "figure"),
    [Input("table-dropdown", "value"),
     Input("x-axis-dropdown", "value"),
     Input("y-axis-dropdown", "value")],
)
def update_graph(selected_table, x_axis, y_axes):
    if not selected_table or not x_axis or not y_axes:
        return go.Figure()
    data = fetch_data(selected_table)
    figure = go.Figure()
    for y in y_axes:
        figure.add_trace(go.Scatter(x=data[x_axis], y=data[y], mode="lines", name=y))
    return figure

# Callbacks for RPC controls
@app.callback(
    Output("status-dump-output", "value"),
    [Input("heater-on-button", "n_clicks"),
     Input("heater-off-button", "n_clicks"),
     Input("temperature-button", "n_clicks"),
     Input("pressure-button", "n_clicks"),
     Input("pump-button", "n_clicks")],
    [State("pump-seconds-input", "value")],
)
def handle_controls(heater_on, heater_off, temp_btn, pressure_btn, pump_btn, pump_duration):
    ctx = dash.callback_context
    if not ctx.triggered:
        return ""
    button_id = ctx.triggered[0]["prop_id"].split(".")[0]
    if button_id == "heater-on-button":
        return control_heater(True)
    elif button_id == "heater-off-button":
        return control_heater(False)
    elif button_id == "temperature-button":
        return get_temperature()
    elif button_id == "pressure-button":
        return get_pressure()
    elif button_id == "pump-button" and pump_duration is not None:
        return control_pump(pump_duration)
    return ""

# Run the app
if __name__ == "__main__":
    app.run_server(debug=True, host="0.0.0.0", port=8050)
