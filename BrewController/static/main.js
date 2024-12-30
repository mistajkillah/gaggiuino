document.addEventListener('DOMContentLoaded', () => {
    const chartDom = document.getElementById('BrewViewChart');
    const myChart = echarts.init(chartDom, 'dark'); // Using dark theme for modern look

    let currentColumns = [
        { label: 'Pressure', key: 'smoothedPressure', yAxisIndex: 0 },
        { label: 'Temperature', key: 'temperature', yAxisIndex: 1 },
        { label: 'Target Pressure', key: 'target_pressure', yAxisIndex: 0 },
        { label: 'Target Temperature', key: 'target_temperature', yAxisIndex: 1 }
    ];

    let availableColumns = [];

    function convertToTime(iterations, timeIntervalMs) {
        return iterations.map(iteration => (iteration * timeIntervalMs) / 1000);
    }

    function updateChart(responseData) {
        const timeLabels = convertToTime(responseData.x, time_interval_ms);
        const series = currentColumns.map(column => ({
            name: column.label,
            type: 'line',
            data: responseData[column.key],
            yAxisIndex: column.yAxisIndex
        }));

        const option = {
            backgroundColor: '#1e1e1e', // Dark theme background
            tooltip: {
                trigger: 'axis',
                textStyle: {
                    color: '#d4d4d4'
                }
            },
            legend: {
                data: currentColumns.map(column => column.label),
                textStyle: {
                    color: '#d4d4d4' // Modern light text color for dark theme
                }
            },
            xAxis: {
                type: 'category',
                data: timeLabels,
                name: 'Time (seconds)',
                axisPointer: {
                    type: 'shadow'
                },
                axisLine: {
                    lineStyle: {
                        color: '#d4d4d4'
                    }
                },
                axisLabel: {
                    color: '#d4d4d4'
                }
            },
            yAxis: [
                {
                    type: 'value',
                    name: 'Pressure (Bar)',
                    min: pres_y_min,
                    max: pres_y_max,
                    position: 'right',
                    axisLine: {
                        lineStyle: {
                            color: '#d4d4d4'
                        }
                    },
                    axisLabel: {
                        color: '#d4d4d4'
                    }
                },
                {
                    type: 'value',
                    name: 'Temperature (°C)',
                    min: temp_y_min,
                    max: temp_y_max,
                    position: 'left',
                    axisLine: {
                        lineStyle: {
                            color: '#d4d4d4'
                        }
                    },
                    axisLabel: {
                        color: '#d4d4d4'
                    }
                }
            ],
            dataZoom: [
                {
                    type: 'slider',
                    xAxisIndex: 0,
                    start: 0,
                    end: 100,
                    bottom: 10,
                    filterMode: 'empty',
                    textStyle: {
                        color: '#d4d4d4'
                    }
                },
                {
                    type: 'inside',
                    xAxisIndex: 0,
                    start: 0,
                    end: 100,
                    filterMode: 'empty'
                },
                {
                    type: 'slider',
                    yAxisIndex: 0,
                    start: 0,
                    end: 100,
                    left: '90%',
                    textStyle: {
                        color: '#d4d4d4'
                    }
                },
                {
                    type: 'slider',
                    yAxisIndex: 1,
                    start: 0,
                    end: 100,
                    right: '90%',
                    textStyle: {
                        color: '#d4d4d4'
                    }
                }
            ],
            series
        };

        myChart.setOption(option);
    }

    function fetchAvailableColumns() {
        fetch('/columns')
            .then(response => response.json())
            .then(columns => {
                availableColumns = columns.map(col => ({ label: col, key: col }));

                const addColumnDropdown = document.getElementById('addColumnDropdown');
                addColumnDropdown.innerHTML = ''; // Clear existing options

                availableColumns.forEach(column => {
                    const option = document.createElement('option');
                    option.value = column.key;
                    option.textContent = column.label;
                    addColumnDropdown.appendChild(option);
                });
            })
            .catch(error => console.error('Error fetching available columns:', error));
    }

    function fetchAndRenderChart() {
        const selectedKeys = currentColumns.map(col => col.key).join(',');
        fetch(`/default-data?columns=${selectedKeys}`)
            .then(response => response.json())
            .then(responseData => {
                console.log('Fetched Data:', responseData);
                console.log('Current Columns:', currentColumns);
                updateChart(responseData);
            })
            .catch(error => console.error('Error loading default data:', error));
    }
    // function fetchAndRenderChart() {
    //     const selectedKeys = currentColumns.map(col => col.key).join(',');
    //     fetch(`/default-data?columns=${selectedKeys}`)
    //         .then(response => response.json())
    //         .then(responseData => {
    //             console.log('Fetched Data:', responseData);
    
    //             // Dynamically update yAxisIndex for each column
    //             currentColumns.forEach(column => {
    //                 const yAxisIndex = assignYAxisIndex(column, responseData);
    //                 column.yAxisIndex = yAxisIndex; // Update the column's axis assignment
    //             });
    
    //             console.log('Updated Columns with Axis:', currentColumns);
    //             updateChart(responseData); // Render the updated chart
    //         })
    //         .catch(error => console.error('Error loading default data:', error));
    // }
    
    // function assignYAxisIndex(column, responseData) {
    //     const data = responseData[column.key] || []; // Handle empty data initially
    //     if (!data.length) return 0; // Default to Pressure axis if no data

    //     const minDataValue = Math.min(...data);
    //     const maxDataValue = Math.max(...data);

    //     if (minDataValue < pres_y_min || maxDataValue > pres_y_max) {
    //         return 1; // Temperature axis
    //     } else {
    //         return 0; // Pressure axis
    //     }
    // }
function assignYAxisIndex(column, responseData) {
    const data = responseData[column.key] || []; // Use real data if available
    if (!data.length) {
        console.warn(`No data found for column: ${column.label}, defaulting to Pressure axis.`);
        return 1; // Default to Pressure axis if no data
    }

    const minDataValue = Math.min(...data);
    const maxDataValue = Math.max(...data);

    // Determine the axis based on the data range
    if (minDataValue < pres_y_min || maxDataValue > pres_y_max) {
        return 1; // Temperature axis
    } else {
        return 0; // Pressure axis
    }
}

    document.getElementById('addColumn').addEventListener('click', () => {
        const addColumnDropdown = document.getElementById('addColumnDropdown');
        const selectedKey = addColumnDropdown.value;

        if (selectedKey && !currentColumns.some(col => col.key === selectedKey)) {
            const columnToAdd = availableColumns.find(col => col.key === selectedKey);

            if (columnToAdd) {
                const yAxisIndex = assignYAxisIndex(columnToAdd, {
                    [columnToAdd.key]: [] // Placeholder if data is not yet fetched
                });

                if (!isNaN(yAxisIndex) && (yAxisIndex === 0 || yAxisIndex === 1)) {
                    currentColumns.push({
                        label: columnToAdd.label,
                        key: columnToAdd.key,
                        yAxisIndex
                    });
                    console.log('Added column:', columnToAdd);
                    fetchAndRenderChart();
                } else {
                    alert('Invalid Y Axis Index! Must be 0 or 1.');
                }
            } else {
                alert('Selected column not found in available columns!');
            }
        } else {
            alert('Column already exists or invalid input!');
        }
    });

    fetchAvailableColumns();
    fetchAndRenderChart();

    // Handle window resize for responsiveness
    window.addEventListener('resize', () => {
        myChart.resize();
    });
});
