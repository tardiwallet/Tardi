# Seedtool CLI fix
Seedtool CLI is designed to be build for platforms other than ESP32. However, it is at the moment the basis of the Tardi Wallet.

In order the make that happen, some updates have to be done in the original files. Most of the fixes are done by redirecting the SRC files to the updated versions under this directory. However, it is not easy to redirect the header files. 

At the moment, the 3 header files have to be manually fixed:

1. params.hpp
Lines 15 and 16 have to be updated as follows:

```C
#include "bc-sskr.h"
#include "bc-ur.hpp"
```

2. format-sskr.hpp
Line 10 has to be updated to:

```C
#include "bc-sskr.h"
```
3. cbor-utils.hpp
Line 15 has to be updated to:

```C
#include "bc-ur.hpp"
```