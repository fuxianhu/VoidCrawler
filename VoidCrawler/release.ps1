param(
    [string]$ProjectPath = "E:\YFY\VoidCrawler"
)

Write-Host "   VoidCrawler 客户端发布脚本" -ForegroundColor Cyan
Write-Host ""
Write-Host "   选项 Options: " -ForegroundColor Cyan
Write-Host "   [string]ProjectPath = E:\YFY\VoidCrawler" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "正在切换到项目目录... ($ProjectPath\VoidCrawler\)" -ForegroundColor Cyan
Set-Location "$ProjectPath\VoidCrawler\"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "正在检测环境..." -ForegroundColor Cyan
$exePath = ".\x64\Release\VoidCrawler.exe"
if (-not (Test-Path $exePath)) {
    Write-Host "错误：未找到 $exePath" -ForegroundColor Red
    Read-Host "按 Enter 键退出"
    exit 1
}

if (Get-Process -Name "devenv" -ErrorAction SilentlyContinue) {
    Write-Host "错误：检测到 devenv.exe (Visual Studio) 正在运行，请先退出它。" -ForegroundColor Red
    Read-Host "按 Enter 键退出"
    exit 1
}

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "正在删除临时或不必要的文件/目录..." -ForegroundColor Cyan
if (Test-Path ".\crash_reports\") {
    Write-Host "删除目录: crash_reports ..." -ForegroundColor Yellow
    Remove-Item ".\crash_reports\" -Recurse -Force
}
if (Test-Path ".\log\") {
    Write-Host "删除目录: log ..." -ForegroundColor Yellow
    Remove-Item ".\log\" -Recurse -Force
}
if (Test-Path ".\.vs\") {
    Write-Host "删除目录: .vs ..." -ForegroundColor Yellow
    Remove-Item ".\.vs\" -Recurse -Force
}
if (Test-Path ".\VoidCrawler\") {
    Write-Host "删除目录: VoidCrawler ..." -ForegroundColor Yellow
    Remove-Item ".\VoidCrawler\" -Recurse -Force
}
if (Test-Path ".\x64\Debug\") {
    Write-Host "删除目录: x64\Debug ..." -ForegroundColor Yellow
    Remove-Item ".\x64\Debug\" -Recurse -Force
}

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "正在发布应用程序... (E:\Qt\6.5.3\msvc2019_64\bin\windeployqt --release .\VoidCrawler.exe)" -ForegroundColor Cyan
Set-Location "$ProjectPath\VoidCrawler\x64\Release\"
& "E:\Qt\6.5.3\msvc2019_64\bin\windeployqt" --release .\VoidCrawler.exe
Set-Location "$ProjectPath\VoidCrawler\"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "正在复制运行时必要文件..." -ForegroundColor Cyan
$foldersToCopy = @("audio", "config", "icon", "translations")
foreach ($folder in $foldersToCopy) {
    $sourceFolder = ".\$folder"
    $destFolder = ".\x64\Release\$folder"
    if (Test-Path $sourceFolder -PathType Container) {
        Write-Host "正在复制文件夹：$folder" -ForegroundColor Green
        if (-not (Test-Path $destFolder)) {
            New-Item -ItemType Directory -Path $destFolder -Force | Out-Null
        }
        Copy-Item -Path "$sourceFolder\*" -Destination $destFolder -Recurse -Force
    }
}

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "正在打包文件为 zip 压缩包..." -ForegroundColor Cyan

$sourceDir = ".\x64\Release"
$zipFile = Join-Path $sourceDir "VoidCrawlerClient.zip"

try {
    Compress-Archive -Path "$sourceDir\*" -DestinationPath $zipFile -Force -CompressionLevel Optimal
    Write-Host "成功创建ZIP文件: $zipFile" -ForegroundColor Green
    $fileInfo = Get-Item $zipFile
    $fileSize = $fileInfo.Length
    if ($fileSize -ge 1GB) {
        $displaySize = "{0:N2} GB" -f ($fileSize / 1GB)
    } elseif ($fileSize -ge 1MB) {
        $displaySize = "{0:N2} MB" -f ($fileSize / 1MB)
    } elseif ($fileSize -ge 1KB) {
        $displaySize = "{0:N2} KB" -f ($fileSize / 1KB)
    } else {
        $displaySize = "$fileSize 字节"
    }
    Write-Host "文件大小: $displaySize" -ForegroundColor Green
} catch {
    Write-Host "错误: 创建ZIP文件失败!" -ForegroundColor Red
    Write-Host "错误详情: $_" -ForegroundColor Red
    Read-Host "按 Enter 键退出"
    exit 1
}

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  发布成功！" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Github Repository: https://github.com/fuxianhu/VoidCrawler/" -ForegroundColor Blue
Write-Host "  Github Releses: https://github.com/fuxianhu/VoidCrawler/releases" -ForegroundColor Blue
Write-Host "  Now, you can open Github Desktop Commit Changes." -ForegroundColor Blue
Write-Host "" -ForegroundColor Blue
Write-Host "  Have a good time!" -ForegroundColor Blue
Write-Host "========================================" -ForegroundColor Cyan
