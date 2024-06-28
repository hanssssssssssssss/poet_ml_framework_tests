#ifndef DATASTRUCTURES_H_
#define DATASTRUCTURES_H_

#include <Rcpp.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace poet {

using FieldColumn = std::vector<double>;

/**
 * Stores data for input/output of a module. The class keeps track of all
 * defined properties with name and import/export to 1D and 2D STL vectors.
 * Also, it eases the update process of a field with an input from another
 * field.
 *
 * It can be seen as an R-like data frame, but with less access to the members.
 * Species values are stored as "columns", where column is a STL vector.
 */
class Field : private std::unordered_map<std::string, FieldColumn> {
public:
  Field(){};

  /**
   * Creates a new instance of a field with fixed expected vector size.
   *
   * \param vec_s expected vector size of each component/column.
   */
  Field(std::uint32_t vec_s) : req_vec_size(vec_s){};

  /**
   * Initializes instance with a 2D vector and according names for each columnn.
   * There is no check if names were given multiple times. The order of name
   * vector also defines the ordering of the output.
   *
   * \param vec_s expected vector size of each component/column.
   * \param input 2D vector using STL semantic describing the current state of
   * the field.
   * \param prop_vec Name of each vector in the input. Shall match
   * the count of vectors.
   *
   * \exception std::runtime_error If prop_vec size doesn't match input vector
   * size or column vectors size doesn't match expected vector size.
   */
  Field(std::uint32_t vec_s, const std::vector<std::vector<double>> &input,
        const std::vector<std::string> &prop_vec, bool row_major = false);

  /**
   * Initializes instance with a 1D continious memory vector and according names
   * for each columnn. There is no check if names were given multiple times. The
   * order of name vector also defines the ordering of the output.
   *
   * \param vec_s expected vector size of each component/column.
   * \param input 1D vector using STL semantic describing the current state of
   * the field storing each column starting at index *i times requested vector
   * size*.
   * \param prop_vec Name of each vector in the input. Shall match the
   * count of vectors.
   *
   * \exception std::runtime_error If prop_vec size doesn't match input vector
   * size or column vectors size doesn't match expected vector size.
   */
  Field(std::uint32_t vec_s, const std::vector<double> &input,
        const std::vector<std::string> &prop_vec);

  Field(const SEXP &s_init) { fromSEXP(s_init); }

  Field &operator=(const Field &rhs) {
    this->req_vec_size = rhs.req_vec_size;
    this->props = rhs.props;

    this->clear();

    for (const auto &pair : rhs) {
      this->insert(pair);
    }
    return *this;
  }

  /**
   * Read in a (previously exported) 1D vector. It has to have the same
   * dimensions as the current column count times the requested vector size of
   * this instance. Import order is given by the species name vector.
   *
   * \param cont_field 1D field as vector.
   *
   * \exception std::runtime_error Input vector does not match the expected
   * size.
   */
  Field &operator=(const FieldColumn &cont_field);

  /**
   * Read in a (previously exported) 2D vector. It has to have the same
   * dimensions as the current column count of this instance and each vector
   * must have the size of the requested vector size. Import order is given by
   * the species name vector.
   *
   * \param cont_field 2D field as vector.
   *
   * \exception std::runtime_error Input vector has more or less elements than
   * the instance or a column vector does not match expected vector size.
   */
  Field &operator=(const std::vector<FieldColumn> &cont_field);

  Field &operator=(const SEXP &s_rhs) {
    fromSEXP(s_rhs);
    return *this;
  }

  /**
   * Returns a reference to the column vector with given name. Creates a new
   * vector if prop was not found. The prop name will be added to the end of the
   * list.
   *
   * \param key Name of the prop.
   *
   * \return Reference to the column vector.
   */
  FieldColumn &operator[](const std::string &key);

  /**
   * Returns the names of all species defined in the instance.
   *
   * \return Vector containing all species names in output order.
   */
  auto GetProps() const -> std::vector<std::string> { return this->props; }

  /**
   * Return the requested vector size.
   *
   * \return Requested vector size set in the instanciation of the object.
   */
  auto GetRequestedVecSize() const -> std::uint32_t {
    return this->req_vec_size;
  };

  /**
   * Updates all species with values from another field. If one element of the
   * input field doesn't match the names of the calling instance it will get
   * skipped.
   *
   * \param input Field to update the current instance's columns.
   */
  void update(const Field &input);

  /**
   * Builds a new 1D vector with the current state of the instance. The output
   * order is given by the given species name vector set earlier and/or added
   * values using the [] operator.
   *
   * \return 1D STL vector stored each column one after another.
   */
  auto AsVector() const -> FieldColumn;

  /**
   * Builds a new 2D vector with the current state of the instance. The output
   * order is given by the given species name vector set earlier and/or added
   * values using the [] operator.
   *
   * \return 2D STL vector stored each column one after anothe in a new vector
   * element.
   */
  auto As2DVector() const -> std::vector<FieldColumn>;

  SEXP asSEXP() const;

  std::size_t cols() const { return this->props.size(); }

private:
  std::uint32_t req_vec_size{0};

  std::vector<std::string> props{{}};

  void fromSEXP(const SEXP &s_rhs);
};

} // namespace poet
#endif // DATASTRUCTURES_H_
