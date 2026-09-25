#!/usr/bin/env bash
set -eu

# Headers only, for manylinux wheel builds. Verify the official release archive.
root=/opt/arp-boost
mkdir -p "$root"
if [ -f "$root/boost_1_85_0/boost/multiprecision/cpp_int.hpp" ]; then
    exit 0
fi
archive="$root/boost_1_85_0.tar.gz"
curl --fail --location --retry 3 \
    https://archives.boost.io/release/1.85.0/source/boost_1_85_0.tar.gz \
    --output "$archive"
printf '%s  %s\n' \
    be0d91732d5b0cc6fbb275c7939974457e79b54d6f07ce2e3dfdd68bef883b0b \
    "$archive" | sha256sum --check
tar -xzf "$archive" -C "$root"
