#ifndef Funcs
#define Funcs

#define _kResetTimer(c, p, i, f) \
  c = TIME_MEASURE();            \
  if (c - p >= i)                \
  {                              \
    p = c;                       \
    f                            \
  }



String getValue(String data, char separator, int index)
{
    unsigned long maxIndex = data.length() - 1;
    int found = 0;
    String chunkVal = "";

    for (unsigned long i = 0; i <= maxIndex && found <= index; i++)
    {
        chunkVal += data[i];
        if (data[i] == separator)
        {
            found++;
            if (found > index)
                return chunkVal.substring(0, chunkVal.length() - 1);
            chunkVal = "";
        }
        else if ((i == maxIndex) && (found < index))
            return chunkVal;
    }
    return chunkVal;
}

unsigned getCountOfCharIn(String inp, char d)
{
    unsigned count = 0;
    for (unsigned i = 0; i < inp.length(); i++)
        if (inp.charAt(i) == d)
            count++;
    return count;
}

String numToBase(unsigned long n, uint8_t base)
{
  char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
  char *str = &buf[sizeof(buf) - 1];

  *str = '\0';

  // prevent crash if called with base == 1
  if (base < 2) base = 10;

  do {
    char c = n % base;
    n /= base;

    *--str = c < 10 ? c + '0' : c + 'A' - 10;
  } while(n);

  return String(str);
}



#endif