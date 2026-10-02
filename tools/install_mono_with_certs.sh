#!/usr/bin/env bash
set -e

apt-get update
apt-get install -y mono-complete wget ca-certificates

cd /tmp
wget -q "https://download.mono-project.com/repo/ubuntu/pool/main/m/mono/ca-certificates-mono_6.13.0.1204-0nightly6+ubuntu2004b1_all.deb" \
  -O ca-certificates-mono.deb

EXPECTED_SHA256="e7674fd2442c9c8c1b303659ad50eefbd1b27878fcfe8a87242a2d526e3b4092"
ACTUAL_SHA256=$(sha256sum ca-certificates-mono.deb | awk '{print $1}')
if [[ "$ACTUAL_SHA256" != "$EXPECTED_SHA256" ]]; then
    echo "ERROR: SHA256 mismatch for ca-certificates-mono.deb" >&2
    echo "Expected: $EXPECTED_SHA256" >&2
    echo "Actual:   $ACTUAL_SHA256" >&2
    exit 1
fi

rm -rf ca-certificates-mono-extracted
dpkg-deb -x ca-certificates-mono.deb ca-certificates-mono-extracted

# Install cert-sync script and managed executable
install -m 0755 ca-certificates-mono-extracted/usr/bin/cert-sync /usr/local/bin/cert-sync
mkdir -p /usr/lib/mono/4.5
install -m 0644 ca-certificates-mono-extracted/usr/lib/mono/4.5/cert-sync.exe /usr/lib/mono/4.5/cert-sync.exe
install -m 0644 ca-certificates-mono-extracted/usr/lib/mono/4.5/cert-sync.exe.config /usr/lib/mono/4.5/cert-sync.exe.config 2>/dev/null || true

# Populate the machine trust store used by Mono.Btls
cert-sync /etc/ssl/certs/ca-certificates.crt

rm -rf ca-certificates-mono.deb ca-certificates-mono-extracted
