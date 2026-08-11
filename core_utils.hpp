#pragma once
#include <stdexcept>
#include <string>
#include <concepts>

// ==========================================
// Q2(A): Hierarquia de Exceções
// ==========================================
class study_error : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class invalid_card_error : public study_error {
public:
    explicit invalid_card_error(const std::string& msg)
        : study_error("Erro de validacao do Card: " + msg) {}
};

// ==========================================
// Q1(C): Concept C++20
// ==========================================
template <typename T>
concept Calculavel = requires(const T& t) {
    { t.calculate_interval() } -> std::convertible_to<float>;
};

// ==========================================
// Q1(B): CRTP - Comportamento estático sem vtable
// ==========================================
template <typename Derived>
class Counted {
    static inline int count_ = 0;
public:
    Counted() { ++count_; }
    Counted(const Counted&) { ++count_; }
    ~Counted() { --count_; }
    static int alive() { return count_; }
};