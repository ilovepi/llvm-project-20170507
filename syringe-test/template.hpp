#ifndef TEMPLATE_HPP_
#define TEMPLATE_HPP_ 1

template <typename T>[[clang::syringe_injection_site]] int foo(T a, T b) {
  return a + b;
}

template <typename T>
//[[clang::syringe_payload("_Z3fooIiEiT_S0_")]] int bad_foo(T a, T b) {
[[clang::syringe_payload("foo")]] int bad_foo(T a, T b) {
  return a - b;
}

template <typename T> class Adder {

public:
  T data;
  Adder(T initialValue) : data(initialValue) {}

  [[clang::syringe_injection_site]] T add(T a) { return data + a; }
};

template <typename T> class BadAdder : public Adder<T> {
public:
  BadAdder(T initialValue) : Adder<T>(initialValue) {}
  [[clang::syringe_payload("_ZN5AdderIiE3addEi")]] T add(T a) {
    return this->data - a;
  }
};

#endif /* ifndef TEMPLATE_HPP_ */
