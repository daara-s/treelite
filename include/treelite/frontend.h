/*!
 * Copyright (c) 2017-2021 by Contributors
 * \file frontend.h
 * \brief Collection of front-end methods to load or construct ensemble model
 * \author Hyunsu Cho
 */
#ifndef TREELITE_FRONTEND_H_
#define TREELITE_FRONTEND_H_

#include <treelite/base.h>
#include <string>
#include <memory>
#include <vector>
#include <cstdint>

namespace treelite {

class Model;  // forward declaration

namespace frontend {

//--------------------------------------------------------------------------
// model loader interface: read from the disk
//--------------------------------------------------------------------------
/*!
 * \brief load a model file generated by LightGBM (Microsoft/LightGBM). The
 *        model file must contain a decision tree ensemble.
 * \param filename name of model file
 * \return loaded model
 */
std::unique_ptr<treelite::Model> LoadLightGBMModel(const char *filename);
/*!
 * \brief Load a LightGBM model from a string. The string should be created with the
 *        model_to_string() method in LightGBM.
 * \param model_str the model string
 * \return loaded model
 */
std::unique_ptr<treelite::Model> LoadLightGBMModelFromString(const char* model_str);
/*!
 * \brief load a model file generated by XGBoost (dmlc/xgboost). The model file
 *        must contain a decision tree ensemble.
 * \param filename name of model file
 * \return loaded model
 */
std::unique_ptr<treelite::Model> LoadXGBoostModel(const char* filename);
/*!
 * \brief load an XGBoost model from a memory buffer.
 * \param buf memory buffer
 * \param len size of memory buffer
 * \return loaded model
 */
std::unique_ptr<treelite::Model> LoadXGBoostModel(const void* buf, std::size_t len);
/*!
 * \brief load a model file generated by XGBoost (dmlc/xgboost). The model file
 *        must contain a decision tree ensemble in the JSON format.
 * \param filename name of model file
 * \param config_json JSON string consisting of key-value pairs; used for configuring the model
 *                    parser
 * \return loaded model
 */
std::unique_ptr<treelite::Model> LoadXGBoostJSONModel(
    const char* filename, const char* config_json);
/*!
 * \brief load an XGBoost model from a JSON string
 * \param json_str JSON char array
 * \param length length of JSON char array
 * \param config_json JSON string consisting of key-value pairs; used for configuring the model
 *                    parser
 * \return loaded model
 */
std::unique_ptr<treelite::Model> LoadXGBoostJSONModelString(
    const char* json_str, std::size_t length, const char* config_json);
/*!
 * \brief Load a scikit-learn random forest regressor model from a collection of arrays. Refer to
 *        https://scikit-learn.org/stable/auto_examples/tree/plot_unveil_tree_structure.html to
 *        learn the mearning of the arrays in detail. Note that this function can also be used to
 *        load an ensemble of extremely randomized trees (sklearn.ensemble.ExtraTreesRegressor).
 * \param n_estimators number of trees in the random forest
 * \param n_features number of features in the training data
 * \param node_count node_count[i] stores the number of nodes in the i-th tree
 * \param children_left children_left[i][k] stores the ID of the left child node of node k of the
 *                      i-th tree. This is only defined if node k is an internal (non-leaf) node.
 * \param children_right children_right[i][k] stores the ID of the right child node of node k of the
 *                       i-th tree. This is only defined if node k is an internal (non-leaf) node.
 * \param feature feature[i][k] stores the ID of the feature used in the binary tree split at node k
 *                of the i-th tree. This is only defined if node k is an internal (non-leaf) node.
 * \param threshold threshold[i][k] stores the threshold used in the binary tree split at node k of
 *                  the i-th tree. This is only defined if node k is an internal (non-leaf) node.
 * \param value value[i][k] stores the leaf output of node k of the i-th tree. This is only defined
 *              if node k is a leaf node.
 * \param n_node_samples n_node_samples[i][k] stores the number of data samples associated with
 *                       node k of the i-th tree.
 * \param weighted_n_node_samples weighted_n_node_samples[i][k] stores the sum of weighted data
 *                                samples associated with node k of the i-th tree.
 * \param impurity impurity[i][k] stores the impurity measure (gini, entropy etc) associated with
 *                 node k of the i-th tree.
 * \return loaded model
 */
std::unique_ptr<treelite::Model> LoadSKLearnRandomForestRegressor(
    int n_estimators, int n_features, const int64_t* node_count, const int64_t** children_left,
    const int64_t** children_right, const int64_t** feature, const double** threshold,
    const double** value, const int64_t** n_node_samples, const double** weighted_n_node_samples,
    const double** impurity);
/*!
 * \brief Load a scikit-learn isolation forest model from a collection of arrays. Refer to
 *        https://scikit-learn.org/stable/auto_examples/tree/plot_unveil_tree_structure.html to
 *        learn the mearning of the arrays in detail.
 * \param n_estimators number of trees in the isolation forest
 * \param n_features number of features in the training data
 * \param node_count node_count[i] stores the number of nodes in the i-th tree
 * \param children_left children_left[i][k] stores the ID of the left child node of node k of the
 *                      i-th tree. This is only defined if node k is an internal (non-leaf) node.
 * \param children_right children_right[i][k] stores the ID of the right child node of node k of the
 *                       i-th tree. This is only defined if node k is an internal (non-leaf) node.
 * \param feature feature[i][k] stores the ID of the feature used in the binary tree split at node k
 *                of the i-th tree. This is only defined if node k is an internal (non-leaf) node.
 * \param threshold threshold[i][k] stores the threshold used in the binary tree split at node k of
 *                  the i-th tree. This is only defined if node k is an internal (non-leaf) node.
 * \param value value[i][k] stores the expected isolation depth of node k of the i-th tree. This is
 *              only defined if node k is a leaf node.
 * \param n_node_samples n_node_samples[i][k] stores the number of data samples associated with
 *                       node k of the i-th tree.
 * \param weighted_n_node_samples weighted_n_node_samples[i][k] stores the sum of weighted data
 *                                samples associated with node k of the i-th tree.
 * \param impurity not used, but must be passed as array of arrays for each tree and node.
 * \param ratio_c standardizing constant to use for calculation of the anomaly score.
 * \return loaded model
 */
std::unique_ptr<treelite::Model> LoadSKLearnIsolationForest(
    int n_estimators, int n_features, const int64_t* node_count, const int64_t** children_left,
    const int64_t** children_right, const int64_t** feature, const double** threshold,
    const double** value, const int64_t** n_node_samples, const double** weighted_n_node_samples,
    const double** impurity, const double ratio_c);
/*!
 * \brief Load a scikit-learn random forest classifier model from a collection of arrays. Refer to
 *        https://scikit-learn.org/stable/auto_examples/tree/plot_unveil_tree_structure.html to
 *        learn the mearning of the arrays in detail. Note that this function can also be used to
 *        load an ensemble of extremely randomized trees (sklearn.ensemble.ExtraTreesClassifier).
 * \param n_estimators number of trees in the random forest
 * \param n_features number of features in the training data
 * \param n_classes number of classes in the target variable
 * \param node_count node_count[i] stores the number of nodes in the i-th tree
 * \param children_left children_left[i][k] stores the ID of the left child node of node k of the
 *                      i-th tree. This is only defined if node k is an internal (non-leaf) node.
 * \param children_right children_right[i][k] stores the ID of the right child node of node k of the
 *                       i-th tree. This is only defined if node k is an internal (non-leaf) node.
 * \param feature feature[i][k] stores the ID of the feature used in the binary tree split at node k
 *                of the i-th tree. This is only defined if node k is an internal (non-leaf) node.
 * \param threshold threshold[i][k] stores the threshold used in the binary tree split at node k of
 *                  the i-th tree. This is only defined if node k is an internal (non-leaf) node.
 * \param value value[i][k] stores the leaf output of node k of the i-th tree. This is only defined
 *              if node k is a leaf node.
 * \param n_node_samples n_node_samples[i][k] stores the number of data samples associated with
 *                       node k of the i-th tree.
 * \param weighted_n_node_samples weighted_n_node_samples[i][k] stores the sum of weighted data
 *                                samples associated with node k of the i-th tree.
 * \param impurity impurity[i][k] stores the impurity measure (gini, entropy etc) associated with
 *                 node k of the i-th tree.
 * \return loaded model
 */
std::unique_ptr<treelite::Model> LoadSKLearnRandomForestClassifier(
    int n_estimators, int n_features, int n_classes, const int64_t* node_count,
    const int64_t** children_left, const int64_t** children_right, const int64_t** feature,
    const double** threshold, const double** value, const int64_t** n_node_samples,
    const double** weighted_n_node_samples, const double** impurity);
/*!
 * \brief Load a scikit-learn gradient boosting regressor model from a collection of arrays. Refer
 *        to https://scikit-learn.org/stable/auto_examples/tree/plot_unveil_tree_structure.html to
 *        learn the mearning of the arrays in detail.
 * \param n_estimators number of trees in the random forest
 * \param n_features number of features in the training data
 * \param node_count node_count[i] stores the number of nodes in the i-th tree
 * \param children_left children_left[i][k] stores the ID of the left child node of node k of the
 *                      i-th tree. This is only defined if node k is an internal (non-leaf) node.
 * \param children_right children_right[i][k] stores the ID of the right child node of node k of the
 *                       i-th tree. This is only defined if node k is an internal (non-leaf) node.
 * \param feature feature[i][k] stores the ID of the feature used in the binary tree split at node k
 *                of the i-th tree. This is only defined if node k is an internal (non-leaf) node.
 * \param threshold threshold[i][k] stores the threshold used in the binary tree split at node k of
 *                  the i-th tree. This is only defined if node k is an internal (non-leaf) node.
 * \param value value[i][k] stores the leaf output of node k of the i-th tree. This is only defined
 *              if node k is a leaf node.
 * \param n_node_samples n_node_samples[i][k] stores the number of data samples associated with
 *                       node k of the i-th tree.
 * \param weighted_n_node_samples weighted_n_node_samples[i][k] stores the sum of weighted data
 *                                samples associated with node k of the i-th tree.
 * \param impurity impurity[i][k] stores the impurity measure (gini, entropy etc) associated with
 *                 node k of the i-th tree.
 * \return loaded model
 */
std::unique_ptr<treelite::Model> LoadSKLearnGradientBoostingRegressor(
    int n_estimators, int n_features, const int64_t* node_count, const int64_t** children_left,
    const int64_t** children_right, const int64_t** feature, const double** threshold,
    const double** value, const int64_t** n_node_samples, const double** weighted_n_node_samples,
    const double** impurity);
/*!
 * \brief Load a scikit-learn gradient boosting classifier model from a collection of arrays. Refer
 *        to https://scikit-learn.org/stable/auto_examples/tree/plot_unveil_tree_structure.html to
 *        learn the mearning of the arrays in detail.
 * \param n_estimators number of trees in the random forest
 * \param n_features number of features in the training data
 * \param n_classes number of classes in the target variable
 * \param node_count node_count[i] stores the number of nodes in the i-th tree
 * \param children_left children_left[i][k] stores the ID of the left child node of node k of the
 *                      i-th tree. This is only defined if node k is an internal (non-leaf) node.
 * \param children_right children_right[i][k] stores the ID of the right child node of node k of the
 *                       i-th tree. This is only defined if node k is an internal (non-leaf) node.
 * \param feature feature[i][k] stores the ID of the feature used in the binary tree split at node k
 *                of the i-th tree. This is only defined if node k is an internal (non-leaf) node.
 * \param threshold threshold[i][k] stores the threshold used in the binary tree split at node k of
 *                  the i-th tree. This is only defined if node k is an internal (non-leaf) node.
 * \param value value[i][k] stores the leaf output of node k of the i-th tree. This is only defined
 *              if node k is a leaf node.
 * \param n_node_samples n_node_samples[i][k] stores the number of data samples associated with
 *                       node k of the i-th tree.
 * \param weighted_n_node_samples weighted_n_node_samples[i][k] stores the sum of weighted data
 *                                samples associated with node k of the i-th tree.
 * \param impurity impurity[i][k] stores the impurity measure (gini, entropy etc) associated with
 *                 node k of the i-th tree.
 * \return loaded model
 */
std::unique_ptr<treelite::Model> LoadSKLearnGradientBoostingClassifier(
    int n_estimators, int n_features, int n_classes, const int64_t* node_count,
    const int64_t** children_left, const int64_t** children_right, const int64_t** feature,
    const double** threshold, const double** value, const int64_t** n_node_samples,
    const double** weighted_n_node_samples, const double** impurity);

//--------------------------------------------------------------------------
// model builder interface: build trees incrementally
//--------------------------------------------------------------------------

/* forward declarations */
struct TreeBuilderImpl;
struct ModelBuilderImpl;
class ModelBuilder;

class Value {
 private:
  std::shared_ptr<void> handle_;
  TypeInfo type_;
 public:
  Value();
  ~Value() = default;
  Value(const Value&) = default;
  Value(Value&&) noexcept = default;
  Value& operator=(const Value&) = default;
  Value& operator=(Value&&) noexcept = default;
  template <typename T>
  static Value Create(T init_value);
  static Value Create(const void* init_value, TypeInfo type);
  template <typename T>
  T& Get();
  template <typename T>
  const T& Get() const;
  template <typename Func>
  inline auto Dispatch(Func func);
  template <typename Func>
  inline auto Dispatch(Func func) const;
  TypeInfo GetValueType() const;
};

/*! \brief tree builder class */
class TreeBuilder {
 public:
  /*!
   * \brief Constructor
   * \param threshold_type Type of thresholds in numerical splits. All thresholds in a given model
   *                       must have the same type.
   * \param leaf_output_type Type of leaf outputs. All leaf outputs in a given model must have the
   *                         same type.
   */
  TreeBuilder(TypeInfo threshold_type, TypeInfo leaf_output_type);  // constructor
  ~TreeBuilder();  // destructor
  // this class is only move-constructible and move-assignable
  TreeBuilder(const TreeBuilder&) = delete;
  TreeBuilder(TreeBuilder&&) = default;
  TreeBuilder& operator=(const TreeBuilder&) = delete;
  TreeBuilder& operator=(TreeBuilder&&) = default;
  /*!
   * \brief Create an empty node within a tree
   * \param node_key unique integer key to identify the new node
   */
  void CreateNode(int node_key);
  /*!
   * \brief Remove a node from a tree
   * \param node_key unique integer key to identify the node to be removed
   */
  void DeleteNode(int node_key);
  /*!
   * \brief Set a node as the root of a tree
   * \param node_key unique integer key to identify the root node
   */
  void SetRootNode(int node_key);
  /*!
   * \brief Turn an empty node into a numerical test node; the test is in the
   *        form [feature value] OP [threshold]. Depending on the result of the
   *        test, either left or right child would be taken.
   * \param node_key unique integer key to identify the node being modified;
   *                 this node needs to be empty
   * \param feature_id id of feature
   * \param op binary operator to use in the test
   * \param threshold threshold value
   * \param default_left default direction for missing values
   * \param left_child_key unique integer key to identify the left child node
   * \param right_child_key unique integer key to identify the right child node
   */
  void SetNumericalTestNode(int node_key, unsigned feature_id, const char* op, Value threshold,
                            bool default_left, int left_child_key, int right_child_key);
  void SetNumericalTestNode(int node_key, unsigned feature_id, Operator op, Value threshold,
                            bool default_left, int left_child_key, int right_child_key);
  /*!
   * \brief Turn an empty node into a categorical test node.
   * A list defines all categories that would be classified as the left side.
   * Categories are integers ranging from 0 to (n-1), where n is the number of
   * categories in that particular feature. Let's assume n <= 64.
   * \param node_key unique integer key to identify the node being modified;
   *                 this node needs to be empty
   * \param feature_id id of feature
   * \param left_categories list of categories belonging to the left child
   * \param default_left default direction for missing values
   * \param left_child_key unique integer key to identify the left child node
   * \param right_child_key unique integer key to identify the right child node
   */
  void SetCategoricalTestNode(int node_key, unsigned feature_id,
                              const std::vector<uint32_t>& left_categories, bool default_left,
                              int left_child_key, int right_child_key);
  /*!
   * \brief Turn an empty node into a leaf node
   * \param node_key unique integer key to identify the node being modified;
   *                 this node needs to be empty
   * \param leaf_value leaf value (weight) of the leaf node
   */
  void SetLeafNode(int node_key, Value leaf_value);
  /*!
  * \brief Turn an empty node into a leaf vector node
  * The leaf vector (collection of multiple leaf weights per leaf node) is
  * useful for multi-class random forest classifier.
  * \param node_key unique integer key to identify the node being modified;
  *                 this node needs to be empty
  * \param leaf_vector leaf vector of the leaf node
  */
  void SetLeafVectorNode(int node_key, const std::vector<Value>& leaf_vector);

