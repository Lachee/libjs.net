# Set URLs and paths
$url = "https://github.com/llvm/llvm-project/releases/download/llvmorg-18.1.8/clang+llvm-18.1.8-x86_64-pc-windows-msvc.tar.xz"
$outputPath = "$env:TEMP\llvm.tar.xz"
$extractPath = "$env:TEMP\llvm"
$destinationPath = "C:\Program Files\LLVM"


# Create the extraction directory if it doesn't exist
try {
    New-Item -ItemType Directory -Force -Path $extractPath | Out-Null
} catch {
    Write-Error "Failed to create directory at $extractPath. Please check permissions and try again."
    exit 1
}

Write-Host "Downloading LLVM..."
# Download the file using TLS 1.2
[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
try {
    Invoke-WebRequest -Uri $url -OutFile $outputPath
} catch {
    Write-Error "Failed to download LLVM. Error: $_"
    exit 1
}

Write-Host "Extracting LLVM from $outputPath to $extractPath..."
Expand-7ZipArchive -Path $outputPath -Destination "$env:TEMP\tarball"   # Extract the .gz file
$tarFile = "$env:TEMP\tarball" | Get-ChildItem | Select-Object -First 1 # Find the .tar file we just extracted
Expand-7ZipArchive -Path $tarFile.FullName -Destination $extractPath    # Extract the .tar file

$firstFolder = $extractPath | Get-ChildItem | Select-Object -First 1    # Find the first folder of the extracted files
Write-Host "Moving contents from $($firstFolder.FullName) to $destinationPath..."
Get-ChildItem -Path $firstFolder.FullName | ForEach-Object {
    $destination = Join-Path $destinationPath $_.Name
    if (Test-Path $destination) {
        Write-Host "Removing existing item: $destination"
        Remove-Item -Path $destination -Recurse -Force
    }
    Move-Item -Path $_.FullName -Destination $destinationPath -Force
}

# Clean up temporary files
# Dont need to clean up files if we delete the entire VM
# Remove-Item $outputPath -Force -ErrorAction SilentlyContinue
# Remove-Item $extractPath -Force -ErrorAction SilentlyContinue
# Remove-Item "$env:TEMP\llvm.tar" -Force -ErrorAction SilentlyContinue

Write-Host "LLVM has been downloaded and extracted to $destinationPath"
# Update the PATH environment variable
$env:Path += ";$destinationPath\bin"

# Update the system environment variable
[System.Environment]::SetEnvironmentVariable("Path", $env:Path, [System.EnvironmentVariableTarget]::Machine)

# Update other environment variables if needed
[System.Environment]::SetEnvironmentVariable("LLVM_HOME", $destinationPath, [System.EnvironmentVariableTarget]::Machine)
[System.Environment]::SetEnvironmentVariable("CC", "$destinationPath\bin\clang.exe", [System.EnvironmentVariableTarget]::Machine)
[System.Environment]::SetEnvironmentVariable("CXX", "$destinationPath\bin\clang++.exe", [System.EnvironmentVariableTarget]::Machine)
Write-Host "Environment variables have been updated."