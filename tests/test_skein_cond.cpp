/**
 * @file test_skein_cond.cpp
 * @brief trivial test case for a prototype onto boost fiber condition variables
 *
 */

#include <iostream>

#include <evtlet/skein/skein.hpp>

class echo_int : public evtlet::skein<int> {
public:
  echo_int(int value) : skein(), m_value(value) {};

protected:
  const int m_value;
  virtual int func() { return m_value; };
};

auto main() -> int {

  auto sk = new echo_int(5);
  auto *vv = sk->get();

  if (*vv == 5) {
    std::cout << "OK: ";
    std::cout << *vv;
    std::cout << std::endl;
    assert(vv == sk->get());
    return 0;
  } else {
    std::cerr << "Unexpected Value: ";
    std::cerr << *vv;
    std::cerr << std::endl;
    return 1;
  }

  return 0;
};
