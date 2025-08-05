// ---------------------------------------------------------------------------------------------------------------------------------
//  _     _ __  __       _   _         _     
// | |   | |  \/  |     | | | |       | |    
// | |   | | \  / | __ _| |_| |__     | |__  
//  \ \ / /| |\/| |/ _` | __| '_ \    | '_ \ 
//   \ V / | |  | | (_| | |_| | | | _ | | | |
//    \_/  |_|  |_|\__,_|\__|_| |_|(_)|_| |_|
//                                           
//                                           
//
// Generic 2-dimensional NxM matrix/vector mathematics class specialized for 3D usage
//
// Best viewed with 8-character tabs and (at least) 132 columns
//
// ---------------------------------------------------------------------------------------------------------------------------------
//
// Originally released under a custom license.
// This historical re-release is provided under the MIT License.
// See the LICENSE file in the repo root for details.
//
// https://github.com/nettlep
//
// ---------------------------------------------------------------------------------------------------------------------------------
// Originally created on 12/06/2000 by Paul Nettle
//
// Copyright 2000, Fluid Studios, Inc., all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------
//
// IMPORTANT NOTE!
//
// Due to the nature of this class being a single solution for all 2D matrices of NxM size (which includes matrices, vectors &
// points) there are two cases where the class's behavior does not make as much sense as you might like. These two cases are in
// reference to the >> and ^ operators.
//
// A multiplication operator is commonly used for component-wise multiplication, dot products and matrix concatenation. Since the
// class supports the operators [+, -, /, *, +=, -=, /=, *=] it was prudent to maintain consistency and let operator* and
// operator *= work as component-wise multiplication. So what operators should be used for dot products and concatenation? I have
// chosen >> for concatenation, because it helps to serve as a reminder in which direction the operation's associativity goes (from
// left-to-right). The dot product uses operator ^ (simply for lack of a better operator.)
//
// The problem you may find then, is that operator >> and operator ^ have lower precedence than addition & subtraction. Without the
// use of parenthesis around these multiplicative operations, you will not get the result you expect.
//
// Furthermore, operator ^ (dot product) has a lower precedence than even operators [<, <=, >, >=]. This can cause prolems in cases
// like:  "if (v1 ^ v2 > 0)".
//
// If you do not like the operators I have chosen, feel free to modify them. Or you may chose to perform these operations through
// the function calls dot(), concat(), cross(), etc.
//
// Consider yourself warned.
//
// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef	_H_VMATH
#define _H_VMATH

// --------------------------------------------------------------------------------------------------------------------------------
// Often times, it's better to use the compiler to generate an error rather than a runtime assert. The following metaprogram (if
// you want to call it that) provides nice compile-time errors on a false condition.
//
// Note that the compiler will probably complain about a funciton that doesn't exist, which is fine. The fact still remains that
// the compiler DOES generate an error that points to the line containing the TemplateAssert macro call.
//
// Also note that much like runtime asserts that get conditionally compiled out during non-debug builds, this gets 'optimized' out
// always (provided it doesn't generate a compiler error.)
// --------------------------------------------------------------------------------------------------------------------------------

template <bool B> struct templateAssert {};
struct templateAssert<true> {static void there_has_been_a_compile_time_assertion() {};};

#define	TemplateAssert(__a) {const bool __b = (__a) ? true:false; templateAssert<__b>::there_has_been_a_compile_time_assertion();}

// --------------------------------------------------------------------------------------------------------------------------------
// Define a matrix that is N columns wide and M rows tall. This matrix is row-major.
//
// N can be thought of as the [N]umber of elements per vector, and M can be thought of as the number of vectors in the matrix
// --------------------------------------------------------------------------------------------------------------------------------

template <unsigned int N, unsigned int M, class T = FLT>
class	matrix
{
public:
				// Default constructor

				matrix() {}

				// Copy constructor

				matrix(const matrix &m)
				{
					memcpy(_data, m._data, N*M*sizeof(T));
				}

				// Initialize with three vectors

				matrix(const matrix<N, 1, T> &xv, const matrix<N, 1, T> &yv, const matrix<N, 1, T> &zv)
				{
					setXVector(xv);
					setYVector(yv);
					setZVector(zv);
				}

				// Initialize with four values (useful for vectors)

				matrix(const T &xv, const T &yv, const T &zv, const T &wv)
				{
					// This assertion guarantees that they are initializing the entire vector

					TemplateAssert(N == 4 && M == 1);

					x() = xv;
					y() = yv;
					z() = zv;
					w() = wv;
				}

				// Initialize with three values (useful for vectors)

				matrix(const T &xv, const T &yv, const T &zv)
				{
					// This assertion guarantees that they are initializing the entire vector

					TemplateAssert(N == 3 && M == 1);

					x() = xv;
					y() = yv;
					z() = zv;
				}

				// Initialize with two values (useful for vectors)

				matrix(const T &xv, const T &yv)
				{
					// This assertion guarantees that they are initializing the entire vector

					TemplateAssert(N == 2 && M == 1);

					x() = xv;
					y() = yv;
				}

