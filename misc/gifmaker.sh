#!/bin/bash

# Directory containing your video files
INPUT_DIR="$1"

# Directory for output GIFs (will be created if it doesn't exist)
OUTPUT_DIR="${INPUT_DIR}/gifs"

# Check if input directory was provided
if [ -z "$INPUT_DIR" ]; then
    echo "Usage: $0 /path/to/video/folder"
    exit 1
fi

# Check if input directory exists
if [ ! -d "$INPUT_DIR" ]; then
    echo "Error: Directory '$INPUT_DIR' not found"
    exit 1
fi

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Counter for processed files
count=0

# Loop through common video file extensions
for video in "$INPUT_DIR"/*.{mp4,mov,avi,mkv,m4v,MP4,MOV,AVI,MKV,M4V}; do
    # Check if file exists (handles case where no files match)
    [ -e "$video" ] || continue
    
    # Get filename without extension
    filename=$(basename "$video")
    name="${filename%.*}"
    
    echo "Converting: $filename"
    
    # Convert to 2-second GIF
    # -t 2: duration of 2 seconds
    # -vf: video filters for quality and size
    # fps=10: 10 frames per second (adjust for quality vs file size)
    # scale=128:-1: width of 128px, height auto-scaled (adjust as needed)
    ffmpeg -i "$video" -t 2 -vf "fps=10,scale=128:-1:flags=lanczos,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" -loop 0 "$OUTPUT_DIR/${name}.gif" -y
    
    ((count++))
done

echo "Done! Converted $count videos to GIFs in: $OUTPUT_DIR"