param($installPath, $toolsPath, $package) 

function Copy-Natvis($DestFolder)
{
    if ((Test-Path $DestFolder) -eq $True)
    {
        $DestFile = Join-Path -path $DestFolder -childpath "autowiring.natvis";
        $SrcFile = Join-Path -path $toolsPath -childpath "autowiring.natvis";
        Copy-Item $SrcFile $DestFile;
    }
}

$VS2012Folder = Join-Path -path $env:userprofile -childpath "Documents\Visual Studio 2012\Visualizers";
$VS2013Folder = Join-Path -path $env:userprofile -childpath "Documents\Visual Studio 2013\Visualizers";
Copy-Natvis $VS2012Folder;
Copy-Natvis $VS2013Folder;