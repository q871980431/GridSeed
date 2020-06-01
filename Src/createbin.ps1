$exepath="D:\build\GirdSeed\windows\Debug\"

$nowpath=pwd
$binpath="bin"
$tmp = Test-Path "./$binpath"
if($tmp -eq $false)
{
    Write-Output "./bin is not exists"
    mkdir $binpath
}

cd $binpath
$includepath="include"
$tmp = Test-Path "./$includepath"
if($tmp -eq $false)
{
    Write-Output "$includepath is not exists"
    mkdir $includepath
}

$libpath="lib/win/debug"
$tmp = Test-Path "./$libpath"
if($tmp -eq $false)
{
    Write-Output "$libpath is not exists"
    mkdir -p $libpath
}

cd $nowpath

cp API/* ${binpath}/${includepath}/
cp Public/MultiSys.h ${binpath}/${includepath}/
cp Public/LinSys.h ${binpath}/${includepath}/
cp Public/WinSys.h ${binpath}/${includepath}/
cp Public/CircularBuffer.h ${binpath}/${includepath}/

#copy 可执行文件
cp $exepath/GridSeed.exe ${binpath}/${libpath}/
cp $exepath/GridSeed.lib ${binpath}/${libpath}/
cp $exepath/GridSeed.pdb ${binpath}/${libpath}/
cp Thridlibrary/windows/debug/*.lib ${binpath}/${libpath}/
cp Thridlibrary/windows/debug/*.dll ${binpath}/${libpath}/
echo "create end......"
