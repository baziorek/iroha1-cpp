# Packages the Windows build into a ZIP for GitHub Releases:
#   <out>/iroha-<version>-windows-x64/
#     bin/irohad.exe, iroha-cli.exe (+ iroha_migrate.exe, iroha_wsv_diff.exe)
#     example/...             single-node RocksDB config, genesis, keys
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
    "$stage/bin", "$stage/example", "$stage/iroha-lib/include", `
    "$stage/iroha-lib/lib" | Out-Null

# --- binaries ---
$exes = @()
foreach ($name in 'irohad.exe', 'iroha-cli.exe') {
    $hit = Get-ChildItem -Path $BuildDir -Recurse -Filter $name -File |
           Select-Object -First 1
    if (-not $hit) { throw "binary not found: $name (looked in $BuildDir)" }
    Copy-Item $hit.FullName "$stage/bin/$name"
    $exes += $name
}
foreach ($name in 'iroha_migrate.exe', 'iroha_wsv_diff.exe') {
    $hit = Get-ChildItem -Path $BuildDir -Recurse -Filter $name -File |
           Select-Object -First 1
    if ($hit) { Copy-Item $hit.FullName "$stage/bin/$name" }
}

# --- single-node example (RocksDB, no PostgreSQL needed) ---
Copy-Item example/genesis.block, example/node0.pub, example/node0.priv, `
    example/admin@test.pub, example/admin@test.priv "$stage/example/"
@'
{
  "block_store_path" : "blocks",
  "torii_port" : 50051,
  "internal_port" : 10001,
  "database": { "type": "rocksdb", "path": "rocksdb" },
  "max_proposal_size" : 10,
  "vote_delay" : 5000,
  "mst_enable" : false,
  "mst_expiration_time" : 1440,
  "max_rounds_delay": 3000,
  "proposal_creation_timeout": 3000,
  "stale_stream_max_rounds": 2
}
'@ | Set-Content -Encoding ascii "$stage/example/config.json"

# --- iroha-lib headers + the in-tree headers they include (source tree) ---
# Each tree keeps its own include root: on a case-insensitive filesystem
# irohad/model/query.hpp and iroha-lib/model/Query.hpp would collide.
foreach ($hdrRoot in 'iroha-lib', 'shared_model', 'libs', 'irohad') {
    $rootPath = (Resolve-Path $hdrRoot).Path
    Get-ChildItem -Path $hdrRoot -Recurse -File -Include *.hpp, *.h |
        ForEach-Object {
            $rel = $_.FullName.Substring($rootPath.Length + 1)
            $dst = Join-Path "$stage/iroha-lib/include/$hdrRoot" $rel
            New-Item -ItemType Directory -Force -Path (Split-Path $dst) | Out-Null
            Copy-Item $_.FullName $dst
        }
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
# Skip import libraries of executables (bin/irohad.lib etc.) and CMake
# internals; keep only the project's static libraries.
$libs = Get-ChildItem -Path $BuildDir -Recurse -Filter *.lib -File |
        Where-Object {
            $_.FullName -notmatch '[\\/]CMakeFiles[\\/]' -and
            -not (Test-Path (Join-Path $_.DirectoryName "$($_.BaseName).exe"))
        }
if (-not $libs) { throw "no *.lib found under $BuildDir" }
foreach ($lib in $libs) { Copy-Item $lib.FullName "$stage/iroha-lib/lib/$($lib.Name)" }

# --- merged single archive: produced by the iroha_lib_all CMake target ---
# (lib.exe /OUT on MSVC, ar+ranlib elsewhere — see cmake/merge-static-libs.cmake)
$mergedSrc = Join-Path $BuildDir "lib/iroha_lib_all.lib"
if (Test-Path $mergedSrc) {
    Copy-Item $mergedSrc "$stage/iroha-lib/lib/iroha_lib_all.lib"
} else {
    Write-Warning "iroha_lib_all.lib not found in $BuildDir; granular .lib files are still shipped"
}

# --- README ---
@"
Iroha 1 for Windows ($version)
================================

bin/
  irohad.exe          - Iroha node
  iroha-cli.exe       - command-line client (interactive and JSON modes)
  iroha_migrate.exe   - WSV migration tool (PostgreSQL <-> RocksDB)
  iroha_wsv_diff.exe  - WSV comparison tool

example/         - single-node network on RocksDB (no PostgreSQL needed):
                     cd example
                     ..\bin\irohad.exe --config config.json ``
                       --genesis_block genesis.block --keypair_name node0
                   then in another console:
                     ..\bin\iroha-cli.exe --account_name admin@test

iroha-lib/
  include/       - headers of the C++ client library; add ALL of these
                   include directories, in this order:
                     include/iroha-lib
                     include/shared_model
                     include/libs
                     include/irohad
                     include/gen/schema
  lib/           - compiled static libraries (*.lib) for linking
                   against iroha-lib; prefer the single merged archive
                   iroha_lib_all.lib when present

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
# Not Compress-Archive: on Windows PowerShell 5.1 it stores entry names with
# '\' separators, which unzip on Linux/macOS treats as part of the file name.
Add-Type -AssemblyName System.IO.Compression, System.IO.Compression.FileSystem
$stageFull = (Resolve-Path $stage).Path
$baseLen = (Split-Path $stageFull -Parent).Length + 1
$archive = [System.IO.Compression.ZipFile]::Open(
    [System.IO.Path]::GetFullPath($zip), 'Create')
try {
    Get-ChildItem -Path $stageFull -Recurse -File | ForEach-Object {
        $entry = $_.FullName.Substring($baseLen).Replace('\', '/')
        [void][System.IO.Compression.ZipFileExtensions]::CreateEntryFromFile(
            $archive, $_.FullName, $entry, 'Optimal')
    }
} finally {
    $archive.Dispose()
}
Write-Host "==> $zip"
Get-Item $zip | Select-Object Name, @{n='SizeMB';e={[math]::Round($_.Length/1MB,1)}}
