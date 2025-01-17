/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#pragma once
#include <SofaGeneralLinearSolver/config.h>

#include <sofa/core/behavior/LinearSolver.h>
#include <SofaBaseLinearSolver/MatrixLinearSolver.h>
#include <SofaBaseLinearSolver/SparseMatrix.h>
#include <SofaBaseLinearSolver/FullMatrix.h>
#include <cmath>
#include <sofa/type/Mat.h>

namespace sofa::component::linearsolver
{

/// Simple bloc full matrix container (used for InvMatrixType)
template< std::size_t N, typename T>
class BlocFullMatrix : public defaulttype::BaseMatrix
{
public:

    enum { BSIZE = N };
    typedef T Real;

    class TransposedBloc{

    public:
        const type::Mat<BSIZE,BSIZE,Real>& m;

        TransposedBloc(const sofa::type::Mat<BSIZE, BSIZE, Real>& m_a) : m(m_a){
}

        type::Vec<BSIZE,Real> operator*(const type::Vec<BSIZE,Real>& v)
        {
            return m.multTranspose(v);
        }

        type::Mat<BSIZE,BSIZE,Real> operator-() const
        {
            return -m.transposed();
        }
    };

    class Bloc : public type::Mat<BSIZE,BSIZE,Real>
    {
    public:
        Index Nrows() const;
        Index Ncols() const;
        void resize(Index, Index);
        const T& element(Index i, Index j) const;
        void set(Index i, Index j, const T& v);
        void add(Index i, Index j, const T& v);
        void operator=(const type::Mat<BSIZE,BSIZE,Real>& v)
        {
            type::Mat<BSIZE,BSIZE,Real>::operator=(v);
        }
        type::Mat<BSIZE,BSIZE,Real> operator-() const
        {
            return type::Mat<BSIZE,BSIZE,Real>::operator-();
        }
        type::Mat<BSIZE,BSIZE,Real> operator-(const type::Mat<BSIZE,BSIZE,Real>& m) const
        {
            return type::Mat<BSIZE,BSIZE,Real>::operator-(m);
        }
        type::Vec<BSIZE,Real> operator*(const type::Vec<BSIZE,Real>& v)
        {
            return type::Mat<BSIZE,BSIZE,Real>::operator*(v);
        }
        type::Mat<BSIZE,BSIZE,Real> operator*(const type::Mat<BSIZE,BSIZE,Real>& m)
        {
            return type::Mat<BSIZE,BSIZE,Real>::operator*(m);
        }
        type::Mat<BSIZE,BSIZE,Real> operator*(const Bloc& m)
        {
            return type::Mat<BSIZE,BSIZE,Real>::operator*(m);
        }
        type::Mat<BSIZE,BSIZE,Real> operator*(const TransposedBloc& mt)
        {
            return type::Mat<BSIZE,BSIZE,Real>::operator*(mt.m.transposed());
        }
        TransposedBloc t() const;
        Bloc i() const;
    };
    typedef Bloc SubMatrixType;
    typedef FullMatrix<T> InvMatrixType;
    // return the dimension of submatrices when requesting a given size
    static Index getSubMatrixDim(Index);

protected:
    Bloc* data;
    Index nTRow,nTCol;
    Index nBRow,nBCol;
    Index allocsize;

public:

    BlocFullMatrix();

    BlocFullMatrix(Index nbRow, Index nbCol);

    ~BlocFullMatrix() override;

    Bloc* ptr() { return data; }
    const Bloc* ptr() const { return data; }

    const Bloc& bloc(Index bi, Index bj) const;

    Bloc& bloc(Index bi, Index bj);

    void resize(Index nbRow, Index nbCol) override;

    Index rowSize(void) const override;

    Index colSize(void) const override;

    SReal element(Index i, Index j) const override;

    const Bloc& asub(Index bi, Index bj, Index, Index) const;

    const Bloc& sub(Index i, Index j, Index, Index) const;

