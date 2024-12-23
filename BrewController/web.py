import dash
from dash import dcc, html, Input, Output, State
import pandas as pd
from sqlalchemy import create_engine
from sqlalchemy.sql import text
import plotly.graph_objs as go
import json
import time

# Database connection
DATABASE_URL = "sqlite:////tmp/mydb.sqlite"  # Update with your SQLite database path

# Create database engine
engine = create_engine(DATABASE_URL, connect_args={"check_same_thread": False})

# Enable PRAGMA read_uncommitted
def enable_read_uncommitted(connection):
    connection.execute(text("PRAGMA read_uncommitted = true;"))

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

# Simulated hardware interaction functions
def toggle_heater(state):
    return f"Heater turned {'ON' if state else 'OFF'}."

def read_temperature():
    return "Temperature: 25.5°C"

def read_pressure():
    return "Pressure: 1.2 bar"

def run_pump(seconds):
    time.sleep(seconds)  # Simulate running the pump
    return f"Pump ran for {seconds} seconds."

def dump_status():
    status = {
        "heater_status": "ON",
        "temperature": "25.5°C",
        "pressure": "1.2 bar",
        "last_pump_run": "5 seconds",
    }
    return json.dumps(status, indent=4)

# Initialize Dash app
app = dash.Dash(__name__)
app.title = "Dynamic Streaming Data Visualization"

# Fetch initial table names
tables = fetch_table_names()

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
        # Main content split into two columns
        html.Div(
            style={"display": "flex", "flex-direction": "row", "gap": "20px"},
            children=[
                # Left Column: Buttons and Debug Controls
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

                        # Status Output
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
                            style={
                                "backgroundColor": "#1e2130",
                                "color": "white",
                                "font-size": "16px",
                                "padding": "5px",
                                "borderRadius": "5px",
                            },
                        ),
                        html.Label("Select X-axis:", style={"color": "white", "margin-top": "10px"}),
                        dcc.Dropdown(
                            id="x-axis-dropdown",
                            placeholder="Select X-axis",
                            clearable=False,
                            style={
                                "backgroundColor": "#1e2130",
                                "color": "white",
                                "font-size": "16px",
                                "padding": "5px",
                                "borderRadius": "5px",
                            },
                        ),
                        html.Label("Select Y-axis (multiple):", style={"color": "white", "margin-top": "10px"}),
                        dcc.Dropdown(
                            id="y-axis-dropdown",
                            placeholder="Select Y-axis",
                            clearable=False,
                            multi=True,
                            style={
                                "backgroundColor": "#1e2130",
                                "color": "white",
                                "font-size": "16px",
                                "padding": "5px",
                                "borderRadius": "5px",
                            },
                        ),
                    ]
                ),
            ],
        ),
        # Graph
        html.Div(
            style={"margin-top": "20px"},
            children=[
                dcc.Graph(
                    id="live-graph",
                    style={"backgroundColor": "#1e2130"},
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

# Callbacks for controls and graph logic
@app.callback(
    Output("status-dump-output", "value"),
    Input("dump-status-button", "n_clicks"),
)
def handle_dump_status(n_clicks):
    if n_clicks > 0:
        return dump_status()
    return ""

@app.callback(
    [Output("x-axis-dropdown", "options"),
     Output("y-axis-dropdown", "options"),
     Output("x-axis-dropdown", "value"),
     Output("y-axis-dropdown", "value")],
    [Input("table-dropdown", "value")]
)
def update_columns(table_name):
    if not table_name:
        return [], [], None, None
    columns = fetch_schema(table_name)
    options = [{"label": col, "value": col} for col in columns]
    return options, options, columns[0], [columns[1]] if len(columns) > 1 else []

@app.callback(
    Output("live-graph", "figure"),
    [
        Input("x-axis-dropdown", "value"),
        Input("y-axis-dropdown", "value"),
        Input("table-dropdown", "value"),
        Input("interval-component", "n_intervals"),
    ],
)
def update_graph(x_axis, y_axes, table_name, n_intervals):
    if not (x_axis and y_axes and table_name):
        return go.Figure()  # Empty graph if inputs are invalid

    # Fetch updated data
    data = fetch_data(table_name)

    # Create traces for each selected Y-axis
    traces = []
    for y_axis in y_axes:
        traces.append(
            go.Scatter(
                x=data[x_axis],
                y=data[y_axis],
                mode="lines+markers",
                name=y_axis,
                line=dict(width=2),
            )
        )

    # Create the figure with a dark theme
    figure = {
        "data": traces,
        "layout": go.Layout(
            template="plotly_dark",
            paper_bgcolor="#1e2130",
            plot_bgcolor="#2b2f3a",
            title=f"Live Data Streaming: {x_axis} vs {', '.join(y_axes)}",
            xaxis=dict(title=x_axis, color="white", gridcolor="#3e4558"),
            yaxis=dict(title="Values", color="white", gridcolor="#3e4558"),
            font=dict(color="white"),
            margin=dict(l=40, r=40, t=40, b=40),
            hovermode="closest",
        ),
    }
    return figure

# Run the app
if __name__ == "__main__":
    app.run_server(debug=True, host="0.0.0.0", port=8050)