				// These are handy 2D/3D/4D casts

				operator matrix<2, 1, T>()
				{
					TemplateAssert(M == 1);

					matrix<2, 1, T>	result;
					result.fill(0);
					
					int	c = N;
					if (c > 2) c = 2;

					for (unsigned int i = 0; i < c; i++) result(i,0) = (*this)(i,0);
					return result;
				}

				operator matrix<3, 1, T>()
				{
					TemplateAssert(M == 1);

					matrix<3, 1, T>	result;
					result.fill(0);
					
					int	c = N;
					if (c > 3) c = 3;

					for (unsigned int i = 0; i < c; i++) result(i,0) = (*this)(i,0);
					return result;
				}

				operator matrix<4, 1, T>()
				{
					TemplateAssert(M == 1);

					matrix<4, 1, T>	result;
					result.fill(0);
					result.w() = (T) 1;
					
					int	c = N;
					if (c > 4) c = 4;

					for (unsigned int i = 0; i < c; i++) result(i,0) = (*this)(i,0);
					return result;
				}

				// Return a zero'd matrix

static		matrix		zero()
				{
					matrix	result;
					result.fill((T) 0);
					return result;
				}

				// The infamous 'operator='

inline		matrix		&operator =(const matrix &m)
				{
					if (&m != this) memcpy(_data, m._data, N*M*sizeof(T));
					return *this;
				}

				// Indexing: format = i down, j across

inline	const	T		&operator()(const unsigned int i, const unsigned int j) const
				{
					return _data[j*N+i];
				}

				// Indexing: format = i down, j across

inline		T		&operator()(const unsigned int i, const unsigned int j)
				{
					return _data[j*N+i];
				}

				// Matrix concatenation
				//
				// Specialized to follow the rules of matrix multiplication, for NxM * OxP:
				//   where M must be equal to O and resulting matrix is NxP. Otherwise, a
				//   compiler error will occur.
				//
				// Note that we use the >> operator. This is because of the lack of available
				//   operators, and also it serves as a reminder that the operations are from
				//   left-to-right (the convenient way.)

#ifndef _MSC_VER
template <unsigned int P>
inline	const	matrix<N, P, T>	operator >>(const matrix<M, P, T> &m) const	{return concat(m);}
template <unsigned int P>
inline				operator >>=(const matrix<M, P, T> &m)		{*this = concat(m);}
template <unsigned int P>
inline	const	matrix<N, P, T>	concat(const matrix<M, P, T> &m) const
				{
					matrix<N, P, T>	result;
					result.fill((T) 0);

					for (unsigned int i = 0; i < N; i++)
					for (unsigned int j = 0; j < M; j++)
					for (unsigned int k = 0; k < P; k++)
					result(i,j) += (*this)(i,k) * m(k,j);

					return result;
				}
#else

// We're doing matrix*vector, which, as far as I can tell, doesn't work the same as vector*vector. Though, it would work the same
// if it was vector*matrix... so this sucker is specialized for that purpose

inline	const	matrix<N, 1, T>	operator >>(const matrix<M, 1, T> &m) const	{return concat(m);}
inline				operator >>=(const matrix<M, 1, T> &m)		{*this = concat(m);}
inline	const	matrix<N, 1, T>	concat(const matrix<M, 1, T> &m) const
				{
					matrix<N, 1, T>	result;
					result.fill((T) 0);

					for (unsigned int i = 0; i < N; i++)
					for (unsigned int j = 0; j < M; j++)
					result(i,0) += (*this)(j,i) * m(j,0);

					return result;
				}

inline	const	matrix<N, 2, T>	operator >>(const matrix<M, 2, T> &m) const	{return concat(m);}
inline				operator >>=(const matrix<M, 2, T> &m)		{*this = concat(m);}
inline	const	matrix<N, 2, T>	concat(const matrix<M, 2, T> &m) const
				{
					matrix<N, 2, T>	result;
					result.fill((T) 0);

					for (unsigned int i = 0; i < N; i++)
					for (unsigned int j = 0; j < M; j++)
					for (unsigned int k = 0; k < 2; k++)
					result(i,j) += (*this)(i,k) * m(k,j);

					return result;
				}

inline	const	matrix<N, 3, T>	operator >>(const matrix<M, 3, T> &m) const	{return concat(m);}
inline				operator >>=(const matrix<M, 3, T> &m)		{*this = concat(m);}
inline	const	matrix<N, 3, T>	concat(const matrix<M, 3, T> &m) const
				{
					matrix<N, 3, T>	result;
					result.fill((T) 0);

					for (unsigned int i = 0; i < N; i++)
					for (unsigned int j = 0; j < M; j++)
					for (unsigned int k = 0; k < 3; k++)
					result(i,j) += (*this)(i,k) * m(k,j);

					return result;
				}

inline	const	matrix<N, 4, T>	operator >>(const matrix<M, 4, T> &m) const	{return concat(m);}
inline				operator >>=(const matrix<M, 4, T> &m)		{*this = concat(m);}
inline	const	matrix<N, 4, T>	concat(const matrix<M, 4, T> &m) const
				{
					matrix<N, 4, T>	result;
					result.fill((T) 0);

					for (unsigned int i = 0; i < N; i++)
					for (unsigned int j = 0; j < M; j++)
					for (unsigned int k = 0; k < 4; k++)
					result(i,j) += (*this)(i,k) * m(k,j);

					return result;
				}
#endif
				// 3D Vector cross product
				//
				// Note that the cross product is specifically a 3-dimensional operation. So this routine
				// will fill the remaining elements (if any) with the contents of this->_data[...]
				//
				// Specialized for Nx1 matrices (i.e. vectors)

inline		void		cross(const matrix<N, 1, T> &m)
				{
					TemplateAssert(N >= 3);

					T	tx = y() * m.z() - z() * m.y();
					T	ty = z() * m.x() - x() * m.z();
					T	tz = x() * m.y() - y() * m.x();
					x() = tx;
					y() = ty;
					z() = tz;
				}

inline	const	matrix<N, 1, T>	operator %(const matrix<N, 1, T> &m) const
				{
					matrix<N, 1, T>	result = *this;
					result.cross(m);
					return result;
				}

inline	const	matrix		operator %=(const matrix &m)
				{
					cross(m);
					return *this;
				}