    Bloc& asub(Index bi, Index bj, Index, Index);

    Bloc& sub(Index i, Index j, Index, Index);

    template<class B>
    void getSubMatrix(Index i, Index j, Index nrow, Index ncol, B& m);

    template<class B>
    void getAlignedSubMatrix(Index bi, Index bj, Index nrow, Index ncol, B& m);

    template<class B>
    void setSubMatrix(Index i, Index j, Index nrow, Index ncol, const B& m);

    template<class B>
    void setAlignedSubMatrix(Index bi, Index bj, Index nrow, Index ncol, const B& m);

    void set(Index i, Index j, double v) override;

    void add(Index i, Index j, double v) override;

    void clear(Index i, Index j) override;

    void clearRow(Index i) override;

    void clearCol(Index j) override;

    void clearRowCol(Index i) override;

    void clear() override;

    template<class Real2>
    FullVector<Real2> operator*(const FullVector<Real2>& v) const
    {
        FullVector<Real2> res(rowSize());
        for (Index bi=0; bi<nBRow; ++bi)
        {
            Index bj = 0;
            for (Index i=0; i<BSIZE; ++i)
            {
                Real r = 0;
                for (Index j=0; j<BSIZE; ++j)
                {
                    r += bloc(bi,bj)[i][j] * v[(bi + bj - 1)*BSIZE + j];
                }
                res[bi*BSIZE + i] = r;
            }
            for (++bj; bj<nBCol; ++bj)
            {
                for (Index i=0; i<BSIZE; ++i)
                {
                    Real r = 0;
                    for (Index j=0; j<BSIZE; ++j)
                    {
                        r += bloc(bi,bj)[i][j] * v[(bi + bj - 1)*BSIZE + j];
                    }
                    res[bi*BSIZE + i] += r;
                }
            }
        }
        return res;
    }


    static const char* Name();
};

template< std::size_t N, typename T>
class BlockVector : public FullVector<T>
{
public:
    typedef FullVector<T> Inherit;
    typedef T Real;
    typedef typename Inherit::Index Index;

    typedef typename Inherit::value_type value_type;
    typedef typename Inherit::Size Size;
    typedef typename Inherit::iterator iterator;
    typedef typename Inherit::const_iterator const_iterator;

    class Bloc : public type::Vec<N,T>
    {
    public:
        Index Nrows() const { return N; }
        void resize(Index) { this->clear(); }
        void operator=(const type::Vec<N,T>& v)
        {
            type::Vec<N,T>::operator=(v);
        }
        void operator=(int v)
        {
            type::Vec<N,T>::fill((float)v);
        }
        void operator=(float v)
        {
            type::Vec<N,T>::fill(v);
        }
        void operator=(double v)
        {
            type::Vec<N,T>::fill(v);
        }
    };

    typedef Bloc SubVectorType;

public:

    BlockVector();

    explicit BlockVector(Index n);

    virtual ~BlockVector();

    const Bloc& sub(Index i, Index) const
    {
        return (const Bloc&)*(this->ptr()+i);
    }

    Bloc& sub(Index i, Index);

    const Bloc& asub(Index bi, Index) const;

    Bloc& asub(Index bi, Index);
};

/// Simple BTD matrix container
template< std::size_t N, typename T>
class BTDMatrix : public defaulttype::BaseMatrix
{
public:
    enum { BSIZE = N };
    typedef T Real;
    typedef typename defaulttype::BaseMatrix::Index Index;

    class TransposedBloc
    {
    public:
        const type::Mat<BSIZE,BSIZE,Real>& m;
        TransposedBloc(const type::Mat<BSIZE,BSIZE,Real>& m) : m(m) {}
        type::Vec<BSIZE,Real> operator*(const type::Vec<BSIZE,Real>& v)
        {
            return m.multTranspose(v);
        }
        type::Mat<BSIZE,BSIZE,Real> operator-() const
        {
            type::Mat<BSIZE,BSIZE,Real> r;
            for (Index i=0; i<BSIZE; i++)
                for (Index j=0; j<BSIZE; j++)
                    r[i][j]=-m[j][i];
            return r;
        }
    };

