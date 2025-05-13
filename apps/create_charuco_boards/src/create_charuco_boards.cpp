#include <filesystem>
#include <iomanip>
#include <stdio.h>

#include <opencv2/opencv.hpp>

#include "McCalib.hpp"
#include "utilities.hpp"


void saveBoards(const std::vector<cv::Mat> boards,
                float margin_ratio = 0.05f,
                const cv::Scalar& margin_color = cv::Scalar(255, 255, 255)) {
  std::filesystem::path charuco_boards_path = "charuco_boards";
  std::string char_name = "charuco_board_";
  if (!std::filesystem::exists(charuco_boards_path)) {
    std::filesystem::create_directories(charuco_boards_path);
  }

  for (std::size_t board_idx = 0u; board_idx < boards.size(); ++board_idx) {
    // Get the original board
    const cv::Mat& board = boards[board_idx];

    // Print board information for debugging
    std::cout << "Board " << board_idx << " info:" << std::endl;
    std::cout << "  Dimensions: " << board.cols << "x" << board.rows << std::endl;
    std::cout << "  Channels: " << board.channels() << std::endl;
    std::cout << "  Type: " << board.type() << std::endl;

    // Calculate absolute margin size in pixels
    // This ensures the margin is added to the outside of the board
    // without affecting the size of the squares
    int margin_x = static_cast<int>(board.cols * margin_ratio);
    int margin_y = static_cast<int>(board.rows * margin_ratio);

    // Calculate new dimensions with margins
    int new_width = board.cols + 2 * margin_x;
    int new_height = board.rows + 2 * margin_y;

    // Check if the board is grayscale and convert to color if needed
    cv::Mat color_board;
    if (board.channels() == 1) {
        cv::cvtColor(board, color_board, cv::COLOR_GRAY2BGR);
    } else {
        color_board = board;
    }

    // Create a new image with margins using the specified color
    cv::Mat board_with_margin(new_height, new_width, CV_8UC3, margin_color);

    // Copy the color board into the center of the new image
    // This preserves the original board's dimensions and square sizes
    color_board.copyTo(board_with_margin(cv::Rect(margin_x, margin_y, board.cols, board.rows)));

    // Prepare file names
    std::ostringstream ss;
    ss << std::setw(3) << std::setfill('0') << board_idx;
    std::string s1 = ss.str();
    std::string base_name = char_name + s1;

    // Print final image information for debugging
    std::cout << "Final image with margin info:" << std::endl;
    std::cout << "  Dimensions: " << board_with_margin.cols << "x" << board_with_margin.rows << std::endl;
    std::cout << "  Channels: " << board_with_margin.channels() << std::endl;
    std::cout << "  Type: " << board_with_margin.type() << std::endl;

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
  float margin_ratio = 0.05f; // Default margin ratio
  std::vector<int> number_x_square_per_board, number_y_square_per_board;
  std::vector<int> resolution_x_per_board, resolution_y_per_board;
  std::vector<double> square_size_per_board;
  cv::Scalar margin_color(255, 255, 255); // Default margin color (white)

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

  // Read margin parameters if they exist
  if (!fs["margin_ratio"].empty()) {
    fs["margin_ratio"] >> margin_ratio;
    std::cout << "Using margin ratio: " << margin_ratio << std::endl;
  } else {
    std::cout << "Using default margin ratio: " << margin_ratio << std::endl;
  }

  // Read margin color if it exists
  if (!fs["margin_color_r"].empty() && !fs["margin_color_g"].empty() && !fs["margin_color_b"].empty()) {
    int r, g, b;
    fs["margin_color_r"] >> r;
    fs["margin_color_g"] >> g;
    fs["margin_color_b"] >> b;
    margin_color = cv::Scalar(b, g, r); // BGR format for OpenCV
    std::cout << "Using margin color: BGR(" << b << ", " << g << ", " << r << ")" << std::endl;
  } else {
    std::cout << "Using default margin color: White" << std::endl;
  }

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
  saveBoards(charuco_boards_images, margin_ratio, margin_color);

  return 0;
}
