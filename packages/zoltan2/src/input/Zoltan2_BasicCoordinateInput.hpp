// @HEADER
// ***********************************************************************
//
//                Copyright message goes here.   TODO
//
// ***********************************************************************
// @HEADER

/*! \file Zoltan2_BasicCoordinateInput.hpp
    \brief Defines the BasicCoordinateInput class.
*/

#ifndef _ZOLTAN2_BASICCOORDINATEINPUT_HPP_
#define _ZOLTAN2_BASICCOORDINATEINPUT_HPP_

#include <Zoltan2_CoordinateInput.hpp>
#include <Zoltan2_StridedInput.hpp>

namespace Zoltan2 {

  /*!  \brief BasicCoordinateInput represents geometric coordinates that are
                supplied by the user as pointers to strided arrays.

    Input adapters provide access for Zoltan2 to the user's data.  The
    methods in the interface must be defined by users.  Many built-in
    adapters are already defined for common data structures, such as
    Tpetra and Epetra objects and C-language pointers to arrays.

    Data types:
    \li \c scalar_t is the data type for weights and coordinates
    \li \c lno_t is the integral data type used by Zoltan2 for local indices and local counts.
    \li \c gno_t is the integral data type used by Zoltan2 to represent global indices and global counts.
    \li \c gid_t is the data type used by the application for global Ids.  If the application's global Id data type is a Teuchos Ordinal, then \c gid_t and \c gno_t are the same.  Otherwise, the application global Ids will be mapped to Teuchos Ordinals for use by Zoltan2 internally.  (Teuchos Ordinals are those data types for which traits are defined in Trilinos/packages/teuchos/src/Teuchos_OrdinalTraits.hpp.)
    \li \c node_t is a sub class of Kokkos::StandardNodeMemoryModel, which is used to optimize performance on many-core and multi-core architectures.  If you don't use Kokkos, you can ignore this data type.

    The template parameter (\c User) is a C++ class type which provides the
    actual data types with which the Zoltan2 library will be compiled, through
    a Traits mechanism.  \c User may be the
    actual class used by application to represent coordinates, or it may be
    the empty helper class \c BasicUserTypes with which a Zoltan2 user
    can easily supply the data types for the library.

  \todo Add simpler constructors specifically for dimension 1, 2 and 3.
*/

template <typename User>
  class BasicCoordinateInput : public CoordinateInput<User> {

public:

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  typedef typename InputTraits<User>::scalar_t scalar_t;
  typedef typename InputTraits<User>::lno_t    lno_t;
  typedef typename InputTraits<User>::gno_t    gno_t;
  typedef typename InputTraits<User>::gid_t    gid_t;
  typedef typename InputTraits<User>::node_t   node_t;
  typedef CoordinateInput<User>   base_adapter_t;
  typedef User user_t;

#endif

  /*! \brief Constructor for dimension 1, 2 or 3 and no weights.
   *
   * \param numIds The number of local coordinates.
   * \param ids    The global identifiers for the coordinates.
   * \param x      A pointer to the first dimension of the coordinates.
   * \param y      A pointer to the second dimension, if any.
   * \param z      A pointer to the third dimension, if any.
   * \param xStride  The stride for the \c x list.  The \x coordinate
   *          for point \c ids[n]  should be found at <tt>x[xStride * n]</tt>.
   * \param yStride  The stride for the \c y list.  The \y coordinate
   *          for point \c ids[n]  should be found at <tt>y[yStride * n]</tt>.
   * \param zStride  The stride for the \c z list.  The \z coordinate
   *          for point \c ids[n]  should be found at <tt>z[zStride * n]</tt>.
   *  
   *  The values pointed to the arguments must remain valid for the
   *  lifetime of this InputAdapter.
   */

  BasicCoordinateInput(lno_t numIds, const gid_t *ids,
    const scalar_t *x, const scalar_t *y, const scalar_t *z,
    int xStride=1, int yStride=1, int zStride=1);

  /*! \brief Constructor for arbitrary dimension with weights.
   *
   *  \param dim is the dimension of the  geometric coordinates.
   *  \param numIds   the local number of coordinates.
   *  \param ids     is a pointer to the coordinate global Ids. 
   *  \param values a list of \c dim pointers to the coordinate values
   *          corresponding to the \c numIds ids
   *  \param valueStrides The \c dim strides for the \c values list.  
   *           The coordinate for dimension \n for \c ids[k] should be
   *           found at <tt>values[n][valueStrides[n] * k]</tt>.
   *           If valueStrides is NULL, it is assumed all strides are one.
   *  \param numWeights the number of weights per coordinate , which may be zero
   *                or greater
   *  \param weights  \c numWeights pointers to arrays of weights.  \c weights
             may be NULL if there are no arrays of weights.
   *  \param weightStrides  a list of \c numWeights strides for the \c weights
   *        arrays. 
   *           The weight for weight dimension \n for \c ids[k] should be
   *           found at <tt>weights[n][weightStrides[n] * k]</tt>.
   *           If weightStrides is NULL, it is assumed all strides are one.
   *  
   *  The values pointed to the arguments must remain valid for the
   *  lifetime of this InputAdapter.
   */