    class Bloc : public type::Mat<BSIZE,BSIZE,Real>
    {
    public:
        Index Nrows() const { return BSIZE; }
        Index Ncols() const { return BSIZE; }
        void resize(Index, Index)
        {
            this->clear();
        }
        const T& element(Index i, Index j) const { return (*this)[i][j]; }
        void set(Index i, Index j, const T& v) { (*this)[i][j] = v; }
        void add(Index i, Index j, const T& v) { (*this)[i][j] += v; }
        void operator=(const type::Mat<BSIZE,BSIZE,Real>& v)
        {
            type::Mat<BSIZE,BSIZE,Real>::operator=(v);
        }
        type::Mat<BSIZE,BSIZE,Real> operator-() const
        {
            type::Mat<BSIZE,BSIZE,Real> r;
            for (Index i=0; i<BSIZE; i++)
                for (Index j=0; j<BSIZE; j++)
                    r[i][j]=-(*this)[i][j];
            return r;
        }
        type::Mat<BSIZE,BSIZE,Real> operator-(const type::Mat<BSIZE,BSIZE,Real>& m) const
        {
            return type::Mat<BSIZE,BSIZE,Real>::operator-(m);
        }
        type::Vec<BSIZE,Real> operator*(const type::Vec<BSIZE,Real>& v)
        {
            return type::Mat<BSIZE,BSIZE,Real>::operator*(v);
        }
        type::Mat<BSIZE,BSIZE,Real> operator*(const type::Mat<BSIZE,BSIZE,Real>& m)
        {
            return type::Mat<BSIZE,BSIZE,Real>::operator*(m);
        }
        type::Mat<BSIZE,BSIZE,Real> operator*(const Bloc& m)
        {
            return type::Mat<BSIZE,BSIZE,Real>::operator*(m);
        }
        type::Mat<BSIZE,BSIZE,Real> operator*(const TransposedBloc& mt)
        {
            return type::Mat<BSIZE,BSIZE,Real>::operator*(mt.m.transposed());
        }
        TransposedBloc t() const
        {
            return TransposedBloc(*this);
        }
        Bloc i() const
        {
            Bloc r;
            r.invert(*this);
            return r;
        }
    };

    typedef Bloc SubMatrixType;
    typedef sofa::type::Mat<N,N,Real> BlocType;
    typedef BlocFullMatrix<N,T> InvMatrixType;
    // return the dimension of submatrices when requesting a given size
    static Index getSubMatrixDim(Index) { return BSIZE; }

protected:
    Bloc* data;
    Index nTRow,nTCol;
    Index nBRow,nBCol;
    Index allocsize;

public:

    BTDMatrix();

    BTDMatrix(Index nbRow, Index nbCol);

    ~BTDMatrix() override;

    Bloc* ptr() { return data; }
    const Bloc* ptr() const { return data; }

    //Real* operator[](Index i)
    //{
    //    return data+i*pitch;
    //}
    const Bloc& bloc(Index bi, Index bj) const;

    Bloc& bloc(Index bi, Index bj);

    void resize(Index nbRow, Index nbCol) override;

    Index rowSize(void) const override;

    Index colSize(void) const override;

    SReal element(Index i, Index j) const override;

    const Bloc& asub(Index bi, Index bj, Index, Index) const;

    const Bloc& sub(Index i, Index j, Index, Index) const;

    Bloc& asub(Index bi, Index bj, Index, Index);

    Bloc& sub(Index i, Index j, Index, Index);

    template<class B>
    void getSubMatrix(Index i, Index j, Index nrow, Index ncol, B& m);

    template<class B>
    void getAlignedSubMatrix(Index bi, Index bj, Index nrow, Index ncol, B& m);