				// Vector dot product
				//
				// Specialized for Nx1 matrices (i.e. vectors)

inline	const	T		dot(const matrix<N, 1, T> &m) const
				{
					T	result = (T) 0;
					for (unsigned int i = 0; i < N; i++) result += _data[i] * m.data()[i];
					return result;
				}

inline	const	T		operator ^(const matrix<N, 1, T> &m) const
				{
					return dot(m);
				}

				// Component-wise multiplication with matrix

inline	const	matrix		operator *(const matrix &m) const
				{
					matrix	result;
					for (unsigned int i = 0; i < N*M; i++) result._data[i] = _data[i] * m._data[i];
					return result;
				}

				// Component-wise multiplication with scalar

inline	const	matrix		operator *(const T &value) const
				{
					matrix	result;
					for (unsigned int i = 0; i < N*M; i++) result._data[i] = _data[i] * value;
					return result;
				}

				// Component-wise multiplication with matrix (into self)

inline	const	matrix		operator *=(const matrix &m)
				{
					for (unsigned int i = 0; i < N*M; i++) _data[i] *= m._data[i];
					return *this;
				}

				// Component-wise multiplication with scalar (into self)

inline	const	matrix		operator *=(const T &value)
				{
					for (unsigned int i = 0; i < N*M; i++) _data[i] *= value;
					return *this;
				}

				// Component-wise division with matrix

inline	const	matrix		operator /(const matrix &m) const
				{
					matrix	result;
					for (unsigned int i = 0; i < N*M; i++) result._data[i] = _data[i] / m._data[i];
					return result;
				}

				// Component-wise division with scalar

inline	const	matrix		operator /(const T &value) const
				{
					matrix	result;
					for (unsigned int i = 0; i < N*M; i++) result._data[i] = _data[i] / value;
					return result;
				}

				// Component-wise division with scalar (scalar / component)

inline	const	matrix		inverseDivide(const T &value) const
				{
					matrix	result;
					for (unsigned int i = 0; i < N*M; i++) result._data[i] = value / _data[i];
					return result;
				}

				// Component-wise division with matrix (into self)

inline	const	matrix		operator /=(const matrix &m)
				{
					for (unsigned int i = 0; i < N*M; i++) _data[i] /= m._data[i];
					return *this;
				}

				// Component-wise division with scalar (into self)

inline	const	matrix		operator /=(const T &value)
				{
					for (unsigned int i = 0; i < N*M; i++) _data[i] /= value;
					return *this;
				}

				// Component-wise addition with matrix

inline	const	matrix		operator +(const matrix &m) const
				{
					matrix	result;
					for (unsigned int i = 0; i < N*M; i++) result._data[i] = _data[i] + m._data[i];
					return result;
				}

				// Component-wise addition with scalar

inline	const	matrix		operator +(const T &value) const
				{
					matrix	result;
					for (unsigned int i = 0; i < N*M; i++) result._data[i] = _data[i] + value;
					return result;
				}

				// Component-wise addition with matrix (into self)

inline	const	matrix		operator +=(const matrix &m)
				{
					for (unsigned int i = 0; i < N*M; i++) _data[i] += m._data[i];
					return *this;
				}

				// Component-wise addition with scalar (into self)

inline	const	matrix		operator +=(const T &value)
				{
					for (unsigned int i = 0; i < N*M; i++) _data[i] += value;
					return *this;
				}

				// Component-wise negation

inline	const	matrix		operator -() const
				{
					matrix	result;
					for (unsigned int i = 0; i < N*M; i++) result._data[i] = -_data[i];
					return result;
				}

