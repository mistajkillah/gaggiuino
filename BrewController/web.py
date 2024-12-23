import dash
from dash import dcc, html, Input, Output
import pandas as pd
from sqlalchemy import create_engine
from sqlalchemy.sql import text
import plotly.graph_objs as go

# Database connection
DATABASE_URL = "sqlite:////tmp/mydb.sqlite"  # Updated database URL

# Create database engine
engine = create_engine(DATABASE_URL, connect_args={"check_same_thread": False})

# Enable PRAGMA read_uncommitted
def enable_read_uncommitted(connection):
    connection.execute(text("PRAGMA read_uncommitted = true;"))

# Function to fetch table names dynamically
def fetch_table_names():
    with engine.connect() as connection:
        enable_read_uncommitted(connection)  # Enable read_uncommitted mode
        query = "SELECT name FROM sqlite_master WHERE type='table';"
        tables = pd.read_sql(query, connection)
    return tables["name"].tolist()  # Return table names as a list

# Function to fetch column names dynamically for a specific table
def fetch_schema(table_name):
    with engine.connect() as connection:
        enable_read_uncommitted(connection)  # Enable read_uncommitted mode
        query = f"PRAGMA table_info({table_name});"
        schema = pd.read_sql(query, connection)
    return schema["name"].tolist()  # Return column names as a list

# Function to fetch data from a specific table
def fetch_data(table_name):
    with engine.connect() as connection:
        enable_read_uncommitted(connection)  # Enable read_uncommitted mode
        query = f"SELECT * FROM {table_name};"
        df = pd.read_sql(query, connection)
    return df

# Fetch initial table names
tables = fetch_table_names()

# Initialize Dash app
app = dash.Dash(__name__)
app.title = "Dynamic Streaming Data Visualization"

# Layout
app.layout = html.Div(
    style={
        "backgroundColor": "#1a1a1a",
        "color": "white",
        "font-family": "Arial, sans-serif",
    },
    children=[
        html.H1(
            "Dynamic Streaming Data Visualization",
            style={"text-align": "center", "color": "white"},
        ),
        # Dropdown for selecting table
        html.Div([
            html.Label("Select Table:", style={"color": "white"}),
            dcc.Dropdown(
                id="table-dropdown",
                options=[{"label": table, "value": table} for table in tables],
                placeholder="Select a table",
                style={"backgroundColor": "#333", "color": "white"},
            ),
        ], style={'width': '48%', 'display': 'inline-block'}),
        
        # Dropdowns for selecting parameters (X and Y)
        html.Div([
            html.Label("Select X-axis:", style={"color": "white"}),
            dcc.Dropdown(
                id="x-axis-dropdown",
                placeholder="Select X-axis",
                clearable=False,
                style={"backgroundColor": "#333", "color": "white"},
            ),
            html.Label("Select Y-axis (multiple):", style={"color": "white", "marginTop": "10px"}),
            dcc.Dropdown(
                id="y-axis-dropdown",
                placeholder="Select Y-axis",
                clearable=False,
                multi=True,
                style={"backgroundColor": "#333", "color": "white"},
            ),
        ], style={'width': '48%', 'display': 'inline-block', 'marginLeft': '5%'}),

        # Graph for real-time visualization
        dcc.Graph(
            id="live-graph",
            style={"backgroundColor": "#1a1a1a"},
        ),

        # Interval component for streaming updates
        dcc.Interval(
            id="interval-component",
            interval=5000,  # Update every 5 seconds
            n_intervals=0,
        ),
    ]
)

# Callback to update column dropdowns when table is selected
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

# Callback to update graph when parameters are selected or interval triggers
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
            )
        )

    # Create the figure with a dark theme
    figure = {
        "data": traces,
        "layout": go.Layout(
            template="plotly_dark",  # Apply dark theme
            title=f"Live Data Streaming: {x_axis} vs {', '.join(y_axes)}",
            xaxis=dict(title=x_axis),
            yaxis=dict(title="Values"),
            margin=dict(l=40, r=40, t=40, b=40),
            hovermode="closest",
        ),
    }
    return figure

# Run the app on all network interfaces
if __name__ == "__main__":
    app.run_server(debug=True, host="0.0.0.0", port=8050)
