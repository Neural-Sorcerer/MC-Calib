#include <filesystem>
#include <iomanip>
#include <stdio.h>

#include <opencv2/opencv.hpp>

#include "McCalib.hpp"
#include "utilities.hpp"


void saveBoards(const std::vector<cv::Mat> boards, float margin_ratio = 0.05f) {
  std::filesystem::path charuco_boards_path = "charuco_boards";
  std::string char_name = "charuco_board_";
  if (!std::filesystem::exists(charuco_boards_path)) {
    std::filesystem::create_directories(charuco_boards_path);
  }

  for (std::size_t board_idx = 0u; board_idx < boards.size(); ++board_idx) {
    // Save the markers with margin
    const cv::Mat& board = boards[board_idx];

    int margin_x = static_cast<int>(board.cols * margin_ratio);
    int margin_y = static_cast<int>(board.rows * margin_ratio);

    int new_width = board.cols + 2 * margin_x;
    int new_height = board.rows + 2 * margin_y;

    // Create a white image
    cv::Mat board_with_margin(new_height, new_width, board.type(), cv::Scalar(255, 255, 255));
    // Copy the board image into the center
    board.copyTo(board_with_margin(cv::Rect(margin_x, margin_y, board.cols, board.rows)));

    // Prepare file names
    std::ostringstream ss;
    ss << std::setw(3) << std::setfill('0') << board_idx;
    std::string s1 = ss.str();
    std::string base_name = char_name + s1;

    // Save as BMP
    std::filesystem::path save_path_bmp = charuco_boards_path / (base_name + ".bmp");
    cv::imwrite(save_path_bmp.string(), board_with_margin);
    std::cout << "Saved BMP: " << save_path_bmp << std::endl;

    // Save as PNG
    std::filesystem::path save_path_png = charuco_boards_path / (base_name + ".png");
    cv::imwrite(save_path_png.string(), board_with_margin);
    std::cout << "Saved PNG: " << save_path_png << std::endl;
  }
}

int main(int argc, char *argv[]) {

  if (argc != 2) {
    std::cout << "You need to provide the absolute path to the calibration file"
              << std::endl;
    return -1;
  }

  // Path of the configuration file
  std::filesystem::path pathInt = argv[1];

  // Read the parameters from the configuration file
  int num_x_square, num_y_square, res_x, res_y, NbBoard;
  float length_square, length_marker;
  std::vector<int> number_x_square_per_board, number_y_square_per_board;
  std::vector<int> resolution_x_per_board, resolution_y_per_board;
  std::vector<double> square_size_per_board;
  
  cv::FileStorage fs; // FileStorage to read calibration params from file
  const bool is_file_available = std::filesystem::exists(pathInt) &&
                                 pathInt.has_filename() &&
                                 pathInt.extension() == ".yml";
  if (!is_file_available) {
    std::cout << "Config path '" << pathInt << "' doesn't exist." << std::endl;
    return -1;
  }

  fs.open(pathInt, cv::FileStorage::READ);
  fs["number_x_square"] >> num_x_square;
  fs["number_y_square"] >> num_y_square;
  fs["length_square"] >> length_square;
  fs["length_marker"] >> length_marker;
  fs["resolution_x"] >> res_x;
  fs["resolution_y"] >> res_y;
  fs["number_board"] >> NbBoard;

  assert(NbBoard > 0);
  const unsigned int num_board = static_cast<unsigned int>(NbBoard);

  fs["square_size_per_board"] >> square_size_per_board;
  fs["number_x_square_per_board"] >> number_x_square_per_board;
  fs["number_y_square_per_board"] >> number_y_square_per_board;
  fs["resolution_x_per_board"] >> resolution_x_per_board;
  fs["resolution_y_per_board"] >> resolution_y_per_board;
  
  fs.release(); // close the input file

  // Check if multi-size boards are used or not
  if (square_size_per_board.size() == 0) {
    for (std::size_t i = 0; i < num_board; i++) {
      number_x_square_per_board.push_back(num_x_square);
      number_y_square_per_board.push_back(num_y_square);
      resolution_x_per_board.push_back(res_x);
      resolution_y_per_board.push_back(res_y);
    }
  }

  const std::vector<cv::Mat> charuco_boards_images =
      McCalib::createCharucoBoardsImages(
        num_board,
        number_x_square_per_board,
        number_y_square_per_board,
        length_square,
        length_marker,
        resolution_x_per_board,
        resolution_y_per_board
      );
  saveBoards(charuco_boards_images);

  return 0;
}