				// Component-wise subtraction with matrix

inline	const	matrix		operator -(const matrix &m) const
				{
					matrix	result;
					for (unsigned int i = 0; i < N*M; i++) result._data[i] = _data[i] - m._data[i];
					return result;
				}

				// Component-wise subtraction with scalar

inline	const	matrix		operator -(const T &value) const
				{
					matrix	result;
					for (unsigned int i = 0; i < N*M; i++) result._data[i] = _data[i] - value;
					return result;
				}

				// Component-wise subtraction with scalar (scalar - component)

inline	const	matrix		inverseSubtract(const T &value) const
				{
					matrix	result;
					for (unsigned int i = 0; i < N*M; i++) result._data[i] = value - _data[i];
					return result;
				}

				// Component-wise subtraction with matrix (into self)

inline	const	matrix		operator -=(const matrix &m)
				{
					for (unsigned int i = 0; i < N*M; i++) _data[i] -= m._data[i];
					return *this;
				}

				// Component-wise subtraction with scalar (into self)

inline	const	matrix		operator -=(const T &value)
				{
					for (unsigned int i = 0; i < N*M; i++) _data[i] -= value;
					return *this;
				}

				// Total all components

inline		T		total()
				{
					T	tot = (T) 0;
					for (unsigned int i = 0; i < N*M; i++) tot += _data[i];
					return tot;
				}

				// Comparison for equality

inline	const	bool		operator ==(const matrix &m) const
				{
					for (unsigned int i = 0; i < N*M; i++) if (_data[i] != m._data[i]) return false;
					return true;
				}

				// Comparison for inequality

inline	const	bool		operator !=(const matrix &m) const
				{
					return !(*this == m);
				}

inline	const	bool		operator <(const matrix &m) const
				{
					for (unsigned int i = 0; i < N*M; i++) if (_data[i] >= m._data[i]) return false;
					return true;
				}

inline	const	bool		operator <=(const matrix &m) const
				{
					for (unsigned int i = 0; i < N*M; i++) if (_data[i] > m._data[i]) return false;
					return true;
				}

inline	const	bool		operator >(const matrix &m) const
				{
					for (unsigned int i = 0; i < N*M; i++) if (_data[i] <= m._data[i]) return false;
					return true;
				}

inline	const	bool		operator >=(const matrix &m) const
				{
					for (unsigned int i = 0; i < N*M; i++) if (_data[i] < m._data[i]) return false;
					return true;
				}

				// Orthogonal transpose
				//
				// Note that matrix must be square (i.e. N == M)

inline		void		transpose()
				{
					TemplateAssert(N == M);

					// Transpose the matrix

					matrix result;
					for (unsigned int j = 0; j < M; j++)
					{
						for (unsigned int i = 0; i < N; i++)
						{
							result(j,i) = (*this)(i,j);
						}
					}
					*this = result;
				}

				// Returns determinant of the matrix (4x4 only)

inline		T		determinant()
				{
					TemplateAssert(N == 4 && M == 4);

					matrix	&m = *this;
					return	  (m(0,0) * m(1,1) - m(1,0) * m(0,1)) * (m(2,2) * m(3,3) - m(3,2) * m(2,3))
						- (m(0,0) * m(2,1) - m(2,0) * m(0,1)) * (m(1,2) * m(3,3) - m(3,2) * m(1,3))
						+ (m(0,0) * m(3,1) - m(3,0) * m(0,1)) * (m(1,2) * m(2,3) - m(2,2) * m(1,3))
						+ (m(1,0) * m(2,1) - m(2,0) * m(1,1)) * (m(0,2) * m(3,3) - m(3,2) * m(0,3))
						- (m(1,0) * m(3,1) - m(3,0) * m(1,1)) * (m(0,2) * m(2,3) - m(2,2) * m(0,3))
						+ (m(2,0) * m(3,1) - m(3,0) * m(2,1)) * (m(0,2) * m(1,3) - m(1,2) * m(0,3));
				}