    template<class B>
    void setSubMatrix(Index i, Index j, Index nrow, Index ncol, const B& m);

    template<class B>
    void setAlignedSubMatrix(Index bi, Index bj, Index nrow, Index ncol, const B& m);

    void set(Index i, Index j, double v) override;

    void add(Index i, Index j, double v) override;

    void clear(Index i, Index j) override;

    void clearRow(Index i) override;

    void clearCol(Index j) override;

    void clearRowCol(Index i) override;

    void clear() override;

    template<class Real2>
    FullVector<Real2> operator*(const FullVector<Real2>& v) const
    {
        FullVector<Real2> res(rowSize());
        for (Index bi=0; bi<nBRow; ++bi)
        {
            Index b0 = (bi > 0) ? 0 : 1;
            Index b1 = ((bi < nBRow - 1) ? 3 : 2);
            for (Index i=0; i<BSIZE; ++i)
            {
                Real r = 0;
                for (Index bj = b0; bj < b1; ++bj)
                {
                    for (Index j=0; j<BSIZE; ++j)
                    {
                        r += data[bi*3+bj][i][j] * v[(bi + bj - 1)*BSIZE + j];
                    }
                }
                res[bi*BSIZE + i] = r;
            }
        }
        return res;
    }

    static const char* Name();
};


/// Linear system solver using Thomas Algorithm for Block Tridiagonal matrices
///
/// References:
/// Conte, S.D., and deBoor, C. (1972). Elementary Numerical Analysis. McGraw-Hill, New York
/// http://en.wikipedia.org/wiki/Tridiagonal_matrix_algorithm
/// http://www.cfd-online.com/Wiki/Tridiagonal_matrix_algorithm_-_TDMA_(Thomas_algorithm)
/// http://www4.ncsu.edu/eos/users/w/white/www/white/ma580/chap2.5.PDF
template<class Matrix, class Vector>
class BTDLinearSolver : public sofa::component::linearsolver::MatrixLinearSolver<Matrix,Vector>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE2(BTDLinearSolver, Matrix, Vector), SOFA_TEMPLATE2(sofa::component::linearsolver::MatrixLinearSolver, Matrix, Vector));

    Data<bool> f_verbose; ///< Dump system state at each iteration
    Data<bool> problem; ///< display debug informations about subpartSolve computation
    Data<bool> subpartSolve; ///< Allows for the computation of a subpart of the system

    Data<bool> verification; ///< verification of the subpartSolve
    Data<bool> test_perf; ///< verification of performance

    typedef typename Vector::SubVectorType SubVector;
    typedef typename Matrix::SubMatrixType SubMatrix;
    typedef typename Vector::Real Real;
    typedef typename Matrix::BlocType BlocType;
    typedef typename sofa::SignedIndex Index;
    typedef std::list<Index> ListIndex;
    typedef std::pair<Index,Index> IndexPair;
    typedef std::map<IndexPair, SubMatrix> MysparseM;
    typedef typename std::map<IndexPair, SubMatrix>::iterator MysparseMit;

    //type::vector<SubMatrix> alpha;
    type::vector<SubMatrix> alpha_inv;
    type::vector<SubMatrix> lambda;
    type::vector<SubMatrix> B;
    typename Matrix::InvMatrixType Minv;  //inverse matrix


    //////////////////////////// for subpartSolve
    MysparseM H; // force transfer
    MysparseMit H_it;
    Vector bwdContributionOnLH;  //
    Vector fwdContributionOnRH;

    Vector _rh_buf;		 //				// buf the right hand term
    //Vector _df_buf;		 //
    SubVector _acc_rh_bloc;		// accumulation of rh through the browsing of the structure
    SubVector _acc_lh_bloc;		// accumulation of lh through the browsing of the strucutre
    Index	current_bloc, first_block;
    std::vector<SubVector> Vec_dRH;			// buf the dRH on block that are not current_bloc...
    ////////////////////////////

    type::vector<Index> nBlockComputedMinv;
    Vector Y;

    Data<int> f_blockSize; ///< dimension of the blocks in the matrix
