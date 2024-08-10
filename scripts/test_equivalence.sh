#!/bin/bash

# Function to generate a random character
generate_random_char() {
    # Generate a random character (A-Z, a-z)
    # echo $(tr -dc 'A-Za-z' < /dev/urandom | head -c 1)
    echo $((RANDOM % 255))
}

# Function to generate a random unsigned short (0-65535)
generate_random_unsigned_short() {
    # Generate a random number between 0 and 65535
    echo $((RANDOM % 65536))
}

# Check if the user provided an argument for the array size
if [ $# -ne 1 ]; then
    echo "Usage: $0 <size of array>"
    exit 1
fi

# Size of the array
array_size=$1

# Declare an array to hold the pairs
declare -a pairs

# Generate random pairs of char and unsigned short values
for ((i = 0; i < array_size; i++)); do
    char=$(generate_random_char)
    ushort=$(generate_random_unsigned_short)
    pairs+=("($char, $ushort)")
done

echo "Initialization done!"

# Compile the program that contains optimized implementation of the CRC algorithm
gcc syrmia_crc_optimized.c

# Compile the program that contains unoptimized implementation of the CRC algorithm
# gcc syrmia_crc_unoptimized.c

# Uncomment the code below to test IR level CRC optimization on unoptimized implementation of the CRC algorithm
# clang -O0 -Xclang -disable-O0-optnone -S -emit-llvm syrmia_crc_unoptimized.c -march="x86-64"  -o unopt.ll
# ~/LLVM/build/bin/opt -S -use-option-one -passes=crc-recognition unopt.ll -o unopt.ll
# clang-15 unopt.ll -opaque-pointers -o unopt

# Start time
start_time=$(date +%s)

# Print the generated array
echo "Generated pairs:"
for pair in "${pairs[@]}"; do
    # Remove the parentheses and quotes
    clean_pair=${pair:1:-1}
    clean_pair=${clean_pair//\'/}

    # Extract the first element (character before the comma)
    first_element=${clean_pair%%,*}

    # Extract the second element (number after the comma and space)
    second_element=${clean_pair##*, }
    
    # echo "$pair"
    # echo "-> ($first_element, $second_element)"
    # ./a.out $first_element $second_element
    ./unopt $first_element $second_element
done

# End time
end_time=$(date +%s)

echo "$start_time >> $end_time"

# Calculate elapsed time
elapsed_time=$((end_time - start_time))

# Display the elapsed time
echo "Elapsed time between the first and last CRC algorithm call: $elapsed_time seconds"