				// Inverts the matrix (4x4 only)

inline		void		invert()
				{
					TemplateAssert(N == 4 && M == 4);

					T	d = determinant();
					if (d == 0.0) return;

					d = 1.0 / d;

					matrix	&m = *this;
					matrix	result;
					result(0,0) = d * (m(1,1) * (m(2,2) * m(3,3) - m(3,2) * m(2,3)) + m(2,1) * (m(3,2) * m(1,3) - m(1,2) * m(3,3)) + m(3,1) * (m(1,2) * m(2,3) - m(2,2) * m(1,3)));
					result(1,0) = d * (m(1,2) * (m(2,0) * m(3,3) - m(3,0) * m(2,3)) + m(2,2) * (m(3,0) * m(1,3) - m(1,0) * m(3,3)) + m(3,2) * (m(1,0) * m(2,3) - m(2,0) * m(1,3)));
					result(2,0) = d * (m(1,3) * (m(2,0) * m(3,1) - m(3,0) * m(2,1)) + m(2,3) * (m(3,0) * m(1,1) - m(1,0) * m(3,1)) + m(3,3) * (m(1,0) * m(2,1) - m(2,0) * m(1,1)));
					result(3,0) = d * (m(1,0) * (m(3,1) * m(2,2) - m(2,1) * m(3,2)) + m(2,0) * (m(1,1) * m(3,2) - m(3,1) * m(1,2)) + m(3,0) * (m(2,1) * m(1,2) - m(1,1) * m(2,2)));
					result(0,1) = d * (m(2,1) * (m(0,2) * m(3,3) - m(3,2) * m(0,3)) + m(3,1) * (m(2,2) * m(0,3) - m(0,2) * m(2,3)) + m(0,1) * (m(3,2) * m(2,3) - m(2,2) * m(3,3)));
					result(1,1) = d * (m(2,2) * (m(0,0) * m(3,3) - m(3,0) * m(0,3)) + m(3,2) * (m(2,0) * m(0,3) - m(0,0) * m(2,3)) + m(0,2) * (m(3,0) * m(2,3) - m(2,0) * m(3,3)));
					result(2,1) = d * (m(2,3) * (m(0,0) * m(3,1) - m(3,0) * m(0,1)) + m(3,3) * (m(2,0) * m(0,1) - m(0,0) * m(2,1)) + m(0,3) * (m(3,0) * m(2,1) - m(2,0) * m(3,1)));
					result(3,1) = d * (m(2,0) * (m(3,1) * m(0,2) - m(0,1) * m(3,2)) + m(3,0) * (m(0,1) * m(2,2) - m(2,1) * m(0,2)) + m(0,0) * (m(2,1) * m(3,2) - m(3,1) * m(2,2)));
					result(0,2) = d * (m(3,1) * (m(0,2) * m(1,3) - m(1,2) * m(0,3)) + m(0,1) * (m(1,2) * m(3,3) - m(3,2) * m(1,3)) + m(1,1) * (m(3,2) * m(0,3) - m(0,2) * m(3,3)));
					result(1,2) = d * (m(3,2) * (m(0,0) * m(1,3) - m(1,0) * m(0,3)) + m(0,2) * (m(1,0) * m(3,3) - m(3,0) * m(1,3)) + m(1,2) * (m(3,0) * m(0,3) - m(0,0) * m(3,3)));
					result(2,2) = d * (m(3,3) * (m(0,0) * m(1,1) - m(1,0) * m(0,1)) + m(0,3) * (m(1,0) * m(3,1) - m(3,0) * m(1,1)) + m(1,3) * (m(3,0) * m(0,1) - m(0,0) * m(3,1)));
					result(3,2) = d * (m(3,0) * (m(1,1) * m(0,2) - m(0,1) * m(1,2)) + m(0,0) * (m(3,1) * m(1,2) - m(1,1) * m(3,2)) + m(1,0) * (m(0,1) * m(3,2) - m(3,1) * m(0,2)));
					result(0,3) = d * (m(0,1) * (m(2,2) * m(1,3) - m(1,2) * m(2,3)) + m(1,1) * (m(0,2) * m(2,3) - m(2,2) * m(0,3)) + m(2,1) * (m(1,2) * m(0,3) - m(0,2) * m(1,3)));
					result(1,3) = d * (m(0,2) * (m(2,0) * m(1,3) - m(1,0) * m(2,3)) + m(1,2) * (m(0,0) * m(2,3) - m(2,0) * m(0,3)) + m(2,2) * (m(1,0) * m(0,3) - m(0,0) * m(1,3)));
					result(2,3) = d * (m(0,3) * (m(2,0) * m(1,1) - m(1,0) * m(2,1)) + m(1,3) * (m(0,0) * m(2,1) - m(2,0) * m(0,1)) + m(2,3) * (m(1,0) * m(0,1) - m(0,0) * m(1,1)));
					result(3,3) = d * (m(0,0) * (m(1,1) * m(2,2) - m(2,1) * m(1,2)) + m(1,0) * (m(2,1) * m(0,2) - m(0,1) * m(2,2)) + m(2,0) * (m(0,1) * m(1,2) - m(1,1) * m(0,2)));
					*this = result;
				}

				// Fill the matrix with a single value

inline		void		fill(const T &value)
				{
					T	*ptr = _data;
					for (unsigned int i = 0; i < N*M; i++, ptr++) *ptr = value;
				}

				// Generate identity matrix
				//
				// Note that matrix must be square (i.e. N == M)

static	const	matrix<N, M, T>	identity()
				{
					TemplateAssert(N == M);

					// Make it identity

					matrix<N, M, T>	result;
					T		*ptr = result._data;
					for (unsigned int j = 0; j < M; j++)
					{
						for (unsigned int i = 0; i < N; i++, ptr++)
						{
							if (i == j)	*ptr = (T) 1;
							else		*ptr = (T) 0;
						}
					}
					return result;
				}

