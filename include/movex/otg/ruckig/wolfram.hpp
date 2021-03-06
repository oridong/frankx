#pragma once

#include <complex>


namespace movex {

inline double Power(double v, int e) {
    return std::pow(v, e);
}

inline double Power(double v, double e) {
    return std::pow(v, e);
}

inline double Sqrt(double v) {
    return std::sqrt(v);
}

inline double Abs(double v) {
    return std::abs(v);
}

inline std::complex<double> Complex(double a, double b) {
    return std::complex<double>(a, b);
}

inline std::complex<double> SqrtComplex(double v) {
    return std::sqrt(std::complex<double>(v, 0));
}

inline std::complex<double> SqrtComplex(std::complex<double> v) {
    return std::sqrt(v);
}

inline std::complex<double> PowerComplex(std::complex<double> v, int e) {
    return std::pow(v, e);
}

inline std::complex<double> PowerComplex(std::complex<double> v, double e) {
    return std::pow(v, e);
}

inline long double PowerLong(long double v, int e) {
    return std::pow(v, e);
}

inline long double PowerLong(long double v, double e) {
    return std::pow(v, e);
}

inline long double SqrtLong(long double v) {
    return std::sqrt(v);
}

inline long double AbsLong(long double v) {
    return std::abs(v);
}

inline std::complex<long double> ComplexLong(long double a, double b) {
    return std::complex<long double>(a, b);
}

inline std::complex<long double> SqrtComplexLong(long double v) {
    return std::sqrt(std::complex<long double>(v, 0));
}

inline std::complex<long double> SqrtComplexLong(std::complex<long double> v) {
    return std::sqrt(v);
}

inline std::complex<long double> PowerComplexLong(std::complex<long double> v, double e) {
    return std::pow(v, e);
}

inline double eval(std::array<double, 6> polynom, double t) {
    return polynom[5] + t * (polynom[4] + t * (polynom[3] + t * (polynom[2] + t * (polynom[1] + t * polynom[0]))));
}

inline double eval(std::array<double, 7> polynom, double t) {
    return polynom[6] + t * (polynom[5] + t * (polynom[4] + t * (polynom[3] + t * (polynom[2] + t * (polynom[1] + t * polynom[0])))));
}

} // namespace movex