 private:
  std::unique_ptr<TreeBuilderImpl> pimpl_;  // Pimpl pattern
  ModelBuilder* ensemble_id_;  // id of ensemble (nullptr if not part of any)
  friend class ModelBuilder;
  friend struct ModelBuilderImpl;
};

/*! \brief model builder class */
class ModelBuilder {
 public:
  /*!
   * \brief Constructor
   * \param num_feature number of features used in model being built. We assume
   *                    that all feature indices are between 0 and
   *                    (num_feature - 1).
   * \param num_class number of output groups. Set to 1 for binary
   *                         classification and regression; >1 for multiclass
   *                         classification
   * \param average_tree_output whether the model is a random forest (true) or
   *                           gradient boosted trees (false)
   * \param threshold_type Type of thresholds in numerical splits. All thresholds in a given model
   *                       must have the same type.
   * \param leaf_output_type Type of leaf outputs. All leaf outputs in a given model must have the
   *                         same type.
   */
  ModelBuilder(int num_feature, int num_class, bool average_tree_output,
               TypeInfo threshold_type, TypeInfo leaf_output_type);
  ~ModelBuilder();  // destructor
  /*!
   * \brief Set a model parameter
   * \param name name of parameter
   * \param value value of parameter
   */
  void SetModelParam(const char* name, const char* value);
  /*!
   * \brief Insert a tree at specified location
   * \param tree_builder builder for the tree to be inserted. The tree must not
   *                     be part of any other existing tree ensemble. Note:
   *                     The tree_builder argument will become unusuable after
   *                     the tree insertion. Should you want to modify the
   *                     tree afterwards, use GetTree(*) method to get a fresh
   *                     handle to the tree.
   * \param index index of the element before which to insert the tree;
   *              use -1 to insert at the end
   * \return index of the new tree within the ensemble; -1 for failure
   */
  int InsertTree(TreeBuilder* tree_builder, int index = -1);
  /*!
   * \brief Get a reference to a tree in the ensemble
   * \param index index of the tree in the ensemble
   * \return reference to the tree
   */
  TreeBuilder* GetTree(int index);
  const TreeBuilder* GetTree(int index) const;
  /*!
   * \brief Remove a tree from the ensemble
   * \param index index of the tree that would be removed
   */
  void DeleteTree(int index);
  /*!
   * \brief finalize the model and produce the in-memory representation
   * \return the finished model
   */
  std::unique_ptr<Model> CommitModel();

 private:
  std::unique_ptr<ModelBuilderImpl> pimpl_;  // Pimpl pattern
};

}  // namespace frontend
}  // namespace treelite

#include "frontend_impl.h"

#endif  // TREELITE_FRONTEND_H_