				// Generate rotation matrix (3x3) for rotation about the X axis (i.e. rotation happens along the Y/Z plane)

static	const	matrix<N, M, T>	xRotation(const T &theta)
				{
					TemplateAssert(N >= 3 && M >= 3);

					// Start with identity

					matrix<N, M, T>	result = identity();

					// Fill it in

					T	ct = (T) cos((double) theta);
					T	st = (T) sin((double) theta);
					result(1,1) =  ct;
					result(2,1) =  st;
					result(1,2) = -st;
					result(2,2) =  ct;
					return result;
				}

				// Generate rotation matrix (3x3) for rotation about the Y axis (i.e. rotation happens along the X/Z plane)
				//
				// Note that the matrix must be a minimum of 3x3

static	const	matrix<N, M, T>	yRotation(const T &theta)
				{
					TemplateAssert(N >= 3 && M >= 3);

					// Start with identity

					matrix<N, M, T>	result = identity();

					// Fill it in

					T	ct = (T) cos((double) theta);
					T	st = (T) sin((double) theta);
					result(0,0) =  ct;
					result(2,0) =  st;
					result(0,2) = -st;
					result(2,2) =  ct;
					return result;
				}

				// Generate rotation matrix (3x3) for rotation about the Z axis (i.e. rotation happens along the X/Y plane)
				//
				// Note that this matrix is allowed to be only 2x2 as this is a 2-D rotation

static	const	matrix<N, M, T>	zRotation(const T &theta)
				{
					TemplateAssert(N >= 3 && M >= 3);

					// Start with identity

					matrix<N, M, T>	result = identity();

					// Fill it in

					T	ct = (T) cos((double) theta);
					T	st = (T) sin((double) theta);
					result(0,0) =  ct;
					result(1,0) = -st;
					result(0,1) =  st;
					result(1,1) =  ct;
					return result;
				}

				// Generate a concatenated rotation matrix (3x3) for rotation about all axes (i.e. arbitrary rotation)

static	const	matrix		rotation(const T &xTheta, const T &yTheta, const T &zTheta)
				{
					return zRotation(zTheta) >> yRotation(yTheta) >> xRotation(xTheta);
				}

				// Generate a 'look-at' matrix. Must be a 3x3 result because this routine uses a cross product

static	const	matrix<3, 3, T>	lookat(const matrix<3, 1, T> &v, const T &theta = (T) 0)
				{
					matrix<3, 1, T>	zAxis = v;
					zAxis.normalize();

					matrix<3, 1, T>	yAxis;
					yAxis.fill((T) 0);

					// Handle the degenerate case... (this acts exactly like 3DS-R4)

					if (!zAxis.x() && !zAxis.z())	yAxis.z() = -zAxis.y();
					else				yAxis.y() = (T) 1;

					matrix<3, 1, T>	xAxis = yAxis % zAxis;
					xAxis.normalize();

					yAxis = xAxis % zAxis;
					yAxis.normalize();
					yAxis = -yAxis;

					matrix<3, 3, T>	m(xAxis, yAxis, zAxis);
					return m >> zRotation(theta);
				}

				// Generate a scale matrix

static		matrix		scale(const matrix<N, 1, T> &m)
				{
					TemplateAssert(N <= M);

					matrix	result;
					result = identity();
					for (unsigned int i = 0; i < N; i++)
					{
						result(i,i) *= m(i,0);
					}
					return result;
				}

				// Generate a translation matrix

static		matrix		translation(const matrix<N, 1, T> &m)
				{
					TemplateAssert(M <= N);

					matrix	result;
					result = identity();
					for (unsigned int i = 0; i < M; i++)
					{
						result(N-1,i) += m(i,0);
					}
					return result;
				}

				// Generate a shear matrix

static		matrix		shear(const T x, const T y)
				{
					TemplateAssert(N > 1 && M > 1);

					matrix	result;
					result = identity();
					result(1,0) = x;
					result(0,1) = y;
					return result;
				}

				// Generate a (4x4) perspective projection matrix (as per D3D)

static	const	matrix<4, 4, T>	projectPerspectiveD3D(const T &fov, const T &aspect, const T &n, const T &f)
				{
					T	w  = 1 / tan(fov / (T) 2);
					T	h  = 1 / tan(fov / (T) 2);
					if (aspect > 1.0)	w /= aspect;
					else			h *= aspect;
					T	q  = f / (f - n);

					matrix<4, 4, T>	result;
					result.fill((T)0);
					result(0,0) = w;
					result(1,1) = h;
					result(2,2) = q;
					result(3,2) = -q*n;
					result(2,3) = 1;
					return result;
				}

				// Generate a (4x4) perspective projection matrix (as per Blinn)

static	const	matrix<4, 4, T>	projectPerspectiveBlinn(const T &fov, const T &aspect, const T &n, const T &f)
				{
					T	w  = cos(fov / (T) 2);
					T	h  = cos(fov / (T) 2);
					if (aspect > 1.0)	w /= aspect;
					else			h *= aspect;
					T	s  = sin(fov / (T) 2); // ???
					T	d  = (T) 1 - n/f;

					matrix<4, 4, T>	result;
					result.fill((T)0);
					result(0,0) = w;
					result(1,1) = h;
					result(2,2) = s / d;
					result(3,2) = -(s * n / d);
					result(2,3) = s;
					return result;
				}

