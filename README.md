# Sudoku Image Processor

This is a program designed to handle and process Sudoku puzzles encoded as BMP images. It offers various functionalities, including parsing JSON data, mirroring Sudoku squares, validating the puzzle, and solving Sudoku puzzles, all while ensuring that error handling and file operations are optimized.

## General Overview

Throughout the implementation process, we focused on enhancing the program's efficiency. We optimized checks, file operations, and error handling to ensure that the program works seamlessly. This guarantees the program's reliability and robust functionality. If errors occur, the program is designed to provide informative error messages and gracefully halt execution.

## BMP Structure

Working with images is a crucial aspect of this program, and to achieve this, we've created a custom BMP structure that captures all the necessary information:

- `bmp_fileheader` (as defined in the BMP file structure)
- `bmp_infoheader` (also defined in the BMP BMP file structure)
- A pixel matrix (a double pointer to a pixel structure)

The `pixel` structure, which represents individual pixels in the image, efficiently stores three unsigned char integers and occupies exactly 3 bytes. This structure allows us to process images pixel by pixel.

## File and Task Configuration

To ensure a seamless workflow, the program automatically extracts the necessary information from the input data. Specifically, we extract the name of the input file from the command-line arguments. From this file name, we determine the board number and task number.

This enables us to work with the input files according to their designated task. The program will select the appropriate file extension and file opening mode based on the task, ensuring compatibility and correct processing.

## Task 1 - Parsing JSON

For the first task, which involves parsing JSON data, we've incorporated the CJSON library, as recommended by the assignment instructions. This library provides an effective solution for handling JSON data. We've read the contents of the JSON file into a string buffer and then extracted the data piece by piece, based on the required keys. The extracted data is then stored in our custom BMP structure.

Once the JSON data has been successfully parsed and the BMP structure is populated, we can render the image. The rendering process involves writing the `bmp_fileheader` and `bmp_infoheader` structures to the output file, ensuring that the structure's size aligns with the data without any padding issues. Following this, we display the pixel matrix, taking care to maintain the BMP format, including the padding necessary for each row.

## Task 2 - Mirroring Squares

In the second task, our goal is to mirror each 7x7 square representing a digit within the Sudoku image. These squares, devoid of the gray or black margins, need to be mirrored by identifying the middle column as the axis of symmetry. We've developed a function to facilitate this operation.

The mirroring function is applied iteratively to each square, starting from the top-left corner and moving in increments of 8 pixels (both horizontally and vertically). Once the mirroring process is complete, we render the BMP structure as an image, just as we did in Task 1.

## Task 3 - Sudoku Validation

In Task 3, our focus is on validating the Sudoku puzzle. To do this, we first convert the image into an integer matrix with a size of 9x9, matching the Sudoku grid. Each cell in this matrix corresponds to a 7x7 square in the image, representing a Sudoku digit. This conversion is achieved by using predefined matrices that map each digit in a 7x7 representation. These matrices are like templates, where each element can be either '.' or 'X' based on the color information required.

We systematically overlay these template matrices onto each corresponding digit in the image. When a match is found, we assign the correct digit to the corresponding position in the integer matrix. If no matching template is found, it indicates an incorrect or incomplete digit, and an error message is generated.

After creating the integer matrix from the image, we validate the Sudoku puzzle by checking for unique values in each row, column, and 3x3 subgrid. Successful validation indicates that the puzzle is in a winning state, and the program returns a "Win" response. Otherwise, it is marked as "Lose."

For generating the JSON response, we make use of the CJSON library once again. We create a JSON object containing the game state and the board number. This information is organized and formatted according to the assignment's requirements and is then written to the output file.

## Task 4 - Reading and Solving Sudoku

In the fourth task, we focus on reading and solving Sudoku puzzles. We read image data from a binary file. The image is split into two main sections: the header structures (file header and info header) and the pixel matrix. Reading these sections involves various considerations, such as avoiding padding and seeking the correct positions within the file.

After reading the image, we convert it into an integer matrix, with each cell representing a digit in the Sudoku puzzle. Cells that were initially empty in the image are assigned the value 0. We then apply an algorithm to solve the Sudoku puzzle.

The algorithm works as follows:
1. Count the number of empty cells in the initial matrix.
2. While there are still empty cells:
   - For each row and each column, if only one empty cell remains, fill it with a unique value that hasn't been used in the same row or column.
3. Perform the above operations iteratively until all empty cells are filled. When a cell is filled, we mark it with a negative value to indicate that it's part of the solution.

The end result is a solved Sudoku puzzle represented as an integer matrix. This matrix is used to recreate the Sudoku image. The recreation involves mapping the integers back to their respective 7x7 representations and adding color information. Once the image has been reconstructed, it is displayed as an output.

## Bonus Task - Multiple Solutions

The bonus task addresses Sudoku puzzles that have multiple valid solutions. To solve these puzzles, we employ a backtracking algorithm. The algorithm follows these steps:
1. It checks if the puzzle is complete (i.e., it has reached a point outside the Sudoku grid).
2. If the puzzle is incomplete, the algorithm:
   - Locates the next empty cell, which is determined based on a lexicographic order (from top to bottom and left to right).
   - For that cell, it explores values from 1 to 9, subject to certain constraints. Specifically, it avoids using values that already appear in the same row, column, or 3x3 subgrid.
   - For each valid choice of a value for the cell, the algorithm recursively attempts to solve the puzzle.
   - If a solution is found, it returns 1, indicating a successful solution. If no solution is found, it returns 0.
3. If a solution is returned, the algorithm proceeds to reconstruct the image, reflecting the multiple possible solutions.

The reconstructed image may contain either the integers representing the possible values for cells or red 'X' marks to indicate ambiguity in those cells.
