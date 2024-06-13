#!/bin/bash

# Check if at least two arguments are provided
if [[ $# -lt 2 ]]; then
    echo "Usage: $0 <old_log_fn> <new_log_fn>"
    exit 1
fi

old_log_fn=$1
new_log_fn=$2

# Check if files exist
if [[ ! -f $old_log_fn ]]; then
    echo "Error: File '$old_log_fn' not found."
    exit 2
fi
if [[ ! -f $new_log_fn ]]; then
    echo "Error: File '$new_log_fn' not found."
    exit 3
fi

# Extract test commands and performance numbers from old logs
while read -r line; do
    if [[ $line == Start* ]]; then
        test_command=$(echo "$line" | awk '{print $3}')
        test_commands+=("$test_command")
    elif [[ $line == *"Collected"* ]]; then
        performance_number=$(echo "$line" | grep -oP 'Collected : \K\d+')
        old_numbers+=("$performance_number")
    fi
done < $old_log_fn

# Extract test commands and performance numbers from new logs
while read -r line; do
    if [[ $line == Start* ]]; then
        test_command=$(echo "$line" | awk '{print $3}')
        #echo "### test_command=$test_command"
        new_test_commands+=("$test_command")
    elif [[ $line == *"Collected"* ]]; then
        performance_number=$(echo "$line" | grep -oP 'Collected : \K\d+')
        #echo "### performance_num=$performance_number"
        new_numbers+=("$performance_number")
    fi
done < $new_log_fn

# Compare the performance numbers based on matching test commands
for ((i=0; i<${#test_commands[@]}; i++)); do
    test_command=${test_commands[$i]}
    old_number=${old_numbers[$i]}
    new_index=-1
    for ((j=0; j<${#new_test_commands[@]}; j++)); do
        if [[ ${new_test_commands[$j]} == "$test_command" ]]; then
            new_index=$j
            break
        fi
    done
    if ((new_index == -1)); then
        echo "Test command '$test_command' not found in new logs"
        continue
    fi
    new_number=${new_numbers[$new_index]}
    if [[ -z "$new_number" ]]; then
        echo "Error: Performance number does not exist for test command '$test_command' in new logs"
        continue
    fi

    percentage_difference=$(bc <<< "scale=6; ($old_number - $new_number) / $old_number * 100" | sed 's/^\./0./' | sed 's/^-\./-0./')

    echo "- $test_command"
    if ((new_number < old_number)); then
        if (( $(bc <<< "$percentage_difference < 0.15") )); then
            echo "Performance is similar but better: $new_number < $old_number (Percentage difference: $percentage_difference%)"
        else
            echo "Performance is BETTER: $new_number < $old_number (Percentage difference: $percentage_difference%)"
        fi
    elif ((new_number > old_number)); then
        if (( $(bc <<< "$percentage_difference > -0.15") )); then
            echo "Performance is similar but worse: $new_number > $old_number (Percentage difference: $percentage_difference%)"
        else
            echo "Performance is WORSE: $new_number > $old_number (Percentage difference: $percentage_difference%)"
        fi
    else
        echo "Performance is the same: $new_number = $old_number"
    fi
done
