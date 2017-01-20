set(SRCS
            ctr.c
            polarssl/aes.c
            polarssl/bignum.c
            polarssl/rsa.c
            polarssl/sha2.c 
            )
set(HEADERS
            ctr.h
            keyset.h
            polarssl/aes.h
            polarssl/bignum.h
            polarssl/bn_mul.h
            polarssl/config.h
            polarssl/padlock.h
            polarssl/rsa.h
            polarssl/sha2.h
            types.h 
            )

create_directory_groups(${SRCS} ${HEADERS})
add_library(ctrtool ${SRCS} ${HEADERS})
target_compile_definitions(ctrtool PUBLIC STATIC_GETOPT)
target_include_directories(ctrtool INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
