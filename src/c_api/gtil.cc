/*!
 * Copyright (c) 2023 by Contributors
 * \file gtil.cc
 * \author Hyunsu Cho
 * \brief C API for functions for GTIL
 */

#include <cstddef>
#include <cstdint>
#include <memory>

#include <treelite/c_api.h>
#include <treelite/c_api_error.h>
#include <treelite/gtil.h>
#include <treelite/logging.h>

#include "./c_api_utils.h"

int TreeliteGTILParseConfig(char const* config_json, TreeliteGTILConfigHandle* out) {
  API_BEGIN();
  auto parsed_config = std::make_unique<treelite::gtil::Configuration>(config_json);
  *out = static_cast<TreeliteGTILConfigHandle>(parsed_config.release());
  API_END();
}

int TreeliteGTILDeleteConfig(TreeliteGTILConfigHandle handle) {
  API_BEGIN();
  delete static_cast<treelite::gtil::Configuration*>(handle);
  API_END();
}

int TreeliteGTILGetOutputShape(TreeliteModelHandle model, std::uint64_t num_row,
    TreeliteGTILConfigHandle config, std::uint64_t const** out, std::uint64_t* out_ndim) {
  API_BEGIN();
  auto const* model_ = static_cast<treelite::Model const*>(model);
  auto const* config_ = static_cast<treelite::gtil::Configuration const*>(config);
  auto& shape = treelite::c_api::ReturnValueStore::Get()->ret_uint64_vec;
  shape = treelite::gtil::GetOutputShape(*model_, num_row, *config_);
  *out = shape.data();
  *out_ndim = shape.size();
  API_END();
}

int TreeliteGTILPredict(TreeliteModelHandle model, void const* input, char const* input_type,
    std::uint64_t num_row, void* output, TreeliteGTILConfigHandle config) {
  API_BEGIN();
  auto const* model_ = static_cast<treelite::Model const*>(model);
  auto const* config_ = static_cast<treelite::gtil::Configuration const*>(config);
  std::string input_type_str = std::string(input_type);
  if (input_type_str == "float32") {
    treelite::gtil::Predict(
        *model_, static_cast<float const*>(input), num_row, static_cast<float*>(output), *config_);
  } else if (input_type_str == "float64") {
    treelite::gtil::Predict(*model_, static_cast<double const*>(input), num_row,
        static_cast<double*>(output), *config_);
  } else {
    TREELITE_LOG(FATAL) << "Unexpected type spec: " << input_type_str;
  }
  API_END();
}

int TreeliteGTILPredictSparse(TreeliteModelHandle model, void const* data, char const* input_type,
    std::uint64_t const* col_ind, std::uint64_t const* row_ptr, std::uint64_t num_row, void* output,
    TreeliteGTILConfigHandle config) {
  API_BEGIN();
  auto const* model_ = static_cast<treelite::Model const*>(model);
  auto const* config_ = static_cast<treelite::gtil::Configuration const*>(config);
  std::string input_type_str = std::string(input_type);
  if (input_type_str == "float32") {
    treelite::gtil::PredictSparse(*model_, static_cast<float const*>(data), col_ind, row_ptr,
        num_row, static_cast<float*>(output), *config_);
  } else if (input_type_str == "float64") {
    treelite::gtil::PredictSparse(*model_, static_cast<double const*>(data), col_ind, row_ptr,
        num_row, static_cast<double*>(output), *config_);
  } else {
    TREELITE_LOG(FATAL) << "Unexpected type spec: " << input_type_str;
  }
  API_END();
}