protected:
    BTDLinearSolver()
        : f_verbose( initData(&f_verbose,false,"verbose","Dump system state at each iteration") )
        , problem(initData(&problem, false,"showProblem", "display debug informations about subpartSolve computation") )
        , subpartSolve(initData(&subpartSolve, false,"subpartSolve", "Allows for the computation of a subpart of the system") )
        , verification(initData(&verification, false,"verification", "verification of the subpartSolve"))
        , test_perf(initData(&test_perf, false,"test_perf", "verification of performance"))
        , f_blockSize( initData(&f_blockSize,6,"blockSize","dimension of the blocks in the matrix") )
    {
        Index bsize = Matrix::getSubMatrixDim(0);
        if (bsize > 0)
        {
            // the template uses fixed bloc size
            f_blockSize.setValue((int)bsize);
            f_blockSize.setReadOnly(true);
        }
    }
public:
    void my_identity(SubMatrix& Id, const Index size_id);

    void invert(SubMatrix& Inv, const BlocType& m);

    void invert(Matrix& M) override;

    void computeMinvBlock(Index i, Index j);

    double getMinvElement(Index i, Index j);

    /// Solve Mx=b
    void solve (Matrix& /*M*/, Vector& x, Vector& b) override;



    /// Multiply the inverse of the system matrix by the transpose of the given matrix, and multiply the result with the given matrix J
    ///
    /// @param result the variable where the result will be added
    /// @param J the matrix J to use
    /// @return false if the solver does not support this operation, of it the system matrix is not invertible
    bool addJMInvJt(defaulttype::BaseMatrix* result, defaulttype::BaseMatrix* J, double fact) override;


    /// Init the partial solve
    void init_partial_solve() override;

    using MatrixLinearSolver<Matrix,Vector>::partial_solve;
    /// partial solve :
    /// b is accumulated
    /// db is a sparse vector that is added to b
    /// partial_x is a sparse vector (with sparse map given) that provide the result of M x = b+db
    /// Solve Mx=b
    //void partial_solve_old(ListIndex&  Iout, ListIndex&  Iin , bool NewIn);
    void partial_solve(ListIndex&  Iout, ListIndex&  Iin , bool NewIn) override;



    void init_partial_inverse(const Index &nb, const Index &bsize);



    template<class RMatrix, class JMatrix>
    bool addJMInvJt(RMatrix& result, JMatrix& J, double fact);



private:


    Index _indMaxNonNullForce; // point with non null force which index is the greatest and for which globalAccumulate was not proceed

    Index _indMaxFwdLHComputed;  // indice of node from which bwdLH is accurate

    /// private functions for partial solve
    /// step1=> accumulate RH locally for the InBloc (only if a new force is detected on RH)
    void bwdAccumulateRHinBloc(Index indMaxBloc);   // indMaxBloc should be equal to _indMaxNonNullForce


    /// step2=> accumulate LH globally to step down the value of current_bloc to 0
    void bwdAccumulateLHGlobal( );


    /// step3=> accumulate RH globally to step up the value of current_bloc to the smallest value needed in OutBloc
    void fwdAccumulateRHGlobal(Index indMinBloc);


    /// step4=> compute solution for the indices in the bloc
    /// (and accumulate the potential local dRH (set in Vec_dRH) [set in step1] that have not been yet taken into account by the global bwd and fwd
    void fwdComputeLHinBloc(Index indMaxBloc);


};

#if  !defined(SOFA_COMPONENT_LINEARSOLVER_BTDLINEARSOLVER_CPP)
extern template class SOFA_SOFAGENERALLINEARSOLVER_API BTDLinearSolver< BTDMatrix<6, double>, BlockVector<6, double> >;



#endif

} //namespace sofa::component::linearsolver
