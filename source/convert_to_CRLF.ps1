$rootDir = Get-Location

# 获取所有文件，包括子目录中的文件
Get-ChildItem -Path $rootDir -Recurse -File | ForEach-Object {
    $filePath = $_.FullName
    Write-Host "Processing file: $filePath"
    
    # 读取文件内容，替换所有的 LF (\n) 换行符为 CRLF (\r\n)
    $content = Get-Content -Path $filePath -Raw
    $content = $content -replace "`n", "`r`n"

    # 将修改后的内容写回文件
    Set-Content -Path $filePath -Value $content -Force
}

Write-Host "All files have been processed."
