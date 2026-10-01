#!/usr/bin/env bash
set -e

apt-get update
apt-get install -y ca-certificates gnupg

gpg --homedir /tmp --no-default-keyring \
  --keyring gnupg-ring:/usr/share/keyrings/mono-official-archive-keyring.gpg \
  --keyserver hkp://keyserver.ubuntu.com:80 \
  --recv-keys 3FA7E0328081BFF6A14DA29AA6A19B38D3D831EF

chmod +r /usr/share/keyrings/mono-official-archive-keyring.gpg

echo "deb [signed-by=/usr/share/keyrings/mono-official-archive-keyring.gpg] https://download.mono-project.com/repo/ubuntu stable-focal main" \
  | tee /etc/apt/sources.list.d/mono-official-stable.list

apt-get update

apt-get install -y mono-complete ca-certificates-mono

cert-sync /etc/ssl/certs/ca-certificates.crt
