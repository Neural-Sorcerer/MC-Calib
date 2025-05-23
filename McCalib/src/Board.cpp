
#include <iostream>
#include <numeric>
#include <stdio.h>

#include "opencv2/core/core.hpp"
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/opencv.hpp>

#include "Board.hpp"
#include "Frame.hpp"
#include "logger.h"

namespace McCalib {

/**
 * @brief Initialize Board object
 *
 * @param config_path path to the configuration file
 * @param board_idx index of the board
 */
Board::Board(const std::filesystem::path &config_path,
             const int board_idx,
             const cv::Ptr<cv::aruco::CharucoBoard> charuco_board) {
  // Read the parameters from the configuration file
  std::vector<int> number_x_square_per_board, number_y_square_per_board;
  std::vector<double> square_size_per_board;
  std::vector<int> boards_index;
  int nb_board;
  cv::FileStorage fs; // FileStorage object to read calibration params from file
  const bool is_file_available = std::filesystem::exists(config_path) &&
                                 config_path.has_filename() &&
                                 config_path.extension() == ".yml";
  
  if (!is_file_available) {
    LOG_FATAL << "Config path '" << config_path << "' doesn't exist.";
    return;
  }

  fs.open(config_path, cv::FileStorage::READ);
  fs["number_board"] >> nb_board;
  fs["number_x_square_per_board"] >> number_x_square_per_board;
  fs["number_y_square_per_board"] >> number_y_square_per_board;
  fs["square_size_per_board"] >> square_size_per_board;
  fs["boards_index"] >> boards_index;

  if (boards_index.size() == 0) {
    boards_index.resize(nb_board);
    std::iota(boards_index.begin(), boards_index.end(), 0);
  }

  if (square_size_per_board.size() == 0) {
    fs["number_x_square"] >> nb_x_square_;
    fs["number_y_square"] >> nb_y_square_;
    fs["length_square"] >> length_square_;
    fs["length_marker"] >> length_marker_;
    fs["resolution_x"] >> res_x_;
    fs["resolution_y"] >> res_y_;
    fs["square_size"] >> square_size_;
  } else {
    nb_x_square_ = number_x_square_per_board[boards_index[board_idx]];
    nb_y_square_ = number_y_square_per_board[boards_index[board_idx]];
    square_size_ = square_size_per_board[boards_index[board_idx]];
  }

  fs.release(); // close the input file

  nb_pts_ = (nb_x_square_ - 1) * (nb_y_square_ - 1);

  // Prepare the 3D pts of the boards
  pts_3d_.reserve(nb_pts_);
  for (int y = 0; y < nb_y_square_ - 1; y++) {
    for (int x = 0; x < nb_x_square_ - 1; x++) {
      pts_3d_.emplace_back(x * square_size_, y * square_size_, 0);
    }
  }

  charuco_board_ = charuco_board;

  board_id_ = board_idx;

  // initialize color of the board
  cv::RNG my_rng(cv::getTickCount());
  color_ = {my_rng.uniform(0, 255), my_rng.uniform(0, 255),
            my_rng.uniform(0, 255)};
}

/**
 * @brief Add a new board observation to the data structure
 *
 * The indexing of the board is incremental (not the frame index).
 *
 * @param new_board the pointer to the new board
 */
void Board::insertNewBoard(const std::shared_ptr<BoardObs> new_board) {
  board_observations_[board_observations_.size()] = new_board;
}

/**
 * @brief Insert a new frame in the data structure
 *
 * The indexing is the frame index.
 *
 * @param new_frame the pointer of the new frame
 */
void Board::insertNewFrame(const std::shared_ptr<Frame> new_frame) {
  frames_[new_frame->frame_idx_] = new_frame;
}

} // namespace McCalib