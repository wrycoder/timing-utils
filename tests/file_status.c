#include "../src/time-remaining.h"
#include <criterion/criterion.h>
#include <inttypes.h>
#include <stdbool.h>

TestSuite(main, .disabled = false);

Test(main, file_found)
{
  cr_expect(false == true);
}