				// Generate a (4x4) perspective projection matrix (as per glFrustum)

static	const	matrix<4, 4, T>	projectPerspectiveGlFrustum(const T &l, const T &r, const T &b, const T &t, const T &n, const T &f)
				{
					matrix<4, 4, T>	result;
					result.fill((T)0);
					result(0,0) = (2*n)/(r-l);
					result(2,0) = (r+l)/(r-l);
					result(1,1) = (2*n)/(t-b);
					result(2,1) = (t+b)/(t-b);
					result(2,2) = (-(f+n))/(f-n);
					result(3,2) = (-2*f*n)/(f-n);
					result(2,3) = -1;
					return result;
				}

				// Generate a (4x4) orthogonal projection matrix (as per glOrtho)

static	const	matrix<4, 4, T>	projectGlOrtho(const T &l, const T &r, const T &b, const T &t, const T &n, const T &f)
				{
					matrix<4, 4, T>	result;
					result.fill((T)0);
					result(0,0) = 2/(r-l);
					result(3,0) = -((r+l)/(r-l));
					result(1,1) = 2/(t-b);
					result(3,1) = -((t+b)/(t-b));
					result(2,2) = (-2)/(f-n);
					result(3,2) = -((f+n)/(f-n));
					result(3,3) = 1;
					return result;
				}

				// Generate a (4x4) orthogonal projection matrix (standard -- maps to z=0 plane)

static	const	matrix<4, 4, T>	projectOrtho(const T &xScale, const T &yScale)
				{
					matrix<4, 4, T>	result;
					result.fill((T)0);
					result(0,0) = xScale;
					result(1,1) = yScale;
					result(3,3) = 1;
					return result;
				}

				// Extract the X vector from the matrix
				//
				// Note that the matrix must be a minimum of 3x3

inline		matrix<N, 1, T>	extractXVector() const
				{
					TemplateAssert(M > 0);

					matrix<N, 1, T>	result;
					for (unsigned int i = 0; i < N; i++)
					{
						result(i,0) = (*this)(i,0);
					}
					return result;
				}

				// Extract the Y vector from the matrix
				//
				// Note that the matrix must be a minimum of 3x3

inline		matrix<N, 1, T>	extractYVector() const
				{
					TemplateAssert(M > 1);

					matrix<N, 1, T>	result;
					for (unsigned int i = 0; i < N; i++)
					{
						result(i,0) = (*this)(i,1);
					}
					return result;
				}

				// Extract the Z vector from the matrix
				//
				// Note that the matrix must be a minimum of 3x3

inline		matrix<N, 1, T>	extractZVector() const
				{
					TemplateAssert(M > 2);

					matrix<N, 1, T>	result;
					for (unsigned int i = 0; i < N; i++)
					{
						result(i,0) = (*this)(i,2);
					}
					return result;
				}

				// Replace the X vector within matrix
				//
				// Note that the matrix must be a minimum of 3x3

inline		void		setXVector(const matrix<N, 1, T> &m)
				{
					for (unsigned int i = 0; i < N; i++)
					{
						(*this)(i,0) = m(i,0);
					}
				}

				// Replace the Y vector within matrix
				//
				// Note that the matrix must be a minimum of 3x3

inline		void		setYVector(const matrix<N, 1, T> &m)
				{
					for (unsigned int i = 0; i < N; i++)
					{
						(*this)(i,1) = m(i,0);
					}
				}

				// Replace the Z vector within matrix
				//
				// Note that the matrix must be a minimum of 3x3

inline		void		setZVector(const matrix<N, 1, T> &m)
				{
					for (unsigned int i = 0; i < N; i++)
					{
						(*this)(i,2) = m(i,0);
					}
				}

				// Vector length calculation (squared)
				//
				// Specialized for Nx1 matrices (i.e. vectors)

inline	const	T		lengthSquared() const
				{
					TemplateAssert(M == 1);

					return dot(*this);
				}

				// Vector length calculation
				//
				// Specialized for Nx1 matrices (i.e. vectors)

inline	const	T		length() const
				{
					TemplateAssert(M == 1);

					return sqrt(lengthSquared());
				}

				// Vector length
				//
				// Specialized for Nx1 matrices (i.e. vectors)

inline		void		setLength(const T &len)
				{
					TemplateAssert(M == 1);

					T	l = len / length();

					x() *= l;
					y() *= l;
					z() *= l;
				}

				// Vector length
				//
				// Specialized for Nx1 matrices (i.e. vectors)

inline		T		distance(const matrix &m) const
				{
					TemplateAssert(M == 1);

					matrix	temp = *this - m;
					return temp.length();
				}

				// Normalize
				//
				// Specialized for Nx1 matrices (i.e. vectors)

inline		void		normalize()
				{
					setLength((T) 1);
				}

