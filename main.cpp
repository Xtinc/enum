#include "enum.h"

#include <sstream>
#include <cassert>

template <typename E>
void test_to_from_string(E const e, std::string const s)
{
  assert(enum_to_string(e) == s);
  assert(enum_from_string<E>(s) == e);
}

template <typename E>
void test_stream_io(E const e)
{
  std::stringstream ss;
  ss << e;
  E v;
  ss >> v;
  assert(v == e);
}

///////////////////////////////

namespace N1
{
  enum WeakEnum
  {
    A,
    B,
    END
  };
}
ENUM_STRINGS(N1::WeakEnum, "wa", "wb");

namespace N2
{
  enum class StrongEnum : int16_t
  {
    A,
    B
  };
}

ENUM_STRINGS(N2::StrongEnum, "sa", "sb");

namespace N3
{
  struct Foo
  {
    enum class NestedEnum
    {
      A,
      B,
      END
    };
  };
}
ENUM_STRINGS(N3::Foo::NestedEnum, "fa", "fb");

///////////////////////////////

int main()
{
  test_to_from_string(N1::A, "wa");
  test_to_from_string(N1::B, "wb");
  test_stream_io(N1::A);
  test_stream_io(N1::B);

  test_to_from_string(N2::StrongEnum::A, "sa");
  test_to_from_string(N2::StrongEnum::B, "sb");
  test_stream_io(N2::StrongEnum::A);
  test_stream_io(N2::StrongEnum::B);

  test_to_from_string(N3::Foo::NestedEnum::A, "fa");
  test_to_from_string(N3::Foo::NestedEnum::B, "fb");
  test_stream_io(N3::Foo::NestedEnum::A);
  test_stream_io(N3::Foo::NestedEnum::B);

  auto array1 = to_array<uint32_t>({1, 2, 3, 4});

  return 0;
}