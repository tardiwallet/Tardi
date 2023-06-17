set (bc_bip39_srcs
    seedtool-cli/deps/bc-bip39/src/bip39.c
    seedtool-cli/deps/bc-bip39/src/prefix2.c
    seedtool-cli/deps/bc-bip39/src/prefix1.c
    seedtool-cli/deps/bc-bip39/src/suffix_array.c
)
# set(bc_bytewords_srcs
#     bc-bytewords/src/bc-bytewords.c
# )
set (bc_crypto_base_srcs
    seedtool-cli/deps/bc-crypto-base/src/crc32.c
    seedtool-cli/deps/bc-crypto-base/src/hmac.c
    seedtool-cli/deps/bc-crypto-base/src/memzero.c
    seedtool-cli/deps/bc-crypto-base/src/pbkdf2.c
    seedtool-cli/deps/bc-crypto-base/src/sha2.c
    seedtool-cli/deps/bc-crypto-base/test/test.c
    seedtool-cli/deps/bc-crypto-base/test/test-utils.c
)
set(bc_shamir_srcs
    seedtool-cli/deps/bc-shamir/src/hazmat.c
    seedtool-cli/deps/bc-shamir/src/interpolate.c
    seedtool-cli/deps/bc-shamir/src/shamir.c
)
set(bc_sskr_srcs
    seedtool-cli/deps/bc-sskr/src/encoding.c
    seedtool-cli/deps/bc-sskr/test/test.c
    seedtool-cli/deps/bc-sskr/test/test-utils.c
)
set(bc_ur_srcs
    seedtool-cli/deps/bc-ur/src/bytewords.cpp
    seedtool-cli/deps/bc-ur/src/crc32.c
    seedtool-cli/deps/bc-ur/src/fountain-decoder.cpp
    seedtool-cli/deps/bc-ur/src/fountain-encoder.cpp
    seedtool-cli/deps/bc-ur/src/fountain-utils.cpp
    seedtool-cli/deps/bc-ur/src/memzero.c
    seedtool-cli/deps/bc-ur/src/random-sampler.cpp
    seedtool-cli/deps/bc-ur/src/sha2.c
    seedtool-cli/deps/bc-ur/src/ur-decoder.cpp
    seedtool-cli/deps/bc-ur/src/ur-encoder.cpp
    seedtool-cli/deps/bc-ur/src/ur.cpp
    seedtool-cli/deps/bc-ur/src/utils.cpp
    seedtool-cli/deps/bc-ur/src/xoshiro256.cpp
)
set(seedtool_srcs
    seedtool-cli/src/format-base10.cpp
    seedtool-cli/src/format-base6.cpp
    seedtool-cli/src/format-bip39.cpp
    seedtool-cli/src/format-bits.cpp
    seedtool-cli/src/format-bytewords-minimal.cpp
    seedtool-cli/src/format-bytewords-uri.cpp
    seedtool-cli/src/format-bytewords.cpp
    seedtool-cli/src/format-cards.cpp
    seedtool-cli/src/format-dice.cpp
    seedtool-cli/src/format-hex.cpp
    seedtool-cli/src/format-ints.cpp
    seedtool-cli/src/format-random.cpp
    seedtool-cli/src/format-sskr.cpp
    seedtool-cli/src/format.cpp
    seedtool-cli/src/hkdf.c
    seedtool-cli/src/params.cpp
    seedtool-cli/src/random.cpp
    seedtool-cli/src/randombytes.c
    seedtool-cli/src/seedtool.cpp
    seedtool-cli/src/utils.cpp
    seedtool-cli/src/argp.c
    # seedtool-cli/src/argp/argp-parse.c
    # seedtool-cli/src/argp.c
    # seedtool-cli/src/getopt1.c
    # seedtool-cli/src/getopt.c
)

set(srcs    
    ${bc_crypto_base_srcs}
    ${bc_ur_srcs}
    ${seedtool_srcs}
    ${bc_sskr_srcs}
    ${bc_bip39_srcs}
    ${bc_shamir_srcs}
)
# ${bc_bytewords_srcs}

set(include_dirs    
    seedtool-cli/deps/bc-crypto-base/src 
    seedtool-cli/deps/bc-crypto-base/test
    seedtool-cli/deps/bc-ur/src
    seedtool-cli/deps/bc-bip39/src
    seedtool-cli/deps/bc-sskr/src
    seedtool-cli/deps/bc-shamir/src
    # seedtool-cli/deps/argp-standalone/argp-standalone
    seedtool-cli/src
)
# bc-bytewords/src

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(COMPONENT_ADD_INCLUDEDIRS ${include_dirs})
# set(COMPONENT_SRCS ${srcs})


idf_component_register(SRCS ${srcs} INCLUDE_DIRS ${include_dirs})
# REQUIRES arduino
# target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)
target_compile_features(${COMPONENT_LIB} PUBLIC cxx_std_17)
set_target_properties(${COMPONENT_LIB}
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

# idf_build_set_property(COMPILE_OPTIONS "-frtti" APPEND)
# set_source_files_properties(seedtool-cli/src/formats-all.hpp
#     PROPERTIES COMPILE_FLAGS
#     -frtti
# )

# set_property(
#   SOURCE seedtool-cli/src/formats-all.hpp
#   APPEND
#   PROPERTY COMPILE_FLAGS
#   -frtti
# )

# set_property(
#   SOURCE formats-all.hpp
#   APPEND
#   PROPERTY COMPILE_FLAGS
#   -frtti
# )

add_definitions(
    -DUSE_STDONLY=1 -DUSE_STD_STRING=1 -DUSE_ARDUINO_STRING=0 -DUSE_ARDUINO_STREAM=0 -DUSE_MBED_STREAM=0 -DARDUINO 
    ) 
    # -DARDUINO_SKIP_IDF_VERSION_CHECK 

# idf_build_get_property(opt_cxx CXX_COMPILE_OPTIONS )
# idf_build_get_property(opt_all COMPILE_OPTIONS )

# message(STATUS "CXX_COMPILE_OPTIONS is: ${opt_cxx}")
# message(STATUS "COMPILE_OPTIONS is: ${opt_all}")

# idf_component_get_property(dir crypto COMPONENT_DIR)
# idf_component_get_property(nam crypto COMPONENT_LIB)
# message(STATUS "The component directory is: ${dir}")
# message(STATUS "The component name is: ${nam}")
