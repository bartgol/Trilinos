// @HEADER
// ****************************************************************************
//                Tempus: Copyright (2017) Sandia Corporation
//
// Distributed under BSD 3-clause license (See accompanying file Copyright.txt)
// ****************************************************************************
// @HEADER

#ifndef Tempus_SecondOrderResidualModelEvaluator_decl_hpp
#define Tempus_SecondOrderResidualModelEvaluator_decl_hpp

#include <functional>
#include "Thyra_StateFuncModelEvaluatorBase.hpp"

namespace Tempus {

/** \brief A ModelEvaluator for residual evaluations given a state.
 *  This ModelEvaluator takes a state, x, and determines its residual,
 *  \f$ g(x) \f$, which is suitable for a nonlinear solve.  This is
 *  accomplished by computing the time derivative of the state, x_dot,
 *  (through Lambda functions), supplying the current time, and calling
 *  the application transient ModelEvaluator, \f$ f(\dot{x},x,t) \f$.
 *
 *  This class breaks the primary design principle for ModelEvaluators;
 *  it is not stateless!
 */
template <typename Scalar>
class SecondOrderResidualModelEvaluator
 : public Thyra::StateFuncModelEvaluatorBase<Scalar>
{
public:
  typedef Thyra::VectorBase<Scalar>  Vector;

  /// Constructor
  SecondOrderResidualModelEvaluator(
    const Teuchos::RCP<const Thyra::ModelEvaluator<Scalar> >& transientModel)
    : transientModel_(transientModel)
  {}

  /// Set the underlying transient ModelEvaluator
  void setTransientModel(
    const Teuchos::RCP<const Thyra::ModelEvaluator<Scalar> > & me)
  { transientModel_ = me; }

  /// Get the underlying transient model 'f'
  Teuchos::RCP<const Thyra::ModelEvaluator<Scalar> > getTransientModel() const
  { return transientModel_; }

  /// Set values to compute x dot and evaluate transient model.
  void initialize(
    std::function<void (const Vector &,Vector &)> computeXDot,
    double t, double alpha, double beta, double omega = 0.0)
  {
    computeXDot_ = computeXDot; t_ = t;
    alpha_ = alpha; beta_ = beta; omega_ = omega;
  }

  /// \name Overridden from Thyra::StateFuncModelEvaluatorBase
  //@{
    Teuchos::RCP<Thyra::LinearOpBase<Scalar> > create_W_op() const
      { return transientModel_->create_W_op(); }

    Teuchos::RCP<const Thyra::LinearOpWithSolveFactoryBase<Scalar> >
    get_W_factory() const { return transientModel_->get_W_factory(); }

    Teuchos::RCP<const Thyra::VectorSpaceBase<Scalar> > get_f_space() const
      { return transientModel_->get_f_space(); }
    Teuchos::RCP<const Thyra::VectorSpaceBase<Scalar> > get_p_space(int p) const
      { return transientModel_->get_p_space(p); };

    Teuchos::RCP<const Teuchos::Array<std::string> > get_p_names(int p) const
      { return transientModel_->get_p_names(p); }

    Teuchos::RCP<const Thyra::VectorSpaceBase<Scalar> > get_x_space() const
      { return transientModel_->get_x_space(); }

    Thyra::ModelEvaluatorBase::InArgs<Scalar> getNominalValues() const
      { return transientModel_->getNominalValues(); }

    Thyra::ModelEvaluatorBase::InArgs<Scalar> createInArgs() const;
    Thyra::ModelEvaluatorBase::OutArgs<Scalar> createOutArgsImpl() const;

    void evalModelImpl(
              const Thyra::ModelEvaluatorBase::InArgs<Scalar>  &inArgs,
              const Thyra::ModelEvaluatorBase::OutArgs<Scalar> &outArgs) const;
  //@}

private:
  /// Default constructor - not allowed
  SecondOrderResidualModelEvaluator();

private:
  Teuchos::RCP<const Thyra::ModelEvaluator<Scalar> > transientModel_;
  std::function<void (const Vector &,Vector &)> computeXDot_;
  Scalar t_;
  Scalar alpha_;
  Scalar beta_;
  Scalar omega_;
};

} // namespace Tempus

#endif // Tempus_SecondOrderResidualModelEvaluator_hpp
