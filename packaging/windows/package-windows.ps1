# Packages the Windows build into a ZIP for GitHub Releases:
#   <out>/iroha-<version>-windows-x64/
#     bin/irohad.exe, iroha-cli.exe
#     iroha-lib/include/...   headers needed to consume iroha-lib
#     iroha-lib/lib/...       compiled static libraries (*.lib)
#     README.txt
# Then compresses it to <out>/iroha-<version>-windows-x64.zip
#
# Usage (from repo root, after a Release build in .\build):
#   pwsh packaging/windows/package-windows.ps1 [-BuildDir build] [-OutDir dist]
param(
    [string]$BuildDir = "build",
    [string]$OutDir   = "dist"
)
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

$version = (git describe --tags --always --dirty 2>$null)
if (-not $version) { $version = "dev" }
$version = $version -replace '[^A-Za-z0-9._-]', '-'

$stage = Join-Path $OutDir "iroha-$version-windows-x64"
if (Test-Path $stage) { Remove-Item -Recurse -Force $stage }
New-Item -ItemType Directory -Force -Path `
    "$stage/bin", "$stage/iroha-lib/include", "$stage/iroha-lib/lib" | Out-Null

# --- binaries ---
$exes = @()
foreach ($name in 'irohad.exe', 'iroha-cli.exe') {
    $hit = Get-ChildItem -Path $BuildDir -Recurse -Filter $name -File |
           Select-Object -First 1
    if (-not $hit) { throw "binary not found: $name (looked in $BuildDir)" }
    Copy-Item $hit.FullName "$stage/bin/$name"
    $exes += $name
}

# --- iroha-lib public headers (source tree) ---
$hdrRoot = 'iroha-lib'
Get-ChildItem -Path $hdrRoot -Recurse -File -Include *.hpp, *.h |
    ForEach-Object {
        $rel = $_.FullName.Substring((Resolve-Path $hdrRoot).Path.Length + 1)
        $dst = Join-Path "$stage/iroha-lib/include" $rel
        New-Item -ItemType Directory -Force -Path (Split-Path $dst) | Out-Null
        Copy-Item $_.FullName $dst
    }

# --- generated protobuf/gRPC headers (build tree) ---
Get-ChildItem -Path $BuildDir -Recurse -File -Include *.pb.h, *.grpc.pb.h |
    ForEach-Object {
        $rel = $_.FullName.Substring((Resolve-Path $BuildDir).Path.Length + 1)
        $dst = Join-Path "$stage/iroha-lib/include/gen" $rel
        New-Item -ItemType Directory -Force -Path (Split-Path $dst) | Out-Null
        Copy-Item $_.FullName $dst
    }

# --- compiled static libraries ---
$libs = Get-ChildItem -Path $BuildDir -Recurse -Filter *.lib -File
if (-not $libs) { throw "no *.lib found under $BuildDir" }
foreach ($lib in $libs) { Copy-Item $lib.FullName "$stage/iroha-lib/lib/$($lib.Name)" }

# --- README ---
@"
Iroha 1 for Windows ($version)
================================

bin/
  irohad.exe     - Iroha node
  iroha-cli.exe  - command-line client (interactive and JSON modes)

iroha-lib/
  include/       - public headers of the C++ client library
                   (iroha-lib sources + generated protobuf/gRPC headers
                   under include/gen/)
  lib/           - compiled static libraries (*.lib) for linking
                   against iroha-lib

Notes
-----
* Static libraries were built with MSVC (Visual Studio 2022) for
  x64-windows; consumers must link with a compatible MSVC toolchain.
* iroha-lib additionally requires the third-party dependencies
  (protobuf, gRPC, Boost, spdlog, rapidjson, gflags, iroha-ed25519)
  — install them with vcpkg or provide your own builds.
* TLS support: pass --tls_root_cert to iroha-cli / use TlsOptions in
  iroha-lib for one-way TLS to torii.
"@ | Set-Content -Encoding UTF8 "$stage/README.txt"

New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
$zip = Join-Path $OutDir "iroha-$version-windows-x64.zip"
if (Test-Path $zip) { Remove-Item -Force $zip }
Compress-Archive -Path $stage -DestinationPath $zip -CompressionLevel Optimal
Write-Host "==> $zip"
Get-Item $zip | Select-Object Name, @{n='SizeMB';e={[math]::Round($_.Length/1MB,1)}}
