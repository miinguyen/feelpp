
#ifndef FEELPP_MODELS_STABILIZATIONGLSPARAMETER_HPP
#define FEELPP_MODELS_STABILIZATIONGLSPARAMETER_HPP 1

#include <feel/feelmodels/modelcore/stabilizationglsparameterbase.hpp>
#include <feel/feelalg/matrixeigendense.hpp>
#include <feel/feelvf/vf.hpp>

//#include <feel/feeldiscr/pch.hpp>
//#include <feel/feeldiscr/pchv.hpp>
#include <feel/feeldiscr/pdh.hpp>

#include <feel/feelmodels/modelvf/stabilizationglsparameter.hpp>

namespace Feel {

namespace FeelModels
{


template<typename MeshType, uint16_type OrderPoly>
class StabilizationGLSParameter : public StabilizationGLSParameterBase<MeshType>
{
    using super_type = StabilizationGLSParameterBase<MeshType>;
public :

    using mesh_t = typename super_type::mesh_t;// MeshType;
    using mesh_ptr_t = typename super_type::mesh_ptr_t;

    static const uint16_type nOrder = OrderPoly;
    using space_t = Pdh_type<mesh_t,nOrder>;
    using space_ptr_t = boost::shared_ptr<space_t>;
    using element_t = typename space_t::element_type;
    using element_ptr_t = typename space_t::element_ptrtype;
#if 0
    using space_P0_t = Pdh_type<mesh_t,0>;
    using space_P0_ptr_t = boost::shared_ptr<space_P0_t>;
    using element_P0_t = typename space_P0_t::element_type;
#endif

    StabilizationGLSParameter( mesh_ptr_t const& mesh, std::string const& prefix )
        :
        super_type( mesh, prefix )
        //M_Xh( Pdh<nOrder>(mesh) )
#if 0
        ,
        M_spaceP0( Pdh<0>(mesh) ),
        M_hSize( M_spaceP0, "hSize" ),
        M_lambdaK( M_spaceP0, "lambdak" ),
        M_sqrtLambdaK( M_spaceP0, "sqrtLambdaK"),
        M_mK( M_spaceP0, "mk")
#endif
        {}

    void init();


    template<bool HasConvectionExpr=true, bool HasCoeffDiffusionExpr=true, typename ExprConvectiontype, typename ExprCoeffDiffusionType, typename RangeType>
    void updateTau( ExprConvectiontype const& exprConvection, ExprCoeffDiffusionType const& exprCoeffDiffusion, RangeType const& RangeStab )
        {
            auto tau = Feel::vf::FeelModels::stabilizationGLSParameterExpr<HasConvectionExpr,HasCoeffDiffusionExpr>( *this, exprConvection, exprCoeffDiffusion );
            M_fieldTau->on(_range=RangeStab,_expr=tau);
        }

    element_ptr_t const& fieldTauPtr() const { return M_fieldTau; }
    element_t const& fieldTau() const { return *M_fieldTau; }

#if 0
    template<typename ExprConvectiontype, typename ExprCoeffDiffusionType>
    auto localPeRe( ExprConvectiontype const& exprConvection, ExprCoeffDiffusionType const& exprCoeffDiffusion, mpl::int_<0> /**/ ) const;
    template<typename ExprConvectiontype, typename ExprCoeffDiffusionType>
    auto localPeRe( ExprConvectiontype const& exprConvection, ExprCoeffDiffusionType const& exprCoeffDiffusion, mpl::int_<1> /**/ ) const;
    template<typename ExprConvectiontype, typename ExprCoeffDiffusionType>
    auto localPeRe( ExprConvectiontype const& exprConvection, ExprCoeffDiffusionType const& exprCoeffDiffusion, mpl::int_<2> /**/ ) const;

    template<typename ExprConvectiontype, typename ExprCoeffDiffusionType>
    auto tau( ExprConvectiontype const& exprConvection, ExprCoeffDiffusionType const& exprCoeffDiffusion ) const
        {
            return this->tau( exprConvection, exprCoeffDiffusion, mpl::int_<0>() );
        }
    template<typename ExprConvectiontype, typename ExprCoeffDiffusionType>
    auto tau( ExprConvectiontype const& exprConvection, ExprCoeffDiffusionType const& exprCoeffDiffusion, mpl::int_<0> /**/ ) const;
    template<typename ExprConvectiontype, typename ExprCoeffDiffusionType>
    auto tau( ExprConvectiontype const& exprConvection, ExprCoeffDiffusionType const& exprCoeffDiffusion, mpl::int_<1> /**/ ) const;
    template<typename ExprConvectiontype, typename ExprCoeffDiffusionType>
    auto tau( ExprConvectiontype const& exprConvection, ExprCoeffDiffusionType const& exprCoeffDiffusion, mpl::int_<2> /**/ ) const;

