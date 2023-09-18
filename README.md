# MultiThreaded-Image-Filtering

his project applies filters to images using C++. The filters include smoothing, sepia, washed-out, and adding a cross. The program processes BMP image files using both single and multiple threads, saving the modified image as a BMP file.

## Project Structure
- **main.cpp**: The main source code file containing the image filtering logic and multi-threading implementation.
- **Makefile**: Makefile for compiling the project.
- **build/**: Directory to store compiled object files and executable.

## Usage
1. Ensure you have a BMP image file (e.g., input.bmp) in the same directory as the executable.
2. Run the executable ImageFilters.out with the input BMP image file as an argument:
   ```bash
   ./ImageFilters.out input.bmp
   ```
3. The program will apply various filters to the input image and generate an output BMP image named output.bmp.
