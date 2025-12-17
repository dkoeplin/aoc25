if (NOT _Z3_FOUND) # If there's none, fetch and build Z3
    set(_Z3_FOUND TRUE)
    include(FetchContent)
    FetchContent_Declare(
            z3
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/Z3Prover/z3/archive/refs/tags/z3-4.15.4.zip
    )
    FetchContent_GetProperties(z3)
    if (NOT Z3_POPULATED) # Have we downloaded Z3 yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_MakeAvailable(z3)
    endif ()
endif ()