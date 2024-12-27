#!/bin/bash

# GPIO pin assignments
BOILER_PIN=5
VALVE_PIN=6
PUMP_PIN=26
BREW_PIN=13
STEAM_PIN=19

# Initialize GPIO pins
gpio_init() {
    raspi-gpio set $BOILER_PIN op pd dl
    raspi-gpio set $VALVE_PIN op pd dl
    raspi-gpio set $PUMP_PIN op pd dl
    raspi-gpio set $BREW_PIN ip pu
    raspi-gpio set $STEAM_PIN ip pu
}

# Reset GPIO pins to default states
gpio_reset() {
    raspi-gpio set $BOILER_PIN op pd dl
    raspi-gpio set $VALVE_PIN op pd dl
    raspi-gpio set $PUMP_PIN op pd dl
    echo "All GPIO pins reset to default states."
}

# Set GPIO pin state
set_gpio() {
    local pin=$1
    local state=$2
    if [[ $state == "on" ]]; then
        raspi-gpio set $pin op pd dh
    elif [[ $state == "off" ]]; then
        raspi-gpio set $pin op pd dl
    else
        echo "Invalid state: $state (use 'on' or 'off')"
        exit 1
    fi
}

# Get GPIO pin state
get_gpio() {
    local pin=$1
    raspi-gpio get $pin | grep -oP "level=\K(\w+)"
}

# Get input pin state
get_input_pin() {
    local pin=$1
    state=$(raspi-gpio get $pin | grep -oP "level=\K(\w+)")
    echo "$state"
}

# Control logic
brewCtrl() {
    local component=$1
    local action=$2

    case $component in
        boiler)
            pin=$BOILER_PIN
            ;;
        valve)
            pin=$VALVE_PIN
            ;;
        pump)
            pin=$PUMP_PIN
            ;;
        brew)
            state=$(get_input_pin $BREW_PIN)
            echo "Brew pin is currently $state"
            return
            ;;
        steam)
            state=$(get_input_pin $STEAM_PIN)
            echo "Steam pin is currently $state"
            return
            ;;
        *)
            echo "Unknown component: $component (use 'boiler', 'valve', 'pump', 'brew', or 'steam')"
            exit 1
            ;;
    esac

    if [[ -z $action ]]; then
        # Get current state if no action provided
        current_state=$(get_gpio $pin)
        echo "$component is currently $current_state"
    else
        # Set the state of the component
        set_gpio $pin $action
        echo "$component turned $action"
    fi
}

# Main script execution
if [[ $1 == "init" ]]; then
    gpio_init
    echo "GPIO pins initialized."
elif [[ $1 == "reset" ]]; then
    gpio_reset
elif [[ $# -ge 1 ]]; then
    brewCtrl $1 $2
else
    echo "Usage: $0 (init | reset | boiler|valve|pump|brew|steam) [on|off]"
    exit 1
fi
