# VoidCrawler 客户端发布脚本
Write-Host "VoidCrawler 客户端发布脚本" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host

# 切换到项目目录
Write-Host "========================================" -ForegroundColor Yellow
Write-Host "正在切换到项目目录... (E:\YFY\VoidCrawler\VoidCrawler\)" -ForegroundColor Yellow
Set-Location "E:\YFY\VoidCrawler\VoidCrawler\"

# 检查可执行文件是否存在
Write-Host "========================================" -ForegroundColor Yellow
Write-Host "正在检测 .\x64\Release\VoidCrawler.exe 是否存在..." -ForegroundColor Yellow
$exePath = ".\x64\Release\VoidCrawler.exe"
if (-not (Test-Path $exePath)) {
    Write-Host "错误：未找到 $exePath" -ForegroundColor Red
    Read-Host "按 Enter 键退出"
    exit 1
}

# 发布应用程序
Write-Host "========================================" -ForegroundColor Yellow
Set-Location "E:\YFY\VoidCrawler\VoidCrawler\x64\Release\"
Write-Host "正在发布应用程序... (E:\Qt\6.5.3\msvc2019_64\bin\windeployqt --release .\VoidCrawler.exe)" -ForegroundColor Yellow
& "E:\Qt\6.5.3\msvc2019_64\bin\windeployqt" --release .\VoidCrawler.exe
Set-Location "E:\YFY\VoidCrawler\VoidCrawler\"

# 复制运行时必要文件
Write-Host "========================================" -ForegroundColor Yellow
Write-Host "正在复制运行时必要文件..." -ForegroundColor Yellow

$foldersToCopy = @("audio", "config", "icon", "translations")
foreach ($folder in $foldersToCopy) {
    $sourceFolder = ".\$folder"
    $destFolder = ".\x64\Release\$folder"
    
    if (Test-Path $sourceFolder -PathType Container) {
        Write-Host "正在复制文件夹：$folder" -ForegroundColor Green
        
        # 确保目标文件夹存在
        if (-not (Test-Path $destFolder)) {
            New-Item -ItemType Directory -Path $destFolder -Force | Out-Null
        }
        
        # 复制文件夹内容
        Copy-Item -Path "$sourceFolder\*" -Destination $destFolder -Recurse -Force
    }
}

# 打包文件为 ZIP 压缩包
Write-Host "========================================" -ForegroundColor Yellow
Write-Host "正在打包文件为 zip 压缩包..." -ForegroundColor Yellow

$sourceDir = ".\x64\Release"
$zipFile = Join-Path $sourceDir "VoidCrawlerClient.zip"

# 压缩文件夹
try {
    Compress-Archive -Path "$sourceDir\*" -DestinationPath $zipFile -Force -CompressionLevel Optimal
    Write-Host "成功创建ZIP文件: $zipFile" -ForegroundColor Green
    
    # 获取文件大小并格式化显示
    $fileInfo = Get-Item $zipFile
    $fileSize = $fileInfo.Length
    
    # 格式化文件大小为更易读的格式
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

# 完成
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "      发布成功！" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan