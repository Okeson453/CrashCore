# Production dependencies for CrashCore transport / persistence / JSON.
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

# Boost (system + headers for Beast/Asio)
find_package(Boost 1.74 REQUIRED COMPONENTS system)

# libpq
find_package(PostgreSQL REQUIRED)

# libcurl
find_package(CURL REQUIRED)

# nlohmann_json (header-only)
find_package(nlohmann_json 3.11 QUIET)
if(NOT nlohmann_json_FOUND)
  # Debian package installs headers without CMake config sometimes
  find_path(NLOHMANN_JSON_INCLUDE_DIR nlohmann/json.hpp)
  if(NLOHMANN_JSON_INCLUDE_DIR)
    add_library(nlohmann_json INTERFACE IMPORTED)
    target_include_directories(nlohmann_json INTERFACE ${NLOHMANN_JSON_INCLUDE_DIR})
    add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)
  endif()
endif()

message(STATUS "OpenSSL: ${OPENSSL_VERSION}")
message(STATUS "Boost: ${Boost_VERSION}")
message(STATUS "PostgreSQL: ${PostgreSQL_LIBRARIES}")
message(STATUS "CURL: ${CURL_LIBRARIES}")
