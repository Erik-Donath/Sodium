#include "pre_kernel.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

// CPU Setup
#include "gdt.h"
#include "tss.h"

static void test_printf(void) {
  printf("\n========== BEGIN printf() TESTS ==========\n\n");

  // --- basics ---
  printf("1. %%d basic: %d\n", 42);
  printf("2. %%d negative: %d\n", -42);
  printf("3. %%i signed alt.: %i\n", -123);
  printf("4. %%u unsigned: %u\n", 3000000000u);
  printf("5. %%o octal: %o\n", 0755);
  printf("6. %%x hex lowercase: %x\n", 0x2A);
  printf("7. %%X hex uppercase: %X\n", 0x2A);
  printf("8. %%p pointer: %p\n", (void*)0xCAFEBABE);

  // --- width and alignment ---
  printf("9. %%8d width right: '%8d'\n", 123);
  printf("10. %%-8d width left: '%-8d'\n", 123);
  printf("11. %%08d zero pad: '%08d'\n", 123);
  printf("12. %%+8d sign right: '%+8d'\n", 123);
  printf("13. %%+8d sign neg: '%+8d'\n", -123);
  printf("14. %% 8d space flag: '% 8d'\n", 123);
  printf("15. %%#x alternate hex: '%#x'\n", 0x2A);
  printf("16. %%#X alternate hex: '%#X'\n", 0x2A);
  printf("17. %%#o alternate octal: '%#o'\n", 0755);

  // --- precision ---
  printf("18. %%.4d precision 4: '%.4d'\n", 42);
  printf("19. %%.0d precision 0, value 0: '%.0d'\n", 0);
  printf("20. %%8.4d width 8, precision 4: '%8.4d'\n", 42);
  printf("21. %%-8.4d left-align width 8, prec 4: '%-8.4d'\n", 42);
  printf("22. %%#8.4x width+alt+prec: '%#8.4x'\n", 0x2A);

  // --- different number sizes ---
  printf("23. %%hd short: '%hd'\n", (short)-12345);
  printf("24. %%hhd char: '%hhd'\n", (signed char)-42);
  printf("25. %%ld long: '%ld'\n", (long)-1234567);
  printf("26. %%lld long long: '%lld'\n", (long long)-12345678901234LL);
  printf("27. %%zu size_t (unsigned): '%zu'\n", (size_t)1234567);
  printf("28. %%jd intmax_t: '%jd'\n", (intmax_t)-9876543210LL);
  printf("29. %%tu ptrdiff_t: '%td'\n", (ptrdiff_t)-2048);

  // --- alternate bases ---
  printf("30. %%#o octal alt flag small: '%#o'\n", 8);
  printf("31. %%#x hex lower alt: '%#x'\n", 0xBEEF);
  printf("32. %%#X hex upper alt: '%#X'\n", 0xBEEF);
  printf("33. %%#8x padded hex alt: '%#8x'\n", 0xBEEF);
  printf("34. %%#08x padded alt zero: '%#08x'\n", 0xBEEF);

  // --- sign and padding edge cases ---
  printf("35. %%+08d pad sign pos: '%+08d'\n", 123);
  printf("36. %%+08d pad sign neg: '%+08d'\n", -123);
  printf("37. %% 08d space pad pos: '% 08d'\n", 123);
  printf("38. %%08u zero pad unsigned: '%08u'\n", 45u);
  printf("39. %%08i zero pad int: '%08i'\n", -45);

  // --- large values ---
  printf("40. 64-bit unsigned: '%llu'\n", (unsigned long long)0x123456789ABCDEF0ULL);
  printf("41. 64-bit signed pos: '%lld'\n", (long long)123456789012345LL);
  printf("42. 64-bit signed neg: '%lld'\n", (long long)-123456789012345LL);

  // --- complex combos ---
  printf("43. %%#10x alt+width hex: '%#10x'\n", 0x2A);
  printf("44. %%-#10x left+alt+width: '%-#10x'\n", 0x2A);
  printf("45. %%+10.5d width+prec+sign: '%+10.5d'\n", 42);
  printf("46. %% 10.5d width+prec+space: '% 10.5d'\n", 42);
  printf("47. %%#10.5o width+prec+alt oct: '%#10.5o'\n", 0755);
  printf("48. %%#010.5x zero pad ignored (alt+prec): '%#010.5x'\n", 0xAB);
  printf("49. %%#6x alt small width: '%#6x'\n", 0xA);
  printf("50. %%#x zero value alt: '%#x'\n", 0);
  printf("51. %%.5i positive precision: '%.5i'\n", 1);
  printf("52. %%.5i negative precision: '%.5i'\n", -1);
  printf("53. %%8.3u unsigned width+prec: '%8.3u'\n", 123);
  printf("54. %%#8o alt octal width: '%#8o'\n", 64u);
  printf("55. %%8.3x hex width+prec: '%8.3x'\n", 255);
  printf("56. %%#8.3x alt hex width+prec: '%#8.3x'\n", 255);

  // --- zero and sign edge handling ---
  printf("57. %%.0u zero suppress unsigned zero: '%.0u'\n", 0u);
  printf("58. %%.0x zero suppress hex zero: '%.0x'\n", 0u);
  printf("59. %%.0o zero suppress oct zero: '%.0o'\n", 0u);

  // --- random combined ---
  printf("60. %%#-08X crazy flag combo: '%#-08X'\n", 0xBE);

  printf("\n========== END printf() TESTS ==========\n");
}

void __attribute__((cdecl)) pre_kernel(void *mb_info) {
  (void)mb_info;

  // Seting up the CPU
  i686_gdt_init();
  i686_tss_init();
  i686_gdt_load();
  i686_tss_load();

  // Printing Sodium in aqua to VGA Output
  static char *i686_vga = (char *)0xB8000;
  const char *hello = "Sodium            ";
  for (uint32_t i = 0; hello[i]; i++) {
    i686_vga[(i << 1) + 0] = hello[i]; // Char
    i686_vga[(i << 1) + 1] = 0x03;     // Color
  }

  // Printing Sodium in aqua using ASCII Escape Seqenz to qemu debug output
  printf("\033[38;5;6;48;5;0mSODIUM\033[0m\n");

  test_printf();

  return;
}
