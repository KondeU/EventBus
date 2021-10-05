function Unzip([string]$sourceFile, [string]$targetFolder)
{
    if(!(Test-Path $targetFolder))
    {
        mkdir $targetFolder
    }
    $sourceFile = Resolve-Path $sourceFile
    $targetFolder = Resolve-Path $targetFolder
    $shellApp = New-Object -ComObject Shell.Application
    $zipFiles = $shellApp.NameSpace($sourceFile).Items()
    $shellApp.NameSpace($targetFolder).CopyHere($zipFiles, 0x10)
}

if(!(Test-Path "patch"))
{
    Unzip "patch-2.5.9-7-bin.zip"     "patch"
    Unzip "diffutils-2.8.7-1-bin.zip" "diff"
    Unzip "diffutils-2.8.7-1-dep.zip" "diff"
    Unzip "zeromq-4.3.4.zip"          "."
    Unzip "cppzmq-4.7.1.zip"          "."
    Unzip "cereal-1.3.0.zip"          "."
    Unzip "msgpack-c-cpp-3.3.0.zip"   "."

    Rename-Item "zeromq-4.3.4"        "zeromq"
    Rename-Item "cppzmq-4.7.1"        "cppzmq"
    Rename-Item "cereal-1.3.0"        "cereal"
    Rename-Item "msgpack-c-cpp-3.3.0" "msgpack"

    Remove-Item "patch\bin\patch.exe"
    Unzip "patch-2.5.9_from_visual_studio_2017_strawberry_c_bin.zip" "patch\bin"
}
