<#
.SYNOPSIS
    Build and execution script for Windows MS-MPI / MinGW environments.
.DESCRIPTION
    Compiles and executes the 4 MPI distributed programs.
#>

param (
    [string]$Action = "build", # "build", "run-p2p", "run-bidi", "run-pingpong", "run-compute", "clean"
    [string]$HostFile = "hosts.txt"
)

$BinDir = Join-Path $PSScriptRoot "bin"
$SrcDir = Join-Path $PSScriptRoot "src"

if (!(Test-Path $BinDir)) {
    New-Item -ItemType Directory -Path $BinDir | Out-Null
}

function Check-MPI {
    $mpicc = Get-Command mpicc -ErrorAction SilentlyContinue
    $mpiexec = Get-Command mpiexec -ErrorAction SilentlyContinue

    if (-not $mpiexec) {
        Write-Warning "mpiexec is not found in PATH. Please install MS-MPI or OpenMPI."
    }
    return [bool]$mpiexec
}

function Build-All {
    Write-Host "==========================================" -ForegroundColor Cyan
    Write-Host " Compiling MPI Distributed Programs...     " -ForegroundColor Cyan
    Write-Host "==========================================" -ForegroundColor Cyan

    $sources = @(
        "01_point_to_point.c",
        "02_bidirectional_exchange.c",
        "03_ping_pong_benchmark.c",
        "04_distributed_array_processing.c"
    )

    # Detect compiler: mpicc or gcc with MS-MPI include/lib
    $hasMpicc = [bool](Get-Command mpicc -ErrorAction SilentlyContinue)

    foreach ($src in $sources) {
        $baseName = [System.IO.Path]::GetFileNameWithoutExtension($src)
        $outExe = Join-Path $BinDir "$baseName.exe"
        $srcPath = Join-Path $SrcDir $src

        Write-Host "Compiling: $src -> bin/$baseName.exe" -ForegroundColor Green
        if ($hasMpicc) {
            & mpicc -O2 $srcPath -o $outExe -lm
        } elseif (Test-Path "C:\Program Files (x86)\Microsoft SDKs\MPI\Include\mpi.h") {
            # MS-MPI with GCC / MinGW
            $mpiInc = "C:\Program Files (x86)\Microsoft SDKs\MPI\Include"
            $mpiLib = "C:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64\msmpi.lib"
            & gcc -O2 -I"$mpiInc" $srcPath "$mpiLib" -o $outExe -lm
        } else {
            Write-Error "No MPI compiler wrapper (mpicc) or MS-MPI SDK detected. Please see README.md for setup instructions."
            return
        }
    }
    Write-Host "All targets compiled successfully into ./bin/" -ForegroundColor Cyan
}

switch ($Action) {
    "build" { Build-All }
    "run-p2p" { & mpiexec -n 2 (Join-Path $BinDir "01_point_to_point.exe") }
    "run-bidi" { & mpiexec -n 2 (Join-Path $BinDir "02_bidirectional_exchange.exe") }
    "run-pingpong" { & mpiexec -n 2 (Join-Path $BinDir "03_ping_pong_benchmark.exe") }
    "run-compute" { & mpiexec -n 2 (Join-Path $BinDir "04_distributed_array_processing.exe") }
    "clean" {
        Remove-Item -Recurse -Force $BinDir
        Write-Host "Cleaned ./bin/" -ForegroundColor Yellow
    }
    default {
        Write-Host "Usage: .\build.ps1 -Action [build | run-p2p | run-bidi | run-pingpong | run-compute | clean]"
    }
}