    template<int ParameterType, typename ExprConvectiontype, typename ExprCoeffDiffusionType>
    auto delta( ExprConvectiontype const& exprConvection, ExprCoeffDiffusionType const& exprCoeffDiffusion ) const;

#endif

private :
    space_ptr_t M_Xh;
    element_ptr_t M_fieldTau;
    element_ptr_t M_fieldDelta;
#if 0
    space_P0_ptr_t M_spaceP0;
    element_P0_t M_hSize, M_lambdaK, M_sqrtLambdaK, M_mK;
#endif
}; // class StabilizationGLSParameter


template<typename MeshType, uint16_type OrderPoly>
void
StabilizationGLSParameter<MeshType,OrderPoly>::init()
{
    auto mesh = this->M_mesh;
    if ( !M_Xh )
    {
        M_Xh = Pdh<nOrder>(mesh);
        M_fieldTau = M_Xh->elementPtr();
    }

#if 0
    if ( this->M_hSizeMethod == "hmin" )
        M_hSize.on(_range=elements(mesh), _expr=hMin() );
    else if ( this->M_hSizeMethod == "h" )
        M_hSize.on(_range=elements(mesh), _expr=h() );
    else if ( this->M_hSizeMethod == "meas" )
        M_hSize.on(_range=elements(mesh), _expr=pow(meas(),1./mesh_t::nDim) );
#endif
    this->M_hSizeValues.clear();
    for ( auto const elt : allelements(mesh) )
    {
        size_type id = elt.id();
        if ( this->M_hSizeMethod == "hmin" )
            this->M_hSizeValues[id] = elt.hMin();
        else if ( this->M_hSizeMethod == "h" )
            this->M_hSizeValues[id] = elt.h();
        else if ( this->M_hSizeMethod == "meas" )
            this->M_hSizeValues[id] = math::pow(elt.measure(),1./mesh_t::nDim);
    }

    // Stabilization : computation of lambdaK
    if ( this->M_method == "eigenvalue" || this->M_method == "eigenvalue-simplified" )
    {
        this->M_lambdaKValues.clear();
        this->M_sqrtLambdaKValues.clear();
        this->M_mKValues.clear();

        if ( nOrder == 1 )
        {
            for ( auto const elt : allelements(mesh) )
                this->M_mKValues[elt.id()] = 1./3;
        }
        else
        {

        auto Ph = M_Xh;
        auto w = Ph->element();

        auto matA = backend()->newMatrix(_test=Ph,_trial=Ph );
        auto matB = backend()->newMatrix(_test=Ph,_trial=Ph );
        auto fA = form2( _trial=Ph, _test=Ph, _matrix=matA );
        fA = integrate( _range=elements(mesh),
                        _expr=inner(laplacian(w),laplaciant(w)) );
        matA->close();
        auto fB = form2( _trial=Ph, _test=Ph, _matrix=matB );
        fB = integrate( _range=elements(mesh),
                        _expr=inner(grad(w),gradt(w)) );
        if ( math::abs( this->M_penalLambdaK ) > 1e-12 )
            fB += integrate( elements(mesh),
                             this->M_penalLambdaK*inner(id(w),idt(w)) );
        matB->close();

        Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic> localEigenMatA(0,0);
        Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic> localEigenMatB(0,0);

        for ( auto const& eltWrap : elements(this->M_mesh) )
        {
            auto const& elt = unwrap_ref( eltWrap );
            auto extractIndices = Ph->dof()->getIndices( elt.id() );
            int nLocalIndices = extractIndices.size();
            if ( localEigenMatA.rows() != nLocalIndices)
            {
                localEigenMatA.resize( nLocalIndices,nLocalIndices );
                localEigenMatB.resize( nLocalIndices,nLocalIndices );
            }
            for ( int i=0;i<nLocalIndices;++i )
            {
                for ( int j=0;j<nLocalIndices;++j )
                {
                    localEigenMatA(i,j) = matA->operator()(extractIndices[i],extractIndices[j]);
                    localEigenMatB(i,j) = matB->operator()(extractIndices[i],extractIndices[j]);
                }
            }
            Eigen::GeneralizedSelfAdjointEigenSolver<Eigen::MatrixXd> es( localEigenMatA,localEigenMatB );
            Eigen::VectorXd eigen_vls= es.eigenvalues();
            double lambda = eigen_vls.maxCoeff();
            size_type eltId = elt.id();
            this->M_lambdaKValues[eltId] = lambda;
            this->M_sqrtLambdaKValues[eltId] = math::sqrt( lambda );
            double currenthSize = this->hSize( eltId );
            double cKValues = 1./(lambda*math::pow(currenthSize,2));
            this->M_mKValues[elt.id()] = std::min( 1./3.,2*cKValues);
#if 0
            M_lambdaK.on( _range=idedelements( mesh,elt.id() ), _expr=cst(lambda) );
#endif
        }
#if 0
        M_sqrtLambdaK.on(_range=elements(mesh), _expr=sqrt(idv(M_lambdaK)) );
        auto cK = cst(1.)/(idv(M_lambdaK)*pow(idv(M_hSize),2));
        M_mK.on(_range=elements(mesh),_expr=min( cst(1./3.),2*cK ) );
#endif
        }
    }
}


#if 0
template<typename MeshType, uint16_type OrderPoly>
template<typename ExprConvectiontype, typename ExprCoeffDiffusionType>
auto
StabilizationGLSParameter<MeshType,OrderPoly>::localPeRe( ExprConvectiontype const& exprConvection, ExprCoeffDiffusionType const& exprCoeffDiffusion, mpl::int_<0> /**/ ) const
{
    auto norm_u = norm2( exprConvection );
    auto Re = idv(M_mK)*norm_u*idv(M_hSize)/(2*exprCoeffDiffusion);
    auto xi_Re = min( cst(1.), Re );
    return xi_Re;
}

template<typename MeshType, uint16_type OrderPoly>
template<typename ExprConvectiontype, typename ExprCoeffDiffusionType>
auto
StabilizationGLSParameter<MeshType,OrderPoly>::localPeRe( ExprConvectiontype const& exprConvection, ExprCoeffDiffusionType const& exprCoeffDiffusion, mpl::int_<1> /**/ ) const
{
    auto norm_u = norm2( exprConvection );
    auto Re = norm_u/(4*idv(M_sqrtLambdaK)*exprCoeffDiffusion);
    auto xi_Re = min( cst(1.), Re );
    return xi_Re;
}

template<typename MeshType, uint16_type OrderPoly>
template<typename ExprConvectiontype, typename ExprCoeffDiffusionType>
auto
StabilizationGLSParameter<MeshType,OrderPoly>::localPeRe( ExprConvectiontype const& exprConvection, ExprCoeffDiffusionType const& exprCoeffDiffusion, mpl::int_<2> /**/ ) const
{
    auto psi=min(cst(1.),(1./3.)*idv(M_hSize)*norm2(exprConvection)/(2*exprCoeffDiffusion));
    return psi;
}


template<typename MeshType, uint16_type OrderPoly>
template<typename ExprConvectiontype, typename ExprCoeffDiffusionType>
auto
StabilizationGLSParameter<MeshType,OrderPoly>::tau( ExprConvectiontype const& exprConvection, ExprCoeffDiffusionType const& exprCoeffDiffusion, mpl::int_<0> /**/ ) const
{
    auto norm_u = norm2( exprConvection );
    auto xi_Re = this->localPeRe( exprConvection, exprCoeffDiffusion, mpl::int_<0>() );
    auto tau = idv(M_hSize)*xi_Re/( max( 1e-10, 2*norm_u) );
    return tau;
}

template<typename MeshType, uint16_type OrderPoly>
template<typename ExprConvectiontype, typename ExprCoeffDiffusionType>
auto
StabilizationGLSParameter<MeshType,OrderPoly>::tau( ExprConvectiontype const& exprConvection, ExprCoeffDiffusionType const& exprCoeffDiffusion, mpl::int_<1> /**/ ) const
{
    auto norm_u = norm2( exprConvection );
    auto xi_Re = this->localPeRe( exprConvection, exprCoeffDiffusion, mpl::int_<1>() );
    auto tau = 2*xi_Re/( max( 1e-10, idv(M_sqrtLambdaK)*norm_u) );
    return tau;
}

template<typename MeshType, uint16_type OrderPoly>
template<typename ExprConvectiontype, typename ExprCoeffDiffusionType>
auto
StabilizationGLSParameter<MeshType,OrderPoly>::tau( ExprConvectiontype const& exprConvection, ExprCoeffDiffusionType const& exprCoeffDiffusion, mpl::int_<2> /**/ ) const
{
    auto psi = this->localPeRe( exprConvection, exprCoeffDiffusion, mpl::int_<2>() );
#if 0
    auto chiConv = chi(norm2(exprConvection)>cst(1e-5));
    auto tau = (psi*h()/(2*norm2(exprConvection) + (1-chiConv) ) )*chiConv;
#else
    auto tau = psi*idv(M_hSize)/( max( 2*norm2(exprConvection), 1e-10) );
#endif
    return tau;
}

template<typename MeshType, uint16_type OrderPoly>
template<int ParameterType, typename ExprConvectiontype, typename ExprCoeffDiffusionType>
auto
StabilizationGLSParameter<MeshType,OrderPoly>::delta( ExprConvectiontype const& exprConvection, ExprCoeffDiffusionType const& exprCoeffDiffusion ) const
{
    auto norm_u = norm2( exprConvection );
    auto xi_Re = localPeRe( exprConvection, exprCoeffDiffusion, mpl::int_<ParameterType>() );
    auto myDelta = cst(1.)*norm_u*idv(M_hSize)*xi_Re;
    return myDelta;
}
#endif

}  // namespace FeelModels

} // namespace Feel

#endif