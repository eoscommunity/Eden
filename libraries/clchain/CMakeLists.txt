find_package(OpenSSL REQUIRED Crypto)

function(add suffix)
    add_library(clchain${suffix})
    target_link_libraries(clchain${suffix} PUBLIC abieos${suffix} boost OpenSSL::Crypto)
    target_include_directories(clchain${suffix} PUBLIC include)
    target_sources(clchain${suffix} PRIVATE
        src/crypto.cpp
    )
    if(DEFINED IS_WASM)
        target_link_libraries(clchain${suffix} PUBLIC wasm-base${suffix})
        target_sources(clchain${suffix} PRIVATE
            wasi-polyfill/__wasi_environ_get.cpp
            wasi-polyfill/__wasi_environ_sizes_get.cpp
            wasi-polyfill/__wasi_fd_read.cpp
            wasi-polyfill/__wasi_proc_exit.cpp
            wasi-polyfill/eosio_assert.cpp
            wasi-polyfill/print.cpp
        )
    endif()
endfunction()

add("")
if(DEFINED IS_WASM)
    add("-debug")
endif()

if(DEFINED IS_NATIVE)
  add_subdirectory(test)
endif()