				// Normalize an orthogonal matrix (i.e. make sure the internal vectors are all perpendicular)
				//
				// Note that the matrix must be a 3x3, due to the fact that this routine uses a cross product

inline		void		orthoNormalize()
				{
					TemplateAssert(N == 3 && M == 3);

					matrix<N, 1, T>	xVector = extractXVector();
					matrix<N, 1, T>	yVector = extractYVector();
					matrix<N, 1, T>	zVector = extractZVector();

					xVector -= yVector * (xVector * yVector);
					xVector -= zVector * (xVector * zVector);
					xVector.normalize();

					yVector -= xVector * (yVector * xVector);
					yVector -= zVector * (yVector * zVector);
					yVector.normalize();

					zVector = xVector % yVector;

					setXVector(xVector);
					setYVector(yVector);
					setZVector(zVector);
				}

				// Absolute value of all components

inline		void		abs()
				{
					for (unsigned int i = 0; i < N*M; i++)
						_data[i] = (T) fabs((double) _data[i]);
				}

				// Specialized 'convenience accessors' for vectors, points, etc.
				//
				// Note that the matrix must have a value of N large enough to store the value
				// in question and M must always be 1.

inline	const	T 		&x() const {TemplateAssert(N > 0 && M == 1); return _data[0];}
inline		T		&x()       {TemplateAssert(N > 0 && M == 1); return _data[0];}
inline	const	T 		&y() const {TemplateAssert(N > 1 && M == 1); return _data[1];}
inline		T		&y()       {TemplateAssert(N > 1 && M == 1); return _data[1];}
inline	const	T 		&z() const {TemplateAssert(N > 2 && M == 1); return _data[2];}
inline		T		&z()       {TemplateAssert(N > 2 && M == 1); return _data[2];}
inline	const	T 		&w() const {TemplateAssert(N > 3 && M == 1); return _data[3];}
inline		T		&w()       {TemplateAssert(N > 3 && M == 1); return _data[3];}

				// Only use this if you need to... prefer operator(int,int) for access

inline	const	T		*data() const {return _data;}
inline		T		*data()       {return _data;}

				// Dimensions

inline		unsigned int	width() const {return N;}
inline		unsigned int	height() const {return M;}

				// Debugging functions

#ifdef _MSC_VER
inline		void		debugTrace() const
				{
					for (unsigned int i = 0; i < N; i++)
					{
						char	temp[90];
						strcpy(temp, "[");
						for (unsigned int j = 0; j < M; j++)
						{
							char	t[90];
							sprintf(t, "%12.5f", (*this)(i,j));
							strcat(temp, t);
						}
						strcat(temp, " ]\n");
						TRACE(temp);
					}
					TRACE("\n");
				}
#endif

#ifdef _H_LOGGER
inline		void		debugLog(const char *title) const
				{
					LOGBLOCK(title);
					for (unsigned int i = 0; i < N; i++)
					{
						char	temp[90];
						strcpy(temp, "[");
						for (unsigned int j = 0; j < M; j++)
						{
							char	t[90];
							sprintf(t, "%12.5f", (*this)(i,j));
							strcat(temp, t);
						}
						strcat(temp, " ]");
						LOG(temp);
					}
				}
#endif

inline		void		debugOut(iostream &s) const
				{
					for (unsigned int i = 0; i < N; i++)
					{
						s << "[";
						for (unsigned int j = 0; j < M; j++)
						{
							char	t[90];
							sprintf(t, "%12.5f", (*this)(i,j));
							s << t;
						}
						s << " ]" << endl;
					}
					s << endl;
				}

private:
		T		_data[N*M];
};

// --------------------------------------------------------------------------------------------------------------------------------
// Convenience types - Most common uses
// --------------------------------------------------------------------------------------------------------------------------------

typedef	matrix<3, 3> matrix3;
typedef	matrix<4, 4> matrix4;
typedef	matrix<3, 1> vector3;
typedef	matrix<4, 1> vector4;
typedef	matrix<2, 1> point2;
typedef	matrix<3, 1> point3;
typedef	matrix<4, 1> point4;

// --------------------------------------------------------------------------------------------------------------------------------
// Mixed-mode global overrides
// --------------------------------------------------------------------------------------------------------------------------------

template <unsigned int N, unsigned int M, class T>
inline	const	matrix<N, M, T>	operator *(const T &value, const matrix<N, M, T> &m) {return m * value;}

template <unsigned int N, unsigned int M, class T>
inline	const	matrix<N, M, T>	operator /(const T &value, const matrix<N, M, T> &m) {return m.inverseDivide(value);}

template <unsigned int N, unsigned int M, class T>
inline	const	matrix<N, M, T>	operator +(const T &value, const matrix<N, M, T> &m) {return m + value;}

template <unsigned int N, unsigned int M, class T>
inline	const	matrix<N, M, T>	operator -(const T &value, const matrix<N, M, T> &m) {return m.inverseSubtract(value);}

#endif // _H_VMATH
// ---------------------------------------------------------------------------------------------------------------------------------
// VMath.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