  BasicCoordinateInput( int dim, lno_t numIds, const gid_t *ids, 
    const scalar_t * const *values, const int *valueStrides,
    int numWeights, const scalar_t * const * weights, const int *weightStrides);

  /*! Destructor
   */
  ~BasicCoordinateInput() {};

  ////////////////////////////////////////////////////////////////
  // The InputAdapter interface.
  ////////////////////////////////////////////////////////////////

  std::string inputAdapterName() const {return std::string("BasicCoordinate");}

  size_t getLocalNumberOfObjects() const { return numIds_;}

  int getNumberOfWeightsPerObject() const { return numWeights_;}

  ////////////////////////////////////////////////////
  // The CoordinateInput interface.
  ////////////////////////////////////////////////////

  int getCoordinateDimension() const { return dimension_;}

  int getNumberOfWeights() const { return numWeights_;}  

  size_t getLocalNumberOfCoordinates() const { return numIds_; }

  size_t getGlobalNumberOfCoordinates() const { return globalNumIds_;}

  size_t getCoordinates(int dim, const gid_t *&gids, const scalar_t *&coords, 
    int &stride) const
  {
    env_->localInputAssertion(__FILE__, __LINE__, "invalid dimension",
      dim >= 0 && dim < dimension_, BASIC_ASSERTION);

    gids = idList_;
    
    size_t length;

    coords_[dim]->getStridedList(length, coords, stride);

    return length;
  }

  size_t getCoordinateWeights(int dim, const scalar_t *&weights, 
    int &stride) const
  {
    env_->localInputAssertion(__FILE__, __LINE__, "invalid dimension",
      dim >= 0 && dim < numWeights_, BASIC_ASSERTION);
    
    size_t length;

    weights_[dim]->getStridedList(length, weights, stride);

    return length;
  }

private:
  void initializeData(const scalar_t * const *values, const int *valueStrides,
    const scalar_t * const *weights, const int *weightStrides);

  // A default Environment for error handling.
  RCP<const Environment> env_;

  lno_t numIds_;
  gno_t globalNumIds_;

  const gid_t *idList_;

  int dimension_;
  Array<RCP<StridedInput<lno_t, scalar_t> > > coords_;

  int numWeights_;
  Array<RCP<StridedInput<lno_t, scalar_t> > > weights_;
};

/////////////////////////////////////////////////////////////////
// Definitions
/////////////////////////////////////////////////////////////////

template <typename User>
  BasicCoordinateInput<User>::BasicCoordinateInput( 
    lno_t numIds, const gid_t *ids,
    const scalar_t *x, const scalar_t *y, const scalar_t *z,
    int xStride, int yStride, int zStride):
      env_(rcp(new Environment)), 
      numIds_(numIds), globalNumIds_(), idList_(ids), 
      dimension_(0), coords_(0), 
      numWeights_(0), weights_(0)
{
  const scalar_t *values[3]={NULL,NULL,NULL};
  int valueStrides[3]={0,0,0};

  if (x){
    values[0] = x;
    valueStrides[0] = xStride;
    dimension_++;
    if (y){
      values[0] = y;
      valueStrides[0] = yStride;
      dimension_++;
      if (z){
        values[0] = z;
        valueStrides[0] = zStride;
        dimension_++;
      }
    }
  }

  coords_.resize(dimension_);

  initializeData(values, valueStrides, NULL, NULL);
}

template <typename User>
  BasicCoordinateInput<User>::BasicCoordinateInput( 
    int dim, lno_t numIds, const gid_t *ids, 
    const scalar_t * const *values, const int *valueStrides,
    int numWeights, const scalar_t * const * weights, const int *weightStrides):
      env_(rcp(new Environment)), 
      numIds_(numIds), globalNumIds_(), idList_(ids), 
      dimension_(dim), coords_(dim), 
      numWeights_(numWeights), weights_(numWeights)
{
  initializeData(values, valueStrides, weights, weightStrides);
}

template <typename User>
  void BasicCoordinateInput<User>::initializeData(
    const scalar_t * const *values, const int *valueStrides,
    const scalar_t * const *weights, const int *weightStrides)
{
  typedef StridedInput<lno_t,scalar_t> input_t;

  gno_t tmp = numIds_;
  try{
    reduceAll<int, gno_t>(*(env_->comm_), Teuchos::REDUCE_SUM, 1, 
       &tmp, &globalNumIds_);
  }
  Z2_THROW_OUTSIDE_ERROR(*env_, e);

  if (numIds_){
    int stride = 1;
    for (int x=0; x < dimension_; x++){
      if (valueStrides)
        stride = valueStrides[x];
      coords_[x] = rcp<input_t>(new input_t(
        ArrayView<const scalar_t>(values[x], stride*numIds_), stride));
    }

    if (numWeights_){
      stride = 1;
      for (int w=0; w < numWeights_; w++){
        if (weightStrides)
          stride = weightStrides[w];
        weights_[w] = rcp<input_t>(new input_t(
          ArrayView<const scalar_t>(weights[w], stride*numIds_), stride));
      }
    }
  }
}
  
}  //namespace Zoltan2
  
#endif
