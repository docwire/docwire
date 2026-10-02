#!/usr/bin/env bash
set -e

apt-get update
apt-get install -y mono-complete wget ca-certificates

cd /tmp
wget -q "https://download.mono-project.com/repo/ubuntu/pool/main/m/mono/ca-certificates-mono_6.13.0.1204-0nightly6+ubuntu2004b1_all.deb" \
  -O ca-certificates-mono.deb

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
